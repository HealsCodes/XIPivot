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

#include <cctype>
#include <algorithm>

#if (_DEBUG || _TEST)
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
#ifndef _TEST
			fopen_s(&m_dbgLog, "XIPivot.dbg.log", "a");
#endif
#endif
#ifdef _TEST
			m_dbgLog = stdout;
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

				_dbgLog("m_hooksSet = %s\n", m_hooksSet ? "true" : "false");
				return m_hooksSet;
			}
			_dbgLog("hooks already set\n");
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

				_dbgLog("m_hooksSet = %s\n", m_hooksSet ? "true" : "false");
				return m_hooksSet;
			}
			_dbgLog("hooks already removed\n");
			return false;
		}

		bool Redirector::setDebugLog(bool state)
		{
#if _DEBUG
			m_doDbgLog = state;
			_dbgLog("m_doDbgLog = %s", m_doDbgLog ? "true" : "false");
			return m_doDbgLog;
#else
			return false;
#endif
		}

		void Redirector::setRootPath(const std::string &newRoot)
		{
			m_rootPath = newRoot;
			m_resolvedPaths.clear();

			_dbgLog("m_rootPath = '%s'", m_rootPath.c_str());
			for (const auto& overlay : m_overlayPaths)
			{
				std::string localPath = m_rootPath + "/" + overlay;
				scanOverlayPath(overlay);
			}
		}

		bool Redirector::addOverlay(const std::string &overlayPath)
		{
			_dbgLog("'%s'\n", overlayPath.c_str());
			if (std::find(m_overlayPaths.begin(), m_overlayPaths.end(), overlayPath) == m_overlayPaths.end())
			{
				std::string localPath = m_rootPath + "/" + overlayPath;
				if (scanOverlayPath(localPath))
				{
					m_overlayPaths.emplace_back(overlayPath);
					_dbgLog("=> success\n");
					return true;
				}
			}
			_dbgLog("=> failed\n");
			return false;
		}

		void Redirector::removeOverlay(const std::string &overlayPath)
		{
			auto it = std::find(m_overlayPaths.begin(), m_overlayPaths.end(), overlayPath);

			_dbgLog("'%s'\n");
			if (it != m_overlayPaths.end())
			{
				m_resolvedPaths.clear();
				m_overlayPaths.erase(it);
				for (auto& path : m_overlayPaths)
				{
					std::string localPath = m_rootPath + "/" + path;
					scanOverlayPath(localPath);
				}
				_dbgLog("=> found, and removed\n");
			}
		}

		/* static hooks */

		HANDLE __stdcall
			Redirector::interceptCreateFileA(LPCSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5, HANDLE a6)
		{
			_dbgLog("lpFileName = '%s'\n", static_cast<const char*>(a0));

			const char* path = findRedirect(a0);
			return Redirector::s_procCreateFileA((LPCSTR)path, a1, a2, a3, a4, a5, a6);
		}

		HANDLE __stdcall
			Redirector::interceptFindFirstFileA(LPCSTR a0, LPWIN32_FIND_DATAA a1)
		{
			_dbgLog("lpFileName = '%s'\n", static_cast<const char*>(a0));

			const char* path = findRedirect(a0);
			return Redirector::s_procFindFirstFileA((LPCSTR)path, a1);
		}

		/* private stuff */
		const char *Redirector::findRedirect(const char *realPath)
		{
			const char *romPath = strstr(realPath, "//ROM");
			const char *sfxPath;

			if (strstr(realPath, "\\win\\se\\") != nullptr || strstr(realPath, "\\win\\music\\") != nullptr)
			{
				sfxPath = &(strstr(realPath, "\\win\\")[-1]);
			}
			else
			{
				sfxPath = nullptr;
			}

			_dbgLog("romPath = %d, sfxPath = %d\n", romPath != nullptr, sfxPath != nullptr);

			if (romPath != nullptr)
			{
				int32_t romIndex = pathToIndex(romPath);
				const auto res = m_resolvedPaths.find(romIndex);
				
				if(res != m_resolvedPaths.end())
				{
					_dbgLog("using overlay '%s'\n", (*res).second.c_str());
					return (*res).second.c_str();
				}
			}
			if (sfxPath != nullptr)
			{
				int32_t sfxIndex = pathToIndexAudio(sfxPath);
				const auto res = m_resolvedPaths.find(sfxIndex);
				
				if(res != m_resolvedPaths.end())
				{
					_dbgLog("using overlay '%s'\n", (*res).second.c_str());
					return (*res).second.c_str();
				}
			}
			return realPath;
		}

		bool Redirector::scanOverlayPath(const std::string &basePath)
		{
			/* crawl an overlay path and collect all the DATs 
			 * in m_resolvedPaths.
			 */
			bool res = false;

			_dbgLog("'%s'\n", basePath.c_str());

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
							if (collectDataFiles(sp, "*.DAT", datFiles))
							{
								for (const auto &dat : datFiles)
								{
									int32_t romIndex = pathToIndex(strstr(dat.c_str(), "//ROM"));
									if (m_resolvedPaths.find(romIndex) == m_resolvedPaths.end())
									{
										_dbgLog("emplace %8d : '%s'\n", romIndex, dat.c_str());
										m_resolvedPaths.emplace(romIndex, dat);
									}
									else
									{
										_dbgLog("WARNING: %8d: ignoring '%s'\n", romIndex, dat.c_str());
									}
								}
								/* at least one overlay file */
								res = true;
							}
						}
					}
				}
			}

			std::vector<std::string> soundDirs;
			if (collectSubPath(basePath, "/sound*", soundDirs))
			{
				for (const auto &p : soundDirs)
				{
					std::vector<std::string> sfxDirs;
					if (collectSubPath(p, "/win/se", "/se*", sfxDirs))
					{
						for (const auto &sp : sfxDirs)
						{
							std::vector<std::string> sfxFiles;
							if (collectDataFiles(sp, "*.spw", sfxFiles))
							{
								for (const auto &sfx : sfxFiles)
								{
									int32_t sfxIndex = pathToIndexAudio(&strstr(sfx.c_str(), "/win/se/")[-1]);
									if (m_resolvedPaths.find(sfxIndex) == m_resolvedPaths.end())
									{
										_dbgLog("emplace %8d : '%s'\n", sfxIndex, sfx.c_str());
										m_resolvedPaths.emplace(sfxIndex, sfx);
									}
								}
								res = true;
							}
						}
					}

					std::vector<std::string> bgwFiles;
					if (collectDataFiles(p, "/win/music/data", "*.bgw", bgwFiles))
					{
						for (const auto &bgw : bgwFiles)
						{
							int32_t bgwIndex = pathToIndexAudio(&strstr(bgw.c_str(), "/win/music/")[-1]);
							if (m_resolvedPaths.find(bgwIndex) == m_resolvedPaths.end())
							{
								_dbgLog("emplace %8d : '%s'\n", bgwIndex, bgw.c_str());
								m_resolvedPaths.emplace(bgwIndex, bgw);
							}
							res = true;
						}
					}
				}
			}
			return res;
		}

		bool Redirector::collectSubPath(const std::string &basePath, const std::string &pattern, std::vector<std::string> &results, bool doubleDirSep)
		{
			return collectSubPath(basePath, "", pattern, results, doubleDirSep);
		}

		bool Redirector::collectSubPath(const std::string &basePath, const std::string &midPath, const std::string &pattern, std::vector<std::string> &results, bool doubleDirSep)
		{
			HANDLE handle;
			WIN32_FIND_DATAA attrs;

			std::string searchPath = basePath + midPath + pattern;
		
			_dbgLog("'%s', '%s', '%s', '%d' (%s)\n", basePath.c_str(), midPath.c_str(), pattern.c_str(), doubleDirSep, searchPath.c_str());

			results.clear();
			handle = Redirector::s_procFindFirstFileA((LPCSTR)searchPath.c_str(), &attrs);
			if (handle != INVALID_HANDLE_VALUE)
			{
				do
				{
					std::string filename = attrs.cFileName;

					if ((attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 && filename != "." && filename != "..")
					{
						if (doubleDirSep)
						{
							/* this is only used to keep the same //ROM notation XI uses */
							_dbgLog("=> '%s'\n", (basePath + midPath + "//" + attrs.cFileName).c_str());
							results.emplace_back(basePath + midPath + "//" + attrs.cFileName);
						}
						else
						{
							_dbgLog("=> '%s'\n", (basePath + midPath + "/" + attrs.cFileName).c_str());
							results.emplace_back(basePath + midPath + "/" + attrs.cFileName);
						}
					}
				} while (FindNextFileA(handle, &attrs));
			}
			_dbgLog("results.size() = %d\n", results.size());
			return results.size() != 0;
		}

		bool Redirector::collectDataFiles(const std::string &parentPath, const std::string &pattern, std::vector<std::string> &results)
		{
			return collectDataFiles(parentPath, "", pattern, results);
		}

		bool Redirector::collectDataFiles(const std::string &parentPath, const std::string &midPath, const std::string &pattern, std::vector<std::string> &results)
		{
			HANDLE handle;
			WIN32_FIND_DATAA attrs;

			std::string searchPath = parentPath + midPath + "/" + pattern;

			_dbgLog("'%s', '%s', '%s' => (%s)\n", parentPath.c_str(), midPath.c_str(), pattern.c_str(), searchPath.c_str());

			results.clear();
			handle = Redirector::s_procFindFirstFileA((LPCSTR)searchPath.c_str(), &attrs);
			if (handle != INVALID_HANDLE_VALUE)
			{
				do
				{
					if ((attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
					{
						std::string finalPath = parentPath + midPath + "/" + attrs.cFileName;
						std::transform(finalPath.begin(), finalPath.end(), finalPath.begin(), [](unsigned char c) { return std::toupper(c); });

						_dbgLog("=> '%s'\n", finalPath.c_str());
						results.emplace_back(finalPath);
					}
				} while (FindNextFileA(handle, &attrs));
			}
			_dbgLog("results.size() = %d\n", results.size());
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
			int32_t romIndex = 0;

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
				for (; p && isdigit(*p); ++p)
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

		int32_t Redirector::pathToIndexAudio(const char *soundPath)
		{
			int32_t soundIndex = 0;

			/* start at the first character after "\\sound"
			 * this is either a digit or '/' in case of the base directory "\\sound\\"
			 * There is no specific check for paths shorter than that
			 * characters, but then again, this method is not called on
			 * random strings either.
			 */
			char *p = (char*)soundPath;

			if (strstr(soundPath, "/win/music/data") == 0 && strstr(soundPath, "\\win\\music\\data") == 0)
			{
				/* sound subdir */
				soundIndex = 10000000;

				/* cut the sound directory number */
				if (isdigit(soundPath[0]))
				{
					soundIndex += (soundPath[0] - '0') * 1000000;
				}
				/* cut out the 6 digits of the filename, they contain the subdir anyway 
				* 9/win/se/seAAA/seAAABBB.spw
				*/
				soundIndex += (soundPath[17] - '0') * 100000;
				soundIndex += (soundPath[18] - '0') * 10000;
				soundIndex += (soundPath[19] - '0') * 1000;
				soundIndex += (soundPath[20] - '0') * 100;
				soundIndex += (soundPath[21] - '0') * 10;
				soundIndex += (soundPath[22] - '0') * 1;
			}
			else
			{
				/* music subdir */
				soundIndex = 20000000;

				/* cut the sound directory number 
				 * 9\win\music\data\music058.bgw
				 */
				if (isdigit(soundPath[0]))
				{
					soundIndex += (soundPath[0] - '0') * 1000000;
				}
				/* cut out the 3 digits of the filename
				 * 9\win\music\data\music058.bgw
				 */
				soundIndex += (soundPath[22] - '0') * 100;
				soundIndex += (soundPath[23] - '0') * 10;
				soundIndex += (soundPath[24] - '0') * 1;
			}
			return soundIndex;
		}

#if (_DEBUG || _TEST)
		void Redirector::dbgLog(const char *fmt, ...)
		{
			if (m_dbgLog != nullptr && m_doDbgLog == true)
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
