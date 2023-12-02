/*
 * 	Copyright (c) 2019-2024, Renee Koecher
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

#pragma once

#include "Delegate.h"

#include <Windows.h>

#include <unordered_map>
#include <vector>
#include <string>
#include <atomic>

namespace XiPivot
{
	namespace Core
	{
		/* in-memory file cache to hopefully speed up XI's loading times
		 *
		 * the current version is based on the observed read behavior of POL:
		 * - a file is opened for reading
		 * - the files contents are sequencially read into memory in chunks
		 * - the file is closed
		 *
		 * POL doesn't seem to seek inside the file so this case is not currently handled.
		 */
		class MemCache
		{
			/* signature types for the Win32 API methods we're going to hook */
		private:
			typedef BOOL(WINAPI * pFnReadFile)(
				HANDLE       hRef,
				LPVOID       lpBuffer,
				DWORD        nNumberOfBytesToRead,
				LPDWORD      lpNumberOfBytesRead,
				LPOVERLAPPED lpOverlapped
				);

			typedef BOOL(WINAPI* pFnCloseHandle)(
				HANDLE       hRef
				);

			/* representation of a single cached file */
			struct CacheObject
			{
				size_t   size;
				PBYTE    data;

				time_t   lastUse;

				std::atomic_int ref;
			};

			/* pointer from a HANDLE to a cache object */
			struct CachePointer
			{
				int32_t  pathKey;
			};

		public:
			/* internal statistics tracking */
			struct CacheStatus
			{
				size_t   used;
				size_t   allocation;

				unsigned cacheHits;
				unsigned cacheMisses;
				unsigned cacheIgnored;

				unsigned activeObjects;
			};

		public:
			virtual ~MemCache(void);

			/* setup and tear-down of syscall hooks*/
			bool setupHooks(void);
			bool releaseHooks(void);

			bool hooksActive(void) const { return m_hooksSet; };

			/* change the active log provider */
			void setLogProvider(IDelegate *logProvider);

			/* toggle debug logging on/off */
			void setDebugLog(bool state);

			/* get the current state of debug logging */
			bool getDebugLog(void) const { return m_logDebug != IDelegate::LogLevel::Discard; }

			/* change the maximum allowed cache size (in byte) */
			void setCacheAllocation(size_t allocationSize);

			/* get the current maximum allowed cache size (in byte) */
			size_t getCacheAllocation(void) const { return m_stats.allocation; }
		
			/* track and cache a file handle for a given key */
			HANDLE trackCacheObject(HANDLE hRef, int32_t pathKey);

			/** trigger a purge of cache objects of a certain age 
			 * @param maxAge maximum time since last access (in seconds)
			 */
			size_t purgeCacheObjects(time_t maxAge);

			/* return a copy of the cache usage statistics */
			CacheStatus getCacheStats(void) const { return m_stats; };

		public:
			/* access or create the actual Redirector instance */
			static MemCache& instance(void);

			/* static callbacks used by the Detours library */
			static BOOL __stdcall dReadFile(HANDLE a0, LPVOID a1, DWORD a2, LPDWORD a3, LPOVERLAPPED a4);
			static BOOL __stdcall dCloseHandle(HANDLE a0);

		private /* static */:

			static pFnReadFile s_procReadFile;
			static pFnCloseHandle s_procCloseHandle;

		protected:
			/* globally unique instance pointer */
			static MemCache* s_instance;

			/* it would be foolish to allow more than one Redirector to be created,
			   as such this whole class is designed as a Singleton.
			 */
			explicit MemCache(void);

		private:
			/* actual code to handle the intercept / redirect of file names */
			BOOL __stdcall interceptReadFile(HANDLE a0, LPVOID a1, DWORD a2, LPDWORD a3, LPOVERLAPPED a4);
			BOOL __stdcall interceptCloseHandle(HANDLE a0);

			/** create or fetch a cached version of a file handle 
			 * @param hRef - if not nullptr will be used to create a new object if it doesn't exist
			 */
			CacheObject* getCachedObject(HANDLE hRef, int32_t pathKey);
			bool readObjectData(HANDLE hRef, CacheObject& obj);

			bool performCachedRead(HANDLE hRef, LPVOID lpBuffer, DWORD bytesToRead, LPDWORD bytesRead);

			bool                                        m_hooksSet;

			CacheStatus                                 m_stats;
			std::atomic_bool                            m_inSyscall;

			std::unordered_map<ptrdiff_t, CachePointer> m_cachePointers;
			std::unordered_map<int32_t, CacheObject*>   m_cacheObjects;

			IDelegate::LogLevel                      m_logDebug;
			IDelegate*                               m_logger;

		};
	}
}

