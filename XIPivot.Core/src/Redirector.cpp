/*
 * 	Copyright © 2019, Renee Koecher
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

#include "Redirector.h"
#include "detours.h"

#include <algorithm>

#ifdef _DEBUG
#	define _dbgLog(...) this->dbgLog(__VA_ARGS__)
#else
#	define _dbgLog(...)
#endif

namespace XiPivot
{
	namespace Core
	{
		/* static member initialisation */

		Redirector* Redirector::s_instance = nullptr;

		Redirector::pFnCreateFileA    Redirector::s_procCreateFileA = CreateFileA;
		Redirector::pFnFindFirstFileA Redirector::s_procFindFirstFileA = FindFirstFileA;

		/* static interface */
		Redirector& Redirector::instance(void)
		{
			if (Redirector::s_instance == nullptr)
			{
				Redirector::s_instance = new Redirector();
			}
			return *Redirector::s_instance;
		}

		HANDLE __stdcall Redirector::dCreateFileA(LPCSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5, HANDLE a6)
		{
			/* don't use the Singleton access here;
			 * if for whatever reason the global object is gone we don't want a new one
			 */
			if (Redirector::s_instance != nullptr)
			{
				return Redirector::s_instance->interceptCreateFileA(a0, a1, a2, a3, a4, a5, a6);
			}
			return Redirector::s_procCreateFileA(a0, a1, a2, a3, a4, a5, a6);
		}

		HANDLE __stdcall Redirector::dFindFirstFileA(LPCSTR a0, LPWIN32_FIND_DATAA a1)
		{
			/* don't use the Singleton access here;
			 * if for whatever reason the global object is gone we don't want a new one
			 */
			if (Redirector::s_instance != nullptr)
			{
				return Redirector::s_instance->interceptFindFirstFileA(a0, a1);
			}
			return Redirector::s_procFindFirstFileA(a0, a1);
		}

		Redirector::Redirector()
			: m_hooksSet(false)
		{
			char workDir[MAX_PATH];

			m_resolvedPaths.clear();
			m_overlayPaths.clear();

			GetCurrentDirectoryA(sizeof(workDir), workDir);

			m_rootPath = workDir;

#ifdef _DEBUG
			fopen_s(&m_dbgLog, "XIPivot.dbg.log", "a");
#endif
		}

		Redirector::~Redirector()
		{
			releaseHooks(); // just in case

#ifdef _DEBUG
			if (m_dbgLog != nullptr)
			{
				fflush(m_dbgLog);
				fclose(m_dbgLog);
				m_dbgLog = nullptr;
			}
#endif
		}

		bool Redirector::setupHooks(void)
		{
			if (m_hooksSet == false)
			{
				/* hooks need to be set */
				m_hooksSet = true;
				DetourTransactionBegin();
				DetourUpdateThread(GetCurrentThread());

				DetourAttach(&(PVOID&)Redirector::s_procCreateFileA, Redirector::dCreateFileA);
				DetourAttach(&(PVOID&)Redirector::s_procFindFirstFileA, Redirector::dFindFirstFileA);

				m_hooksSet = DetourTransactionCommit() == NO_ERROR;
				return m_hooksSet;
			}
			return false;
		}

		bool Redirector::releaseHooks(void)
		{
			if (m_hooksSet == true)
			{
				m_hooksSet = false;
				DetourTransactionBegin();
				DetourUpdateThread(GetCurrentThread());

				DetourDetach(&(PVOID&)Redirector::s_procCreateFileA, Redirector::dCreateFileA);
				DetourDetach(&(PVOID&)Redirector::s_procFindFirstFileA, Redirector::dFindFirstFileA);

				m_hooksSet = (DetourTransactionCommit() == NO_ERROR) ? false : true;
				return m_hooksSet;
			}
			return false;
		}

		void Redirector::setRootPath(const std::string &newRoot)
		{
			m_rootPath = newRoot;
			m_resolvedPaths.clear();
			for (const auto& overlay : m_overlayPaths)
			{
				std::string localPath = m_rootPath + "/" + overlay;
				scanOverlayPath(overlay);
			}
		}

		bool Redirector::addOverlay(const std::string &overlayPath)
		{
			if (std::find(m_overlayPaths.begin(), m_overlayPaths.end(), overlayPath) == m_overlayPaths.end())
			{
				std::string localPath = m_rootPath + "/" + overlayPath;
				if (scanOverlayPath(localPath))
				{
					m_overlayPaths.emplace_back(overlayPath);
					return true;
				}
			}
			return false;
		}

		void Redirector::removeOverlay(const std::string &overlayPath)
		{
			auto it = std::find(m_overlayPaths.begin(), m_overlayPaths.end(), overlayPath);

			if (it != m_overlayPaths.end())
			{
				m_resolvedPaths.clear();
				m_overlayPaths.erase(it);
				for (auto& path : m_overlayPaths)
				{
					std::string localPath = m_rootPath + "/" + path;
					scanOverlayPath(localPath);
				}
			}
		}

		/* static hooks */

		HANDLE __stdcall
			Redirector::interceptCreateFileA(LPCSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5, HANDLE a6)
		{
			const char* path = findRedirect(a0);
			return Redirector::s_procCreateFileA((LPCSTR)path, a1, a2, a3, a4, a5, a6);
		}

		HANDLE __stdcall
			Redirector::interceptFindFirstFileA(LPCSTR a0, LPWIN32_FIND_DATAA a1)
		{
			const char* path = findRedirect(a0);
			return Redirector::s_procFindFirstFileA((LPCSTR)path, a1);
		}

		/* private stuff */
		const char *Redirector::findRedirect(const char *realPath)
		{
			const char *romPath = strstr(realPath, "//ROM");
			if (romPath == nullptr)
			{
				return realPath;
			}

			int32_t romIndex = pathToIndex(romPath);
			const auto res = m_resolvedPaths.find(romIndex);
			
			if(res != m_resolvedPaths.end())
			{
				return (*res).second.c_str();
			}
			return realPath;
		}

		bool Redirector::scanOverlayPath(const std::string &basePath)
		{
			/* crawl an overlay path and collect all the DATs 
			 * in m_resolvedPaths.
			 */
			bool res = false;

			std::vector<std::string> romDirs;
			if (collectSubPath(basePath, "//ROM*", romDirs, true))
			{
				for (const auto &p : romDirs)
				{
					std::vector<std::string> subDirs;
					if (collectSubPath(p, "/*", subDirs))
					{
						for (const auto &sp : subDirs)
						{
							std::vector<std::string> datFiles;
							if (collectDatFiles(sp, datFiles))
							{
								for (const auto &dat : datFiles)
								{
									int32_t romIndex = pathToIndex(strstr(dat.c_str(), "//ROM"));
									if (m_resolvedPaths.find(romIndex) == m_resolvedPaths.end())
									{
										m_resolvedPaths.emplace(romIndex, dat);
									}
								}
								/* at least one overlay file */
								res = true;
							}
						}
					}
				}
			}
			return res;
		}

		bool Redirector::collectSubPath(const std::string &basePath, const std::string &pattern, std::vector<std::string> &results, bool doubleDirSep)
		{
			HANDLE handle;
			WIN32_FIND_DATAA attrs;

			std::string searchPath = std::string(basePath) + std::string(pattern);

			results.clear();
			handle = Redirector::s_procFindFirstFileA((LPCSTR)searchPath.c_str(), &attrs);
			if (handle != INVALID_HANDLE_VALUE)
			{
				do
				{
					std::string filename = attrs.cFileName;

					if (attrs.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && filename != "." && filename != "..")
					{
						if (doubleDirSep)
						{
							/* this is only used to keep the same //ROM notation XI uses */
							results.emplace_back(basePath + "//" + attrs.cFileName);
						}
						else
						{
							results.emplace_back(basePath + "/" + attrs.cFileName);
						}
					}
				} while (FindNextFileA(handle, &attrs));
			}
			return results.size() != 0;
		}

		bool Redirector::collectDatFiles(const std::string &parentPath, std::vector<std::string> &results)
		{
			HANDLE handle;
			WIN32_FIND_DATAA attrs;

			std::string searchPath = parentPath + "/*.DAT";

			results.clear();
			handle = Redirector::s_procFindFirstFileA((LPCSTR)searchPath.c_str(), &attrs);
			if (handle != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (attrs.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
					{
						results.emplace_back(parentPath + "/" + attrs.cFileName);
					}
				} while (FindNextFileA(handle, &attrs));
			}
			return results.size() != 0;
		}

		int32_t Redirector::pathToIndex(const char *romPath)
		{
			/* **very** tailored approach to get a fast,
			 * unique index for every given //ROM* path
			 *
			 * it's build on the current ROM layout and
			 * **will break** if SE ever decides to add
			 * more than 999 sub folders or 9 ROM roots.
			 *
			 * every numeric part of the ROM path will 
			 * be extracted and added to the romIndex.
			 * directory separators multiply the number
			 * by 1000 and ensure the numbers don't collide.
			 *
			 * See the following paths and their resulting index:
			 *
			 * //ROM/0/0.DAT        =>        0
			 * //ROM/0/1.DAT        =>        1
			 * //ROM1/2/3.DAT       =>  1002003
			 * //ROM1/22/33.DAT     =>  1022033
			 * //ROM1/222/333.DAT   =>  1222333
			 * //ROM9/999/999.DAT   =>  9999999
			 * - anything invalid - =>       -1
			 */
			int32_t romIndex = -1;

			/* start at the first character after "//ROM"
			 * this is either a digit or '/' in case of the base "//ROM/"
			 * There is no specific check for paths shorter than 6
			 * characters, but then again, this method is not called on
			 * random strings either.
			 */
			char *p = (char*)&romPath[5];
			while (p && *p != '.' && *p != 0)
			{
				int subIndex = 0;
				/* cut out the number and add it to romIndex */
				for (; p && *p >= '0' && *p <= '9'; ++p)
				{
					subIndex *= 10;
					subIndex += (*p) - '0';
				}

				if (*p == '/')
				{
					/* skip the '/' and shift the ROM base left */
					romIndex *= 1000;
					romIndex += subIndex;
					++p;
				}
				else if (*p == '.')
				{
					/* break at the start of the file extension */
					romIndex *= 1000;
					romIndex += subIndex;
					break;
				}
				else
				{
					/* not a path we can handle :( */
					return -1;
				}
			}
			return romIndex;
		}

#ifdef _DEBUG
		void Redirector::dbgLog(const char *fmt, ...)
		{
			if (m_dbgLog != nullptr)
			{
				va_list args;
				va_start(args, fmt);
				vfprintf_s(m_dbgLog, fmt, args);
				fflush(m_dbgLog);
				va_end(args);
			}
		}
#endif
	}
}
