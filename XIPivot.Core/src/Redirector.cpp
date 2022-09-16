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

#include "Redirector.h"
#include "MemCache.h"
#include "detours.h"

#include <cctype>
#include <algorithm>

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
			m_logger = DummyLogProvider::instance();
		}

		Redirector::~Redirector()
		{
			releaseHooks(); // just in case
		}

		void Redirector::setLogProvider(ILogProvider* newLogProvider)
		{
			if (newLogProvider == nullptr)
			{
				return;
			}
			m_logger = newLogProvider;
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

				m_logger->logMessageF(ILogProvider::LogLevel::Info, "m_hooksSet = %s", m_hooksSet ? "true" : "false");
				return m_hooksSet;
			}
			m_logger->logMessage(m_logDebug, "hooks already set");
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

				m_logger->logMessageF(ILogProvider::LogLevel::Info, "m_hooksSet = %s", m_hooksSet ? "true" : "false");
				return m_hooksSet;
			}
			m_logger->logMessage(m_logDebug, "hooks already removed");
			return false;
		}

		void Redirector::setDebugLog(bool state)
		{
			m_logDebug = (state) ? ILogProvider::LogLevel::Debug : ILogProvider::LogLevel::Discard;
			m_logger->logMessageF(ILogProvider::LogLevel::Info, "m_logDebug = %s", state ? "Debug" : "Discard");
		}

		void Redirector::setRootPath(const std::string &newRoot)
		{
			m_rootPath = newRoot;
			m_resolvedPaths.clear();

			m_logger->logMessageF(ILogProvider::LogLevel::Info, "m_rootPath = '%s'", m_rootPath.c_str());
			for (const auto& overlay : m_overlayPaths)
			{
				std::string localPath = m_rootPath + "/" + overlay;
				scanOverlayPath(overlay);
			}
		}

		bool Redirector::addOverlay(const std::string &overlayPath)
		{
			m_logger->logMessageF(ILogProvider::LogLevel::Info, "addOverlay: '%s'", overlayPath.c_str());
			if (std::find(m_overlayPaths.begin(), m_overlayPaths.end(), overlayPath) == m_overlayPaths.end())
			{
				std::string localPath = m_rootPath + "/" + overlayPath;
				if (scanOverlayPath(localPath))
				{
					m_overlayPaths.emplace_back(overlayPath);
					m_logger->logMessage(ILogProvider::LogLevel::Info, "=> success");
					return true;
				}
			}
			m_logger->logMessage(ILogProvider::LogLevel::Error, "=> failed");
			return false;
		}

		void Redirector::removeOverlay(const std::string &overlayPath)
		{
			auto it = std::find(m_overlayPaths.begin(), m_overlayPaths.end(), overlayPath);

			m_logger->logMessageF(ILogProvider::LogLevel::Info, "removeOverlay: '%s'", overlayPath.c_str());
			if (it != m_overlayPaths.end())
			{
				m_resolvedPaths.clear();
				m_overlayPaths.erase(it);
				for (auto& path : m_overlayPaths)
				{
					std::string localPath = m_rootPath + "/" + path;
					scanOverlayPath(localPath);
				}
				m_logger->logMessage(ILogProvider::LogLevel::Info, "=> found, and removed");
			}
		}

		/* static hooks */

		HANDLE __stdcall
			Redirector::interceptCreateFileA(LPCSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5, HANDLE a6)
		{
			m_logger->logMessageF(m_logDebug, "lpFileName = '%s'", static_cast<const char*>(a0));

			int32_t pathKey = -1;
			const char* path = findRedirect(a0, pathKey);
			return MemCache::instance().trackCacheObject(Redirector::s_procCreateFileA((LPCSTR)path, a1, a2, a3, a4, a5, a6), pathKey);
		}

		HANDLE __stdcall
			Redirector::interceptFindFirstFileA(LPCSTR a0, LPWIN32_FIND_DATAA a1)
		{
			m_logger->logMessageF(m_logDebug, "lpFileName = '%s'", static_cast<const char*>(a0));

			int32_t _unusedPathKey = -1;
			const char* path = findRedirect(a0, _unusedPathKey);
			return Redirector::s_procFindFirstFileA((LPCSTR)path, a1);
		}

		/* private stuff */
		const char *Redirector::findRedirect(const char *realPath, int32_t &outPathKey)
		{
			/*
			 * findRedirect relies on a very specific implementation detail in the game client.
			 * Namely the fact that - probably based on the old PS2 code? - XI will use unix-style,
			 * denormalised paths when attempting to read DAT/VTBALE and FTABLE files for engine use.
			 * These paths are only used during gameplay, updates and the early launch as well as POL.exe
			 * all use reguler, normalised paths.
			 * 
			 * With this in mind the scope of which files are attempted to be hooked can be kept as
			 * narrow as possible by checking for the denormalised '/ROM*' path suffix.
			 * 
			 * This also - on purpose - prevents accidental hooking of addons or plugins that try to
			 * read the original game files.
			 * 
			 * If an addon / plugin of any of the various interfaces needs a redirected path it SHOULD
			 * ask for it the same way the game would do natively.
			 * 
			 * Yes, I'm aware that in case the developers at SE "fix" this pivot would have to be
			 * patched in order to work and this would also imply adding checks for when a file is requested
			 * and *from where in XIs code* in order to prevent unwanted redirection.
			 * However.. this has been around for 20 years now and I have my doubts it will be changed
			 * unless windows stops supporting denormalised paths.
			 */
			const char *romPath = strstr(realPath, "//ROM");
			const char *sfxPath;

			// FIXME: denormalised paths don't apply to music redirects with has the potential
			// FIXME: to break music overlays in combination with the Ashita_v4 interface if there's an update to those. 
			if (strstr(realPath, "\\win\\se\\") != nullptr || strstr(realPath, "\\win\\music\\") != nullptr)
			{
				sfxPath = &(strstr(realPath, "\\win\\")[-1]);
			}
			else
			{
				sfxPath = nullptr;
			}

			m_logger->logMessageF(m_logDebug, "romPath = %d, sfxPath = %d\n", romPath != nullptr, sfxPath != nullptr);

			if (romPath != nullptr)
			{
				int32_t romIndex = pathToIndex(romPath);
				const auto res = m_resolvedPaths.find(romIndex);
			
				outPathKey = romIndex;
				if(res != m_resolvedPaths.end())
				{
					m_logger->logMessageF(m_logDebug, "using overlay '%s'\n", (*res).second.c_str());
					return (*res).second.c_str();
				}
			}
			if (sfxPath != nullptr)
			{
				int32_t sfxIndex = pathToIndexAudio(sfxPath);
				const auto res = m_resolvedPaths.find(sfxIndex);
			
				// disabled for music until I have time to look into why it creates audio garbage.
				outPathKey = sfxIndex;
				if(res != m_resolvedPaths.end())
				{
					m_logger->logMessageF(m_logDebug, "using overlay '%s'\n", (*res).second.c_str());
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

			m_logger->logMessageF(m_logDebug, "scanOverlayPath '%s'", basePath.c_str());

			std::vector<std::string> romDirs;
			if (collectSubPath(basePath, "//ROM*", romDirs, true))
			{
				for (const auto &p : romDirs)
				{
					std::vector<std::string> datTables;
					if (collectDataFiles(p, "*.DAT", datTables))
					{
						for (const auto& table : datTables)
						{
							if (strstr(table.c_str(), "VTABLE") == nullptr && strstr(table.c_str(), "FTABLE") == nullptr)
							{
								m_logger->logMessageF(ILogProvider::LogLevel::Warn, "WARNING: ignoring invalid DAT (not VTABLE/FTABLE) '%s'", table.c_str());
								continue;
							}

							int32_t romIndex = pathToIndex(strstr(table.c_str(), "//ROM"));
							if (romIndex != -1)
							{
								if (m_resolvedPaths.find(romIndex) == m_resolvedPaths.end())
								{
									m_logger->logMessageF(m_logDebug, "emplace %8d : '%s'", romIndex, table.c_str());
									m_resolvedPaths.emplace(romIndex, table);
								}
								else
								{
									m_logger->logMessageF(ILogProvider::LogLevel::Warn, "WARNING: %8d: ignoring '%s'", romIndex, table.c_str());
								}
								/* don't touch res here */
							}
						}
					}

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
									if (romIndex == -1)
									{
										m_logger->logMessageF(ILogProvider::LogLevel::Info, "Ignoring '%s' - invalid filename", dat.c_str());
										continue;
									}

									if (m_resolvedPaths.find(romIndex) == m_resolvedPaths.end())
									{
										m_logger->logMessageF(m_logDebug, "emplace %8d : '%s'", romIndex, dat.c_str());
										m_resolvedPaths.emplace(romIndex, dat);
									}
									else
									{
										m_logger->logMessageF(ILogProvider::LogLevel::Warn, "WARNING: %8d: ignoring '%s'", romIndex, dat.c_str());
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
								for (auto &sfx : sfxFiles)
								{
									std::transform(sfx.begin(), sfx.end(), sfx.begin(), [](unsigned char c) { return std::tolower(c); });
									int32_t sfxIndex = pathToIndexAudio(&strstr(sfx.c_str(), "/win/se/")[-1]);
									if (sfxIndex == -1)
									{
										m_logger->logMessageF(ILogProvider::LogLevel::Info, "Ignoring '%s' - invalid filename", sfx.c_str());
										continue;
									}
									if (m_resolvedPaths.find(sfxIndex) == m_resolvedPaths.end())
									{
										m_logger->logMessageF(m_logDebug, "emplace %8d : '%s'", sfxIndex, sfx.c_str());
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
						for (auto &bgw : bgwFiles)
						{
							std::transform(bgw.begin(), bgw.end(), bgw.begin(), [](unsigned char c) { return std::tolower(c); });
							int32_t bgwIndex = pathToIndexAudio(&strstr(bgw.c_str(), "/win/music/")[-1]);
							if (bgwIndex == -1)
							{
								m_logger->logMessageF(ILogProvider::LogLevel::Info, "Ignoring '%s' - invalid filename", bgw.c_str());
								continue;
							}
							if (m_resolvedPaths.find(bgwIndex) == m_resolvedPaths.end())
							{
								m_logger->logMessageF(m_logDebug, "emplace %8d : '%s'", bgwIndex, bgw.c_str());
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
		
			m_logger->logMessageF(m_logDebug, "'%s', '%s', '%s', '%d' (%s)", basePath.c_str(), midPath.c_str(), pattern.c_str(), doubleDirSep, searchPath.c_str());

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
							m_logger->logMessageF(m_logDebug, "=> '%s'", (basePath + midPath + "//" + attrs.cFileName).c_str());
							results.emplace_back(basePath + midPath + "//" + attrs.cFileName);
						}
						else
						{
							m_logger->logMessageF(m_logDebug, "=> '%s'", (basePath + midPath + "/" + attrs.cFileName).c_str());
							results.emplace_back(basePath + midPath + "/" + attrs.cFileName);
						}
					}
				} while (FindNextFileA(handle, &attrs));
			}
			m_logger->logMessageF(m_logDebug, "results.size() = %d", results.size());
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

			m_logger->logMessageF(m_logDebug, "'%s', '%s', '%s' => (%s)", parentPath.c_str(), midPath.c_str(), pattern.c_str(), searchPath.c_str());

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

						m_logger->logMessageF(m_logDebug, "=> '%s'", finalPath.c_str());
						results.emplace_back(finalPath);
					}
				} while (FindNextFileA(handle, &attrs));
			}
			m_logger->logMessageF(m_logDebug, "results.size() = %d", results.size());
			return results.size() != 0;
		}

		int32_t Redirector::pathToIndex(const char *romPath)
		{
			/* **very** tailored approach to get a fast,
			 * unique index for every given //ROM* path
			 *
			 * it's build on the current ROM layout and
			 * **will break** if SE ever decides to add
			 * more than 999 sub folders or 13 ROM roots.
			 *
			 * every numeric part of the ROM path will 
			 * be extracted and added to the romIndex.
			 * directory separators multiply the number
			 * by 1000 and ensure the numbers don't collide.
			 * 
			 * VTABLE an FTABLE use the base 14000000 and 15000000.
			 *
			 * See the following paths and their resulting index:
			 *
			 * //ROM/0/0.DAT        =>         0
			 * //ROM/0/1.DAT        =>         1
			 * //ROM1/2/3.DAT       =>   1002003
			 * //ROM1/22/33.DAT     =>   1022033
			 * //ROM1/222/333.DAT   =>   1222333
			 * //ROM9/999/999.DAT   =>   9999999
			 * //ROM10/999/999.DAT  => 109999999
			 * //ROM10/VTABLE.DAT   => 140000010
			 * //ROM10/FTABLE.DAT   => 150000010
			 * - anything invalid - =>        -1
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
				else if (*p == 'V')
				{
					/* this is a VTABLE*.DAT */
					romIndex += 14000000;
					break;
				}
				else if (*p == 'F')
				{
					/* this is a FTABLE*.DAT */
					romIndex += 15000000;
					break;
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
				soundIndex = 20000000;

				/* cut the sound directory number */
				if (isdigit(soundPath[0]))
				{
					soundIndex += (soundPath[0] - '0') * 1000000;
				}

				if (!isdigit(soundPath[17]) || !isdigit(soundPath[18]) || !isdigit(soundPath[19]) ||
					!isdigit(soundPath[20]) || !isdigit(soundPath[21]) || !isdigit(soundPath[22]))
				{
					return -1;
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
				soundIndex = 30000000;

				/* cut the sound directory number 
				 * 9\win\music\data\music058.bgw
				 */
				if (isdigit(soundPath[0]))
				{
					soundIndex += (soundPath[0] - '0') * 1000000;
				}

				if (!isdigit(soundPath[22]) || !isdigit(soundPath[23]) || !isdigit(soundPath[24]))
				{
					return -1;
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
	}
}
