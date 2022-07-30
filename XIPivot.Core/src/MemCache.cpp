/*
 * 	Copyright © 2019-2022, Renee Koecher
 * 	All rights reserved.
 * 
 * 	Redistribution and use in source and binary forms, with or without
 * 	modification, are permitted provided that the following conditions are met :
 * 
 * 	* Redistributions of source code must retain the above copyright
 * 	  notice, this list of conditions and the following disclaimer.
 * 	* Redistributions in binary form must reproduce the above copyright
 * 	  notice, this list of conditions and the following disclaimer in the
 * 	  documentation and/or other materials provided with the distribution.
 * 	* Neither the name of XIPivot nor the
 * 	  names of its contributors may be used to endorse or promote products
 * 	  derived from this software without specific prior written permission.
 * 
 * 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * 	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * 	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 	DISCLAIMED.IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * 	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * 	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * 	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * 	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * 	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * 	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "MemCache.h"
#include "detours.h"

#include <cctype>
#include <ctime>

namespace XiPivot
{
	namespace Core
	{
		/* static member initialisation */

		namespace {
			static constexpr size_t sMaxCacheObjectSize = 104857599U; // 100MB -1byte
		}
		MemCache* MemCache::s_instance = nullptr;

		MemCache::pFnReadFile    MemCache::s_procReadFile = ReadFile;
		MemCache::pFnCloseHandle MemCache::s_procCloseHandle = CloseHandle;

		/* static interface */
		MemCache& MemCache::instance(void)
		{
			if (MemCache::s_instance == nullptr)
			{
				MemCache::s_instance = new MemCache();
			}
			return *MemCache::s_instance;
		}

		BOOL __stdcall MemCache::dReadFile(HANDLE a0, LPVOID a1, DWORD a2, LPDWORD a3, LPOVERLAPPED a4)
		{
			/* don't use the Singleton access here;
			 * if for whatever reason the global object is gone we don't want a new one
			 */
			if (MemCache::s_instance != nullptr || a4 != nullptr)
			{
				return MemCache::s_instance->interceptReadFile(a0, a1, a2, a3, a4);
			}
			return MemCache::s_procReadFile(a0, a1, a2, a3, a4);
		}

		BOOL __stdcall MemCache::dCloseHandle(HANDLE a0)
		{
			/* don't use the Singleton access here;
			 * if for whatever reason the global object is gone we don't want a new one
			 */
			if (MemCache::s_instance != nullptr)
			{
				return MemCache::s_instance->interceptCloseHandle(a0);
			}
			return MemCache::s_procCloseHandle(a0);
		}

		MemCache::MemCache()
			: m_hooksSet(false),
			  m_stats({ 0, 0, 0, 0, 0, 0 }),
			  m_logDebug(ILogProvider::LogLevel::Discard)
		{
			m_logger = DummyLogProvider::instance();
		}

		MemCache::~MemCache()
		{
			releaseHooks(); // just in case

			m_cachePointers.clear();

			purgeCacheObjects(0);
			m_cacheObjects.clear();
		}

		void MemCache::setLogProvider(ILogProvider* newLogProvider)
		{
			if (newLogProvider == nullptr)
			{
				return;
			}
			m_logger = newLogProvider;
		}

		bool MemCache::setupHooks(void)
		{
			if (m_hooksSet == false)
			{
				/* hooks need to be set */
				m_hooksSet = true;
				DetourTransactionBegin();
				DetourUpdateThread(GetCurrentThread());

				DetourAttach(&(PVOID&)MemCache::s_procReadFile, MemCache::dReadFile);
				DetourAttach(&(PVOID&)MemCache::s_procCloseHandle, MemCache::dCloseHandle);

				m_hooksSet = DetourTransactionCommit() == NO_ERROR;

				m_logger->logMessageF(ILogProvider::LogLevel::Info, "m_hooksSet = %s", m_hooksSet ? "true" : "false");
				return m_hooksSet;
			}
			m_logger->logMessage(m_logDebug, "hooks already set");
			return false;
		}

		bool MemCache::releaseHooks(void)
		{
			if (m_hooksSet == true)
			{
				m_hooksSet = false;
				DetourTransactionBegin();
				DetourUpdateThread(GetCurrentThread());

				DetourDetach(&(PVOID&)MemCache::s_procReadFile, MemCache::dReadFile);
				DetourDetach(&(PVOID&)MemCache::s_procCloseHandle, MemCache::dCloseHandle);

				m_hooksSet = (DetourTransactionCommit() == NO_ERROR) ? false : true;

				m_logger->logMessageF(ILogProvider::LogLevel::Info, "m_hooksSet = %s", m_hooksSet ? "true" : "false");
				return m_hooksSet;
			}
			m_logger->logMessage(m_logDebug, "hooks already removed");
			return false;
		}

		void MemCache::setDebugLog(bool state)
		{
			m_logDebug = (state) ? ILogProvider::LogLevel::Debug : ILogProvider::LogLevel::Discard;
			m_logger->logMessageF(ILogProvider::LogLevel::Info, "m_logDebug = %s", state ? "Debug" : "Discard");
		}

		void MemCache::setCacheAllocation(size_t allocationSize)
		{
			/* this changes the allowed allocation but it does not trigger a cache purge */
			m_logger->logMessageF(ILogProvider::LogLevel::Info, "changing cache allocation to %dMB", allocationSize / 0x100000);
			m_stats.allocation = allocationSize;
		}
	
		/* track and cache a file handle for a given key */
		HANDLE MemCache::trackCacheObject(HANDLE hRef, int32_t pathKey)
		{
			if (m_stats.allocation != 0 && hRef != nullptr && pathKey != -1)
			{
				if (m_cachePointers.find(reinterpret_cast<ptrdiff_t>(hRef)) == m_cachePointers.end())
				{

					auto cacheObj = getCachedObject(hRef, pathKey);
					if (cacheObj != nullptr)
					{
						++cacheObj->ref;
						m_cachePointers.emplace(reinterpret_cast<ptrdiff_t>(hRef), CachePointer({ pathKey }));
						m_logger->logMessageF(m_logDebug, "started to track HANDLE %p => %d", hRef, pathKey);
					}
				}
			}
			return hRef;
		}

		size_t MemCache::purgeCacheObjects(time_t maxAge)
		{
			if (m_inSyscall)
			{
				return 0;
			}

			size_t objectsPurged = 0;
			time_t oldAge = time(nullptr) - maxAge;

			for (auto it = m_cacheObjects.cbegin(); it != m_cacheObjects.cend();)
			{
				if (it->second->lastUse < oldAge && it->second->ref < 1)
				{
					if (m_inSyscall == false)
					{

						m_logger->logMessageF(m_logDebug, "purgeCacheObjects: removing %d (%zd bytes)", it->first, it->second->size);

						auto obj = it->second;
						m_cacheObjects.erase(it++);

						m_stats.used -= obj->size;
						--m_stats.activeObjects;

						delete[] obj->data;
						delete obj;

						++objectsPurged;
					}
				}
				else
				{
					++it;
				}
			}

			m_stats.cacheIgnored = 0;

			return objectsPurged;
		}

		/* static hooks */

		BOOL __stdcall
			MemCache::interceptReadFile(HANDLE a0, LPVOID a1, DWORD a2, LPDWORD a3, LPOVERLAPPED a4)
		{
			m_inSyscall.store(true);
			if (performCachedRead(a0, a1, a2, a3) == true)
			{
				m_inSyscall.store(false);
				return true;
			}
			m_inSyscall.store(false);
			return MemCache::s_procReadFile(a0, a1, a2, a3, a4);
		}

		BOOL __stdcall
			MemCache::interceptCloseHandle(HANDLE a0)
		{
			m_inSyscall.store(true);
			const auto it = m_cachePointers.find(reinterpret_cast<ptrdiff_t>(a0));
			if (it != m_cachePointers.end())
			{
				m_logger->logMessageF(m_logDebug, "stopped tracking HANDLE %p", a0);

				auto cacheObj = getCachedObject(nullptr, it->second.pathKey);
				if (cacheObj != nullptr)
				{
					--cacheObj->ref;
				}
				m_cachePointers.erase(it);
			}
			m_inSyscall.store(false);
			return MemCache::s_procCloseHandle(a0);
		}

		/* private stuff */

		MemCache::CacheObject* MemCache::getCachedObject(HANDLE hRef, int32_t pathKey)
		{
			auto cachedObj = m_cacheObjects.find(pathKey);
			if (cachedObj != m_cacheObjects.end())
			{
				++m_stats.cacheHits;
				return cachedObj->second;
			}

			if (hRef == nullptr)
			{
				/* not in cache and no way to create it */
				++m_stats.cacheMisses;
				return nullptr;
			}

			size_t size = GetFileSize(hRef, nullptr);
			if (size > sMaxCacheObjectSize)
			{
				/* do NOT cache objects above sMaxCacheObjectSize lower the risk of "blackouts"
				 * caused by XI running out of available memory */
				m_logger->logMessageF(ILogProvider::LogLevel::Debug, "getCachedObject: object size exceeds limit, no cache object created.");
				++m_stats.cacheIgnored;
				return nullptr;
			}

			/* try and create a new object on the fly and store it */
			CacheObject* obj = new (std::nothrow) CacheObject;
			if (obj != nullptr)
			{
				if (m_stats.used + size <= m_stats.allocation)
				{
					memset(obj, 0, sizeof(CacheObject));
					obj->data = new (std::nothrow) BYTE[size];
					obj->size = size;
					obj->ref = 0;

					if (obj->data != nullptr)
					{
						/* read the actual data into memory */

						if(readObjectData(hRef, *obj))
						{
							m_stats.used += obj->size;
							++m_stats.activeObjects;

							obj->lastUse = time(nullptr);
							m_cacheObjects.emplace(pathKey, obj);

							m_logger->logMessageF(m_logDebug, "getCachedObject: created cache object for %p => %zd bytes", hRef, obj->size);

							++m_stats.cacheMisses;
							return obj;
						}

						delete[] obj->data;
					}
				}
				else
				{
					m_logger->logMessageF(ILogProvider::LogLevel::Warn, "getCachedObject: cache limit exceeded");
				}
				delete obj;
			}
			++m_stats.cacheMisses;
			return nullptr;
		}

		bool MemCache::readObjectData(HANDLE hRef, CacheObject& obj)
		{
			if (hRef == nullptr || obj.data == nullptr)
			{
				return false;
			}

			SetFilePointer(hRef, 0, nullptr, FILE_BEGIN);

			size_t readSize = 0;
			while (readSize < obj.size)
			{
				DWORD bytesRead = 0;
				if (MemCache::s_procReadFile(hRef, &obj.data[readSize], obj.size - readSize, &bytesRead, nullptr) == FALSE)
				{
					m_logger->logMessageF(ILogProvider::LogLevel::Warn, "readObjectData: aborting read with %zd / %zd bytes", readSize, obj.size);
					break;
				}

				readSize += bytesRead;

				if (bytesRead == 0)
				{
					break;
				}
			}

			SetFilePointer(hRef, 0, nullptr, FILE_BEGIN);
			return readSize == obj.size;
		}

		bool MemCache::performCachedRead(HANDLE hRef, LPVOID lpBuffer, DWORD bytesToRead, LPDWORD bytesRead)
		{
			if (hRef == nullptr || lpBuffer == nullptr)
			{
				return false;
			}

			const auto pointer = m_cachePointers.find(reinterpret_cast<ptrdiff_t>(hRef));
			if (pointer == m_cachePointers.end())
			{
				/* not tracked */
				return false;
			}

			const auto cacheObj = getCachedObject(nullptr, pointer->second.pathKey);
			if (cacheObj == nullptr)
			{
				/* tracked but no longer cached -- this should actually NEVER EVER HAPPEN */
				m_logger->logMessageF(ILogProvider::LogLevel::Error, "performCachedRead: cache object for %p (%d) vanished, untracking HANDLE.", hRef, pointer->second.pathKey);
				m_cachePointers.erase(pointer);

				return MemCache::s_procReadFile(hRef, lpBuffer, bytesToRead, bytesRead, nullptr) == TRUE;
			}

			/* everything valid - touch the object to keep it alive */
			cacheObj->lastUse = time(nullptr);

			/* update the stored offset in case the handle was seeked in */
			DWORD fileOffset = SetFilePointer(hRef, 0, nullptr, FILE_CURRENT);
			if (fileOffset >= cacheObj->size)
			{
				/* already at end of file */
				bytesToRead = 0;
			}
			else if (fileOffset + bytesToRead > cacheObj->size)
			{
				/* read to end of file */
				bytesToRead = cacheObj->size - fileOffset;
			}

			if (bytesToRead > 0)
			{
				memcpy(lpBuffer, &cacheObj->data[fileOffset], bytesToRead);
				fileOffset += bytesToRead;
			}

			if (bytesRead != nullptr)
			{
				*bytesRead = bytesToRead;
			}

			/* update the real file pointer in case it is used with other API methods */
			SetFilePointer(hRef, bytesToRead, nullptr, FILE_CURRENT);
			return true;
		}
	}
}
