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

#include "Redirector.h"
#include "MemCache.h"
#include "detours.h"

#include <cctype>
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace
{
	char* strstr_r(char* heystack, const char* needle)
	{
		char* res = nullptr;

		if (needle == nullptr ||heystack == nullptr)
		{
			return heystack;
		}

		while (true)
		{
			char* tmp = strstr(heystack, needle);
			if (tmp == nullptr)
			{
				break;
			}
			res = tmp++;
			heystack = tmp;
		}
		return res;
	}

	bool denormalised_ansi_path(const char* path, char* ansiPath, size_t ansiPathMax)
	{
		char pathBuf[MAX_PATH + 2];
		memset(pathBuf, 0, sizeof(pathBuf));
		strcpy_s(pathBuf, sizeof(pathBuf) - 2, path);

		char* romSuffix = strstr_r(pathBuf, "\\ROM");
		if (romSuffix != nullptr)
		{
			size_t suffixLen = 0;
			for (; romSuffix[suffixLen]; ++suffixLen)
			{
				romSuffix[suffixLen] = romSuffix[suffixLen] == '\\' ? '/' : romSuffix[suffixLen];
			}
			memmove(&romSuffix[1], &romSuffix[0], suffixLen);
			romSuffix[0] = '/';
		}
		memcpy(ansiPath, pathBuf, (ansiPathMax < sizeof(pathBuf) ? ansiPathMax : sizeof(pathBuf)));
		return true;
	}
}

namespace XiPivot
{
	namespace Core
	{
		/* static member initialisation */

		Redirector* Redirector::s_instance = nullptr;

		Redirector::pFnCreateFileA    Redirector::s_procCreateFileA = CreateFileA;
		Redirector::pFnFindFirstFileA Redirector::s_procFindFirstFileA = FindFirstFileA;
		Redirector::pFnFOpenS         Redirector::s_procFOpenS = fopen_s;

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

		errno_t __cdecl Redirector::dFOpenS(FILE** a0, const char* a1, const char* a2)
		{
			/* don't use the Singleton access here;
			 * if for whatever reason the global object is gone we don't want a new one
			 */
			if (Redirector::s_instance != nullptr)
			{
				return Redirector::s_instance->interceptFOpenS(a0, a1, a2);
			}
			return Redirector::s_procFOpenS(a0, a1, a2);
		}

		Redirector::Redirector()
			: m_hooksSet(false)
			, m_hookFOpenSet(false)
			, m_hookFOpenEnabled(false)
		{
			char workDir[MAX_PATH];

			m_resolvedPaths.clear();
			m_overlayPaths.clear();

			GetCurrentDirectoryA(sizeof(workDir), workDir);

			m_rootPath = workDir;
			m_delegate = DummyDelegate::instance();
		}

		Redirector::~Redirector()
		{
			releaseHooks(); // just in case
		}

		void Redirector::setLogProvider(IDelegate* newLogProvider)
		{
			if (newLogProvider == nullptr)
			{
				return;
			}
			m_delegate = newLogProvider;
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
				if (m_hookFOpenEnabled)
				{
					m_delegate->logMessage(IDelegate::LogLevel::Info, "enabling fopen_s hook");
					DetourAttach(&(PVOID&)Redirector::s_procFOpenS, Redirector::dFOpenS);
				}

				m_hooksSet = DetourTransactionCommit() == NO_ERROR;

				m_delegate->logMessageF(IDelegate::LogLevel::Info, "m_hooksSet = %s", m_hooksSet ? "true" : "false");
				return m_hooksSet;
			}
			m_delegate->logMessage(m_logDebug, "hooks already set");
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
				if (m_hookFOpenEnabled)
				{
					DetourDetach(&(PVOID&)Redirector::s_procFOpenS, Redirector::dFOpenS);
				}

				m_hooksSet = (DetourTransactionCommit() == NO_ERROR) ? false : true;

				m_delegate->logMessageF(IDelegate::LogLevel::Info, "m_hooksSet = %s", m_hooksSet ? "true" : "false");
				return m_hooksSet;
			}
			m_delegate->logMessage(m_logDebug, "hooks already removed");
			return false;
		}

		void Redirector::setDebugLog(bool state)
		{
			m_logDebug = (state) ? IDelegate::LogLevel::Debug : IDelegate::LogLevel::Discard;
			m_delegate->logMessageF(IDelegate::LogLevel::Info, "m_logDebug = %s", state ? "Debug" : "Discard");
		}

		bool Redirector::setRedirectFOpenS(bool state)
		{
			m_hookFOpenSet = state;
			if (!m_hooksSet)
			{
				m_hookFOpenEnabled = state;
			}
			return m_hookFOpenEnabled;
		}

		void Redirector::setRootPath(const std::string &newRoot)
		{
			m_rootPath = newRoot;
			m_resolvedPaths.clear();

			m_delegate->logMessageF(IDelegate::LogLevel::Info, "m_rootPath = '%s'", m_rootPath.c_str());
			for (const auto& overlay : m_overlayPaths)
			{
				std::string localPath = m_rootPath + "/" + overlay;
				scanOverlayPath(overlay);
			}
		}

		bool Redirector::addOverlay(const std::string &overlayPath)
		{
			m_delegate->logMessageF(IDelegate::LogLevel::Info, "addOverlay: '%s'", overlayPath.c_str());
			if (std::find(m_overlayPaths.begin(), m_overlayPaths.end(), overlayPath) == m_overlayPaths.end())
			{
				std::string localPath = m_rootPath + "/" + overlayPath;
				if (scanOverlayPath(localPath))
				{
					m_overlayPaths.emplace_back(overlayPath);
					m_delegate->logMessage(IDelegate::LogLevel::Info, "=> success");
					return true;
				}
			}
			m_delegate->logMessage(IDelegate::LogLevel::Error, "=> failed");
			return false;
		}

		void Redirector::removeOverlay(const std::string &overlayPath)
		{
			auto it = std::find(m_overlayPaths.begin(), m_overlayPaths.end(), overlayPath);

			m_delegate->logMessageF(IDelegate::LogLevel::Info, "removeOverlay: '%s'", overlayPath.c_str());
			if (it != m_overlayPaths.end())
			{
				m_resolvedPaths.clear();
				m_overlayPaths.erase(it);
				for (auto& path : m_overlayPaths)
				{
					std::string localPath = m_rootPath + "/" + path;
					scanOverlayPath(localPath);
				}
				m_delegate->logMessage(IDelegate::LogLevel::Info, "=> found, and removed");
			}
		}

		void Redirector::queryAll(std::vector<std::string> &queryReport) const
		{
			const auto rootPath = std::filesystem::path(m_rootPath).make_preferred();

			queryReport.clear();
			queryReport.emplace_back("#pivot-dump;");
			queryReport.emplace_back("#overlay-list;");
			queryReport.insert(queryReport.end(), m_overlayPaths.begin(), m_overlayPaths.end());
			queryReport.emplace_back("#redirects;");

			std::list<int32_t> redirectKeys;
			for (const auto& redirect : m_resolvedPaths)
			{
				redirectKeys.push_back(redirect.first);
			}
			/* this way the redirects will be sorted numerically
			 * instead of alphabetic.
			 */
			redirectKeys.sort();

			std::list<std::string> redirects;
			for (const auto key : redirectKeys)
			{
				auto redirectPath = std::filesystem::relative(std::filesystem::path(m_resolvedPaths.at(key)), rootPath).make_preferred();
				auto redirectOverlay = redirectPath.begin()->string();

				auto redirectName = redirectPath.string();
				auto index = redirectName.find("\\");
				if (index != std::string::npos)
				{
					redirectName = redirectName.substr(index + 1);
					/* otherwise we dump the full path - better than skipping a redirect. */
				}

				auto listEntry = redirectName + std::string(";") + redirectOverlay;
				queryReport.emplace_back(listEntry);
			}
		}

		bool Redirector::queryPath(const std::string& lookupPathStr, std::string& overlayName) const
		{
			auto rootPath = std::filesystem::path(m_rootPath).make_preferred();
			auto lookupPath = std::filesystem::path(lookupPathStr).make_preferred();
			auto lookupPathLowerStr = lookupPath.string();
			std::transform(lookupPathLowerStr.begin(), lookupPathLowerStr.end(), lookupPathLowerStr.begin(),
						   [](auto c) { return std::tolower(c); });

			auto suffix = std::filesystem::path(lookupPathLowerStr).extension();
			if (suffix != ".dat" && suffix != ".spw" && suffix != ".bgw")
			{
				overlayName = std::string("invalid argument");
				return true;
			}

			for (const auto& redirect : m_resolvedPaths)
			{
				auto redirectPath = std::filesystem::path(redirect.second);
				auto redirectPathLowerStr = redirectPath.make_preferred().string();
				std::transform(redirectPathLowerStr.begin(), redirectPathLowerStr.end(), redirectPathLowerStr.begin(),
					           [](auto c) { return std::tolower(c); });

				if (redirectPathLowerStr.find(lookupPathLowerStr) != std::string::npos)
				{
					// this is assuming the caller has provided a path that ends in a filename
					// otherwise we'll return the first path that contains whatever snippet
					// they provided. 
					auto lookupPathStr = std::filesystem::relative(redirectPath, rootPath).string();
					auto index = lookupPathStr.find("\\");
					if (index != std::string::npos)
					{
						overlayName = lookupPathStr.substr(0, index);
						return true;
					}
					overlayName = lookupPathStr;
					return true;
				}
			}
			return false;
		}

		/* static hooks */

		HANDLE __stdcall
			Redirector::interceptCreateFileA(LPCSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5, HANDLE a6)
		{
			if (shouldInterceptPath(a0))
			{
				//m_delegate->logMessageF(m_logDebug, "lpFileName = '%s'", static_cast<const char*>(a0));

				int32_t pathKey = -1;
				bool _unusedPathRedirected = false;
				const char* path = findRedirect(a0, pathKey, _unusedPathRedirected);
				return MemCache::instance().trackCacheObject(Redirector::s_procCreateFileA((LPCSTR)path, a1, a2, a3, a4, a5, a6), pathKey);
			}
			return Redirector::s_procCreateFileA(a0, a1, a2, a3, a4, a5, a6);
		}

		HANDLE __stdcall
			Redirector::interceptFindFirstFileA(LPCSTR a0, LPWIN32_FIND_DATAA a1)
		{
			if (shouldInterceptPath(a0))
			{
				m_delegate->logMessageF(m_logDebug, "lpFileName = '%s'", static_cast<const char*>(a0));

				int32_t _unusedPathKey = -1;
				bool _unusedPathRedirected = false;
				const char* path = findRedirect(a0, _unusedPathKey, _unusedPathRedirected);
				return Redirector::s_procFindFirstFileA((LPCSTR)path, a1);
			}
			return Redirector::s_procFindFirstFileA(a0, a1);
		}

		errno_t
			Redirector::interceptFOpenS(FILE** a0, const char* a1, const char* a2)
		{
			if (shouldInterceptFOpenS(a1))
			{
				m_delegate->logMessageF(m_logDebug, "lpFileName = [fopen_s] '%s'", a1);
				const char* path = findDenormalisedRedirect(a1);
				if (path != nullptr)
				{
					auto normalised = std::filesystem::weakly_canonical(std::filesystem::path(path)).make_preferred().string();
					return Redirector::s_procFOpenS(a0, normalised.c_str(), a2);
				}
			}

			return Redirector::s_procFOpenS(a0, a1, a2);
		}

		/* private stuff */
		const char *Redirector::findRedirect(const char *realPath, int32_t &outPathKey, bool &pathRedirected) const
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

			m_delegate->logMessageF(m_logDebug, "romPath = %d, sfxPath = %d", romPath != nullptr, sfxPath != nullptr);

			if (romPath != nullptr)
			{
				int32_t romIndex = pathToIndex(romPath);
				const auto res = m_resolvedPaths.find(romIndex);
			
				outPathKey = romIndex;
				if(res != m_resolvedPaths.end())
				{
					pathRedirected = true;
					m_delegate->logMessageF(m_logDebug, "using overlay '%s'", (*res).second.c_str());
					return (*res).second.c_str();
				}
			}
			if (sfxPath != nullptr)
			{
				int32_t sfxIndex = pathToIndexAudio(sfxPath);
				const auto res = m_resolvedPaths.find(sfxIndex);
			
				outPathKey = sfxIndex;
				if(res != m_resolvedPaths.end())
				{
					pathRedirected = true;
					m_delegate->logMessageF(m_logDebug, "using overlay '%s'", (*res).second.c_str());
					return (*res).second.c_str();
				}
			}
			pathRedirected = false;
			return realPath;
		}

		const char *Redirector::findDenormalisedRedirect(const char* realPath) const
		{
			char ansiPath[MAX_PATH + 2];
			if (denormalised_ansi_path(realPath, ansiPath, MAX_PATH))
			{
				int32_t _unusedPathKey = -1;
				bool redirectFound = false;
				const char* redirect = findRedirect(ansiPath, _unusedPathKey, redirectFound);
				if (redirectFound)
				{
					return redirect;
				}

			}
			return nullptr;
		}

		bool Redirector::scanOverlayPath(const std::string &basePath)
		{
			/* crawl an overlay path and collect all the DATs 
			 * in m_resolvedPaths.
			 */
			bool res = false;

			m_delegate->logMessageF(m_logDebug, "scanOverlayPath '%s'", basePath.c_str());

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
								m_delegate->logMessageF(IDelegate::LogLevel::Warn, "WARNING: ignoring invalid DAT (not VTABLE/FTABLE) '%s'", table.c_str());
								continue;
							}

							int32_t romIndex = pathToIndex(strstr(table.c_str(), "//ROM"));
							if (romIndex != -1)
							{
								if (m_resolvedPaths.find(romIndex) == m_resolvedPaths.end())
								{
									m_delegate->logMessageF(m_logDebug, "emplace %8d : '%s'", romIndex, table.c_str());
									m_resolvedPaths.emplace(romIndex, table);
								}
								else
								{
									m_delegate->logMessageF(IDelegate::LogLevel::Warn, "WARNING: %8d: ignoring '%s'", romIndex, table.c_str());
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
										m_delegate->logMessageF(IDelegate::LogLevel::Info, "Ignoring '%s' - invalid filename", dat.c_str());
										continue;
									}

									if (m_resolvedPaths.find(romIndex) == m_resolvedPaths.end())
									{
										m_delegate->logMessageF(m_logDebug, "emplace %8d : '%s'", romIndex, dat.c_str());
										m_resolvedPaths.emplace(romIndex, dat);
									}
									else
									{
										m_delegate->logMessageF(IDelegate::LogLevel::Warn, "WARNING: %8d: ignoring '%s'", romIndex, dat.c_str());
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
										m_delegate->logMessageF(IDelegate::LogLevel::Info, "Ignoring '%s' - invalid filename", sfx.c_str());
										continue;
									}
									if (m_resolvedPaths.find(sfxIndex) == m_resolvedPaths.end())
									{
										m_delegate->logMessageF(m_logDebug, "emplace %8d : '%s'", sfxIndex, sfx.c_str());
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
								m_delegate->logMessageF(IDelegate::LogLevel::Info, "Ignoring '%s' - invalid filename", bgw.c_str());
								continue;
							}
							if (m_resolvedPaths.find(bgwIndex) == m_resolvedPaths.end())
							{
								m_delegate->logMessageF(m_logDebug, "emplace %8d : '%s'", bgwIndex, bgw.c_str());
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
		
			m_delegate->logMessageF(m_logDebug, "'%s', '%s', '%s', '%d' (%s)", basePath.c_str(), midPath.c_str(), pattern.c_str(), doubleDirSep, searchPath.c_str());

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
							m_delegate->logMessageF(m_logDebug, "=> '%s'", (basePath + midPath + "//" + attrs.cFileName).c_str());
							results.emplace_back(basePath + midPath + "//" + attrs.cFileName);
						}
						else
						{
							m_delegate->logMessageF(m_logDebug, "=> '%s'", (basePath + midPath + "/" + attrs.cFileName).c_str());
							results.emplace_back(basePath + midPath + "/" + attrs.cFileName);
						}
					}
				} while (FindNextFileA(handle, &attrs));
			}
			m_delegate->logMessageF(m_logDebug, "results.size() = %d", results.size());
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

			m_delegate->logMessageF(m_logDebug, "'%s', '%s', '%s' => (%s)", parentPath.c_str(), midPath.c_str(), pattern.c_str(), searchPath.c_str());

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

						m_delegate->logMessageF(m_logDebug, "=> '%s'", finalPath.c_str());
						results.emplace_back(finalPath);
					}
				} while (FindNextFileA(handle, &attrs));
			}
			m_delegate->logMessageF(m_logDebug, "results.size() = %d", results.size());
			return results.size() != 0;
		}

		int32_t Redirector::pathToIndex(const char *romPath) const
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

		int32_t Redirector::pathToIndexAudio(const char *soundPath) const
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


		bool Redirector::shouldInterceptFOpenS(const char* path)
		{
			if (m_hookFOpenEnabled)
			{
				return shouldInterceptPath(path) && m_delegate->runFOpenSHook(path);
			}
			return false;
		}

		bool Redirector::shouldInterceptPath(const char* path)
		{
			return path != nullptr &&
				(strncmp(path, "\\\\?\\", 4) != 0 || strncmp(path, "\\\\.\\", 4) != 0) &&
				(
					strstr(path, "/ROM") != nullptr || strstr(path, "\\ROM") != nullptr 
					|| strstr(path, "/win/se/") != nullptr || strstr(path, "\\win\\se\\") != nullptr 
					|| strstr(path, "/win/music/") != nullptr || strstr(path, "\\win\\music\\") != nullptr
				);
		}
	}
}
