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

#include "AshitaInterface.h"
#include "MemCache.h"

#include <regex>

#define _XI_RESET    "\x1E\x01"
#define _XI_NORMAL   "\x1E\x01"
#define _XI_LGREEN   "\x1E\x02"
#define _XI_PINK     "\x1E\x05"
#define _XI_DEEPBLUE "\x1E\x05"
#define _XI_SEAFOAM  "\x1E\x53"
#define _XI_CREAM    "\x1F\x82"

static std::string join(const std::vector<std::string>::iterator &begin, const std::vector<std::string>::iterator &end, const std::string &sep)
{
	std::string res;
	for (auto it = begin; it != end; ++it)
	{
		if (res.empty() == false)
		{
			res += sep;
		}
		res += (*it);
	}
	return res;
}

static std::vector<std::string> split(const std::string &input, const std::string &exp)
{
	std::regex re(exp);
	std::sregex_token_iterator first{ input.begin(), input.end(), re, -1 }, last;
	return { first, last };
}

namespace XiPivot
{
	plugininfo_t *AshitaInterface::s_pluginInfo = nullptr;

	AshitaInterface::AshitaInterface(void)
	  : Core::Redirector(),
	
	    m_showConfigWindow(false),
	    m_pluginId(0),
	    m_ashitaCore(nullptr),
	    m_logManager(nullptr),
	    m_direct3DDevice(nullptr)
	{
		/* FIXME: does this play anywhere nice with reloads?
		 * FIXME: .. I hope it does
		 */
		Redirector::s_instance = this;

		m_uiConfig.debugState = false;
		m_uiConfig.purgeOverlay.clear();
	}

	plugininfo_t AshitaInterface::GetPluginInfo(void)
	{
		return *AshitaInterface::s_pluginInfo;
	}

	bool AshitaInterface::Initialize(IAshitaCore *core, ILogManager *log, uint32_t id)
	{
		bool initialized = true;

		m_ashitaCore = core;
		m_logManager = log;
		m_pluginId = id;
		m_config = (core ? core->GetConfigurationManager() : nullptr);

		instance().setLogProvider(this);

		if (m_config != nullptr)
		{
			if (m_settings.load(m_config))
			{
				m_uiConfig.debugState = m_settings.debugLog;

				instance().setDebugLog(m_settings.debugLog);
				instance().setRootPath(m_settings.rootPath);
				for (const auto &path : m_settings.overlays)
				{
					instance().addOverlay(path);
				}

				if (m_settings.cacheEnabled)
				{
					Core::MemCache::instance().setLogProvider(this);
					Core::MemCache::instance().setDebugLog(m_settings.debugLog);
					Core::MemCache::instance().setCacheAllocation(m_settings.cacheSize);

					if (initialized)
					{
						m_nextCachePurge = time(nullptr) + m_settings.cachePurgeDelay;
					}
				}
			}
			m_settings.save(m_config);
		}

		if (m_settings.cacheEnabled)
		{
			initialized &= Core::MemCache::instance().setupHooks();
		}
		initialized &= instance().setupHooks();
		return initialized;
	}

	void AshitaInterface::Release(void)
	{
		if (m_settings.cacheEnabled)
		{
			Core::MemCache::instance().releaseHooks();
		}
		instance().releaseHooks();
	}

	bool AshitaInterface::HandleCommand(const char *command, int32_t /*type*/)
	{
		std::vector<std::string> args;
		Ashita::Commands::GetCommandArgs(command, &args);

		HANDLECOMMAND("/pivot")
		{
			if (args.size() == 3)
			{

				if (args[1] == "a" || args[1] == "add")
				{
					if (instance().addOverlay(args[2]))
					{
						m_settings.overlays.emplace_back(args[2]);
						m_settings.save(m_config);
					}
					else
					{
						chatPrintf("$cs(7)failed to add '$cs(9)%s$cs(7)'.$cr", args[2].c_str());
					}

				}
				else if (args[1] == "r" || args[1] == "remove")
				{
					std::vector<std::string>::iterator it;

					instance().removeOverlay(args[2]);

					it = std::find(m_settings.overlays.begin(), m_settings.overlays.end(), args[2].c_str());
					if (it != m_settings.overlays.end())
					{
						m_settings.overlays.erase(it);
						m_settings.save(m_config);
					}
				}
			}
			else if (args.size() == 2 && (args[1] == "h" || args[1] == "help"))
			{
				chatPrintf("$cs(16)%s$cs(19) v.$cs(16)%.2f$cs(19) by $cs(14)%s$cr", s_pluginInfo->Name, s_pluginInfo->PluginVersion, s_pluginInfo->Author);
				chatPrintf("   $cs(9)a$cs(16)dd overlay_dir $cs(19)- Adds a path to be searched for DAT overlays$cr");
				chatPrintf("   $cs(9)r$cs(16)emove overlay_dir $cs(19)- Removes a path from the DAT overlays$cr");
				chatPrintf("   $cs(16)-$cr");
				chatPrintf("   $cs(19)Adding or removing overlays at runtime can cause $cs(16)all kinds of unexpected behaviour.$cr");
				chatPrintf("   $cs(19)It is recommended to edit XIPivot.xml instead - $cs(16)you have been warned.$cr");
			}
			else if (args.size() == 2 && (args[1] == "c" || args[1] == "cache") && m_settings.cacheEnabled == true)
			{
				m_showCacheWindow = !m_showCacheWindow;
			}
			else
			{
				m_uiConfig.debugState = m_settings.debugLog;
				m_uiConfig.purgeOverlay.clear();
				m_uiConfig.allOverlays = listAvailableOverlays();

				m_uiConfig.applyCacheChanges = false;
				m_uiConfig.cacheState = m_settings.cacheEnabled;
				m_uiConfig.cacheSizeMB = m_settings.cacheSize / 0x100000;
				m_uiConfig.cachePurgeDelay = m_settings.cachePurgeDelay;

				m_showConfigWindow = true;
			}
			return true;
		}

		return false;
	}

	bool AshitaInterface::Direct3DInitialize(IDirect3DDevice8* device)
	{
		m_direct3DDevice = device;
		return true;
	}

	void AshitaInterface::Direct3DPreRender()
	{
		if (m_settings.debugLog != m_uiConfig.debugState)
		{
			m_settings.debugLog = m_uiConfig.debugState;
			instance().setDebugLog(m_settings.debugLog);
		}
		if (m_uiConfig.purgeOverlay.empty() == false)
		{
			instance().removeOverlay(m_uiConfig.purgeOverlay);

			m_settings.overlays = instance().overlayList();
			m_settings.save(m_config);

			m_uiConfig.purgeOverlay.clear();
		}

		if (m_uiConfig.applyCacheChanges == true)
		{
			m_uiConfig.applyCacheChanges = false;

			m_settings.cacheEnabled    = m_uiConfig.cacheState;
			m_settings.cacheSize       = m_uiConfig.cacheSizeMB * 0x100000; // cacheSize is in bytes internally
			m_settings.cachePurgeDelay = m_uiConfig.cachePurgeDelay;
			m_settings.save(m_config);

			Core::MemCache::instance().setCacheAllocation(m_settings.cacheSize);
			if (m_settings.cacheEnabled == true && Core::MemCache::instance().hooksActive() == false)
			{
				m_settings.cacheEnabled = Core::MemCache::instance().setupHooks();
				m_uiConfig.cacheState = m_settings.cacheEnabled;
			}
			else if (m_settings.cacheEnabled == false && Core::MemCache::instance().hooksActive() == true)
			{
				Core::MemCache::instance().releaseHooks();
			}
		}

		if (m_settings.cacheEnabled == true)
		{
			const time_t now = time(nullptr);
			if (now > m_nextCachePurge)
			{
				m_nextCachePurge = now + m_settings.cachePurgeDelay;
				Core::MemCache::instance().purgeCacheObjects(m_settings.cachePurgeDelay);
			}
		}
	}

	void AshitaInterface::Direct3DRender()
	{
		const auto imgui = m_ashitaCore->GetGuiManager();

		if (m_showConfigWindow)
		{
			if (imgui->Begin(u8"XiPivot Setup", &m_showConfigWindow, ImVec2(600, 550), -1.0f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize) == true)
			{
				const char *tabTitles[] = { "overlays", "cache"/*, "about"*/, nullptr };
				for (int i = 0; tabTitles[i] != nullptr; ++i)
				{
					imgui->PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.69f, 1.0f, m_uiConfig.activeTab == i ? 0.8f : 0.1f));
					if (imgui->Button(tabTitles[i]))
					{
						m_uiConfig.activeTab = i;
					}
					imgui->PopStyleColor();
					imgui->SameLine();
				}
				imgui->NewLine();
				imgui->Separator();

				switch (m_uiConfig.activeTab)
				{
					case 0:
						renderOverlayConfigUI(imgui);
						break;

					case 1:
						renderMemCacheConfigUI(imgui);
						break;
				}
			}
			imgui->End();
		}

		if (m_showCacheWindow)
		{
			if (imgui->Begin(u8"XiPivot Cache", &m_showCacheWindow, ImVec2(0, 0), 0.25f, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar) == true)
			{
				renderCacheStatsUI(imgui);
			}
			imgui->End();
		}
	}

	/* ILogProvider */
	void AshitaInterface::logMessage(Core::ILogProvider::LogLevel level, std::string msg)
	{
		logMessageF(level, msg);
	}

	void AshitaInterface::logMessageF(Core::ILogProvider::LogLevel level, std::string msg, ...)
	{
		if (level != Core::ILogProvider::LogLevel::Discard)
		{
			char msgBuf[512];
			Ashita::LogLevel ashitaLevel = Ashita::LogLevel::None;

			switch (level)
			{
				case Core::ILogProvider::LogLevel::Discard: /* never acutally reached */
					return;

				case Core::ILogProvider::LogLevel::Debug:
					ashitaLevel = Ashita::LogLevel::Debug;
					break;

				case Core::ILogProvider::LogLevel::Info:
					ashitaLevel = Ashita::LogLevel::Information;
					break;

				case Core::ILogProvider::LogLevel::Warn:
					ashitaLevel = Ashita::LogLevel::Warning;
					break;

				case Core::ILogProvider::LogLevel::Error:
					ashitaLevel = Ashita::LogLevel::Error;
					break;
			}

			va_list args;
			va_start(args, msg);

			vsnprintf_s(msgBuf, 511, msg.c_str(), args);
			m_logManager->Log(static_cast<uint32_t>(ashitaLevel), "XiPivot", msgBuf);

			va_end(args);
		}
	}

	/* private parts below */

	AshitaInterface::Settings::Settings()
	{
		char workPath[MAX_PATH];

		GetCurrentDirectoryA(MAX_PATH, static_cast<LPSTR>(workPath));

		/* default to "plugin location"/DATs */
		rootPath = std::string(workPath) + "/DATs";
		overlays.clear();
		debugLog = false;
		cacheEnabled = false;
		cacheSize = 0;
		cachePurgeDelay = 600;
	}

	bool AshitaInterface::Settings::load(IConfigurationManager *config)
	{
		if (config->Load("XIPivot", "XIPivot"))
		{
			const char *rP = config->get_string("XIPivot", "root_path");
			const char *oL = config->get_string("XIPivot", "overlays");
			const bool dbg = config->get_bool("XIPivot", "debug_log", false);

			debugLog = dbg;
			rootPath = (rP ? rP : rootPath);

			overlays.clear();
			if(oL != nullptr && strlen(oL) > 0)
			{
				overlays = split(oL, ",");
			}

			cacheEnabled = config->get_bool("XIPivot", "cache_enabled", false);
			cacheSize = config->get_int32("XIPivot", "cache_size", 2048) * 0x100000; // 2gb
			cachePurgeDelay = config->get_int32("XIPivot", "cache_max_age", 600); // 10min

			return true;
		}
		return false;
	}

	void AshitaInterface::Settings::save(IConfigurationManager *config)
	{
		config->set_value("XIPivot", "overlays", join(overlays.begin(), overlays.end(), ",").c_str());
	
		config->set_value("XIPivot", "cache_enabled", cacheEnabled ? "true" : "false");

		char val[32];
		snprintf(val, 31, "%u", cacheSize / 0x100000);
		config->set_value("XIPivot", "cache_size", val);

		snprintf(val, 31, "%u", cachePurgeDelay);
		config->set_value("XIPivot", "cache_max_age", val);

		config->Save("XIPivot", "XIPivot");
	}

	void AshitaInterface::chatPrint(const char *msg)
	{
		/* chatPrint works just like the Write method of ChatManager, except that
		 * it adds some convenience escapes:
		 *  $cs(index)  - will change the text colour to `index` from the below table
		 *  $cr         - will reset the text colour
		 */
		static const char *colourTab[] = 
		{
			/* thanks to atom0s in the Ashita discord for figuring these out */
			"\x1e\x01", /* 0  - reset / white   */
			"\x1e\x02", /* 1  - neon green      */
			"\x1e\x03", /* 2  - deep periwinkle */
			"\x1e\x05", /* 3  - pink            */
			"\x1e\x06", /* 4  - deep cyan       */
			"\x1e\x07", /* 5  - cream           */
			"\x1e\x08", /* 6  - deep orange     */
			"\x1e\x44", /* 7  - red             */
			"\x1e\x45", /* 8  - yellow          */
			"\x1e\x47", /* 9  - slate blue      */
			"\x1e\x48", /* 10 - deep pink       */
			"\x1e\x49", /* 11 - light pink      */
			"\x1e\x4c", /* 12 - deep red        */
			"\x1e\x4f", /* 13 - green           */
			"\x1e\x51", /* 14 - purple          */
			"\x1e\x52", /* 15 - light cyan      */
			"\x1e\x53", /* 16 - seafoam         */
			"\x1e\x5d", /* 17 - light red       */
			"\x1e\x6d", /* 18 - light yellow    */
			"\x1f\x82", /* 19 - cream white     */
		};

		char *parsed = new char[strlen(msg) * 2], *p = parsed;

		memset(parsed, 0, strlen(msg) * 2);
		for (auto i = 0U; i < strlen(msg); )
		{
			switch (msg[i])
			{
				case '$':
				{
					auto j = i, ci = 0U;
					bool copyColour = false;

					/* check for a colour selection "$cs(123..)" */
					if (strncmp(&msg[i], "$cs(", 4) == 0)
					{
						j += 4;
						while (j < strlen(msg) && isdigit(msg[j]))
						{
							ci *= 10;
							ci += msg[j++] - '0';
						}

						if (msg[j] == ')')
						{
							/* if it's within the table mark it for copy */
							copyColour = (ci < sizeof(colourTab));
							i = j + 1;
						}
						else
						{
							*p++ = '$';
							++i;
						}
					}
					/* check for a colour reset $cr */
					else if (strncmp(&msg[i], "$cr", 3) == 0)
					{
						copyColour = true;
						ci = 0;
						i += 3;
					}
					else
					{
						if (strncmp(&msg[i], "$$", 2) == 0)
						{
							*p++ = '$';
							i += 2;
						}
						else
						{
							*p++ = '$';
							++i;
						}
					}

					if (copyColour)
					{
						auto len = strlen(colourTab[ci]);
						strncat_s(p, len + 1, colourTab[ci], len);
						p += strlen(colourTab[ci]);
					}
				}
				break;

			default:
				*p++ = msg[i++];
				break;
			}
		}

		if (m_ashitaCore != nullptr)
		{
			m_ashitaCore->GetChatManager()->Write(parsed);
		}
		delete[] parsed;
	}

	void AshitaInterface::chatPrintf(const char *fmt, ...)
	{
		char msg[512]; /* wildly random size.. */

		va_list args;
		va_start(args, fmt);
		vsnprintf_s(msg, sizeof(msg), sizeof(msg) - 1, fmt, args);
		va_end(args);

		chatPrint(msg);
	}


	/* GUI stuff */
	std::vector<std::string> AshitaInterface::listAvailableOverlays() const
	{
		std::vector<std::string> res;

		HANDLE hRef;
		WIN32_FIND_DATAA data;
		auto pattern = rootPath() + "/*";

		if ((hRef = FindFirstFileA(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE)
		{
			do {
				if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
				{
					if (strcmp(data.cFileName, "..") != 0 && strcmp(data.cFileName, ".") != 0)
					{
						res.emplace_back(data.cFileName);
					}
				}
			} while (FindNextFileA(hRef, &data) != FALSE);
			FindClose(hRef);
		}

		return res;
	}

	void AshitaInterface::renderOverlayConfigUI(IGuiManager* imgui)
	{

		imgui->Checkbox(u8"debug log", &m_uiConfig.debugState);
		imgui->LabelText(u8"root path", "%s", m_settings.rootPath.c_str());
		imgui->Text(u8"active overlays:");
		imgui->BeginChild(u8"overlay_list", ImVec2(0, 200));
		{
			if (m_uiConfig.purgeOverlay.empty())
			{
				int prio = 0;
				for (const auto& path : instance().overlayList())
				{
					char btnId[] = { ' ', '-', ' ', '#', '#', static_cast<char>(prio + 'a') };
					if (imgui->Button(btnId))
					{
						m_uiConfig.purgeOverlay = path;
					}
					imgui->SameLine();
					imgui->Text(u8"[%02d] %s", prio++, path.c_str());
				}
			}
		}
		imgui->EndChild();
		imgui->Separator();

		imgui->Text(u8"available overlays:");
		imgui->BeginChild(u8"available_overlays", ImVec2(0, 200));
		{
			int n = 0;
			const auto l = instance().overlayList();
			for (const auto &path : m_uiConfig.allOverlays)
			{
				if (std::find(l.begin(), l.end(), path) == l.end())
				{
					// n is the index of the overlay in the list of all overlays which should be constant
					char btnId[] = { ' ', '+', ' ', '#', '#', static_cast<char>(n + 'a') };
					if (imgui->Button(btnId))
					{
						if (instance().addOverlay(path))
						{
							m_settings.overlays.emplace_back(path);
							m_settings.save(m_config);
						}
					}
					imgui->SameLine();
					imgui->Text(u8"%s", path.c_str());
				}
				++n;
			}
		}
		imgui->EndChild();

		imgui->Separator();
		imgui->TextDisabled(u8"Adding or removing overlays at runtime can cause all kinds of unexpected behaviour.");
		imgui->TextDisabled(u8"It is recommended to edit XIPivot.xml instead");
	}

	void AshitaInterface::renderMemCacheConfigUI(IGuiManager* imgui)
	{
		imgui->Checkbox(u8"use cache", &m_uiConfig.cacheState);
		imgui->SliderInt(u8"reserved size", &m_uiConfig.cacheSizeMB, 1, 4096, "%.0f mb");
		imgui->SliderInt(u8"purge interval", &m_uiConfig.cachePurgeDelay, 1, 600, "%.0f sec");

		imgui->Separator();
		if (imgui->Button(u8"apply##cache settings", ImVec2(FLT_MAX, 0)))
		{
			m_uiConfig.applyCacheChanges = true;
		}
		imgui->NewLine();
		imgui->TextDisabled(u8"Reducing the cache size will not instantly take effect if the currently");
		imgui->TextDisabled(u8"used cache size is larger then the new maximum.");
		imgui->TextDisabled(u8"Cached files that have no open handle will be removed after the purge delay.");

		if (m_settings.cacheEnabled == true)
		{
			imgui->NewLine();
			imgui->Separator();
			renderCacheStatsUI(imgui);
		}
	}

	void AshitaInterface::renderCacheStatsUI(IGuiManager* imgui)
	{
		const auto stats = Core::MemCache::instance().getCacheStats();
		if (stats.cacheHits != 0 || stats.cacheMisses != 0)
		{
			imgui->LabelText(u8"cache hits", "%d%%", stats.cacheHits * 100 / (stats.cacheHits + stats.cacheMisses));
			imgui->Separator();
		}
		imgui->LabelText(u8"allocation", "%.2fmb", stats.allocation / 1048576.0f);
		imgui->LabelText(u8"used size", "%.2fmb", stats.used / 1048576.0f);
		imgui->LabelText(u8"objects", "%d", stats.activeObjects);
		imgui->LabelText(u8"ignored", "%d", stats.cacheIgnored);
		imgui->Separator();

		imgui->LabelText(u8"next purge in", "%ds", m_nextCachePurge - time(nullptr));
	}
}
