/*
 * 	Copyright © 2019-2020, Renee Koecher
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
#include <iostream>

namespace XiPivot
{
	namespace Pol
	{
		AshitaInterface::AshitaInterface(const char* args)
			: IPolPlugin(),
			m_pluginArgs(args ? args : "")
		{
		}

		const char* AshitaInterface::GetName(void) const        { return PluginName; }
		const char* AshitaInterface::GetAuthor(void) const      { return PluginAuthor; }
		const char* AshitaInterface::GetDescription(void) const { return PluginDescr; }
		const char* AshitaInterface::GetLink(void) const        { return PluginUrl; }
		double AshitaInterface::GetVersion(void) const          { return PluginVersion; }

		uint32_t AshitaInterface::GetFlags(void) const          { return (uint32_t)(Ashita::PluginFlags::UsePluginEvents | Ashita::PluginFlags::UseCommands | Ashita::PluginFlags::UseDirect3D); }

		bool AshitaInterface::Initialize(IAshitaCore* core, ILogManager* log, uint32_t id)
		{
			bool initialized = true;

			auto& memCache   = Core::MemCache::instance();
			auto& redirector = Core::Redirector::instance();

			IPolPlugin::Initialize(core, log, id);

			m_ashitaCore = core;
			m_logManager = log;

			redirector.setLogProvider(this);

			auto config = m_ashitaCore->GetConfigurationManager();
			if (config != nullptr)
			{
				if (!CreateConfigPath(std::string(m_ashitaCore->GetInstallPath()) + "config\\pivot",
									  std::string(m_ashitaCore->GetInstallPath()) + "config\\pivot.ini"))
				{
					return false;
				}

				if (!m_settings.load(config))
				{
					logMessageF(LogLevel::Warn, "Failed to load config file, saving defaults instead");
					m_settings.save(config);
				}

				redirector.setDebugLog(m_settings.debugLog);
				redirector.setRootPath(m_settings.rootPath);
				for (const auto& path : m_settings.overlays)
				{
					redirector.addOverlay(path);
				}

				m_ui.setCachePurgeDelay(m_settings.cachePurgeDelay);

				if (m_settings.cacheEnabled)
				{
					memCache.setLogProvider(this);
					memCache.setDebugLog(m_settings.debugLog);
					memCache.setCacheAllocation(m_settings.cacheSize);
				}
				m_settings.save(config);
			}

			if (m_settings.cacheEnabled)
			{
				initialized &= memCache.setupHooks();
			}
			initialized &= redirector.setupHooks();

			return initialized;
		}

		void AshitaInterface::Release(void)
		{
			if (Core::MemCache::instance().hooksActive() == true)
			{
				Core::MemCache::instance().releaseHooks();
			}
			Core::Redirector::instance().releaseHooks();
			IPolPlugin::Release();
		}

		bool AshitaInterface::HandleCommand(int32_t /*type*/, const char* command, bool /*injected*/)
		{
			std::vector<std::string> args;
			Ashita::Commands::GetCommandArgs(command, &args);

			HANDLECOMMAND("/pivot")
			{
				args.erase(args.begin());
				return m_ui.HandleCommand(m_ashitaCore->GetChatManager(), args);
			}
			return false;
		}

		void AshitaInterface::HandleEvent(const char* eventName, const void* eventData, const uint32_t eventSize)
		{
		}

		void AshitaInterface::Direct3DEndScene(bool isRenderingBackBuffer)
		{
			if (isRenderingBackBuffer == true)
			{
				if (m_settings.dirty == true)
				{
					/* settings have changed during the last EndScene or Present */
					m_settings.debugLog = Core::Redirector::instance().getDebugLog();
					m_settings.rootPath = Core::Redirector::instance().rootPath();
					m_settings.overlays = Core::Redirector::instance().overlayList();

					m_settings.cacheEnabled    = Core::MemCache::instance().hooksActive();
					m_settings.cacheSize       = Core::MemCache::instance().getCacheAllocation();
					m_settings.cachePurgeDelay = static_cast<uint32_t>(m_ui.getCachePurgeDelay());

					m_settings.save(m_ashitaCore->GetConfigurationManager());
				}

				m_ui.ProcessUI(m_settings.dirty);
			}
		}

		void AshitaInterface::Direct3DPresent(const RECT*, const RECT*, HWND, const RGNDATA*)
		{
			m_ui.RenderUI(m_ashitaCore->GetGuiManager());
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
					ashitaLevel = Ashita::LogLevel::Info;
					break;

				case Core::ILogProvider::LogLevel::Warn:
					ashitaLevel = Ashita::LogLevel::Warn;
					break;

				case Core::ILogProvider::LogLevel::Error:
					ashitaLevel = Ashita::LogLevel::Error;
					break;
				}

				va_list args;
				va_start(args, msg);

				vsnprintf_s(msgBuf, 511, msg.c_str(), args);
				m_logManager->Log(static_cast<uint32_t>(ashitaLevel), GetName(), msgBuf);

				va_end(args);
			}
		}

		/* private parts below */

		bool AshitaInterface::CreateConfigPath(const std::string& path, const std::string& moveOld)
		{
			std::istringstream pathReader(path);
			std::stringstream pathPos;
			std::string dirName;

			while (std::getline(pathReader, dirName, '\\')) {
				if (dirName.empty()) {
					break;
				}

				if (pathPos.str().empty()) 
				{
					// drive letter 
					pathPos << dirName;
				}
				else
				{
					pathPos << "\\" << dirName;
				}

				auto attrib = GetFileAttributesA(pathPos.str().c_str());
				if (attrib == INVALID_FILE_ATTRIBUTES) 
				{
					logMessageF(LogLevel::Debug, "trying to create directory '%s'", pathPos.str().c_str());
					if (CreateDirectoryA(pathPos.str().c_str(), nullptr) != TRUE)
					{
						logMessageF(LogLevel::Error, "unable to create directory '%s'", pathPos.str().c_str());
						return false;
					}
				}
				else if (!(attrib & FILE_ATTRIBUTE_DIRECTORY))
				{
					logMessageF(LogLevel::Error, "path '%s' already exists and is not a directory", pathPos.str().c_str());
					return false;
				}
			}

			if (!moveOld.empty())
			{
				auto attrib = GetFileAttributesA(moveOld.c_str());
				if (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY))
				{
					auto newPath = path + moveOld.substr(moveOld.rfind('\\'));

					if (GetFileAttributesA(newPath.c_str()) == INVALID_FILE_ATTRIBUTES)
					{
						logMessageF(LogLevel::Debug, "moving existing configuration '%s' to '%s'", moveOld.c_str(), newPath.c_str());
						MoveFileA(moveOld.c_str(), newPath.c_str());
					}
					else
					{
						logMessageF(LogLevel::Debug, "unable to move existing configuration '%s' to '%s' (file exists)", moveOld.c_str(), newPath.c_str());
					}
				}
			}
			return true;
		}

		AshitaInterface::Settings::Settings()
		{
			char workPath[MAX_PATH];

			GetCurrentDirectoryA(MAX_PATH, static_cast<LPSTR>(workPath));

			/* default to "plugin location"\\DATs */
			rootPath = std::string(workPath) + "\\DATs";
			overlays.clear();
			debugLog = false;
			cacheEnabled = false;
			cacheSize = 0;
			cachePurgeDelay = 600;
			dirty = false;
		}

		bool AshitaInterface::Settings::load(IConfigurationManager* config)
		{
			dirty = false;
			if (config->Load(PluginName, ConfigPath))
			{
				const char* rP = config->GetString(PluginName, "settings", "root_path");
				const bool dbg = config->GetBool(PluginName, "settings", "debug_log", true);

				debugLog = dbg;
				rootPath = (rP ? rP : rootPath);

				overlays.clear();

				unsigned overlayIndex = 0;
				char overlayIndexStr[10];
				char* overlayName = nullptr;

				do
				{
					snprintf(overlayIndexStr, sizeof(overlayIndexStr) - 1, "%d", overlayIndex++);
					overlayName = const_cast<char*>(config->GetString(PluginName, "overlays", overlayIndexStr));

					if (overlayName != nullptr && strcmp(overlayName, "") != 0)
					{
						overlays.push_back(overlayName);
					}
				} while (overlayName != nullptr);

				cacheEnabled = config->GetBool(PluginName, "cache", "enabled", false);
				cacheSize = config->GetInt32(PluginName, "cache", "size", 2048) * 0x100000;
				cachePurgeDelay = config->GetInt32(PluginName, "cache", "max_age", 600);

				return true;
			}
			return false;
		}

		void AshitaInterface::Settings::save(IConfigurationManager* config)
		{
			config->Delete(PluginName);

			config->SetValue(PluginName, "settings", "root_path", rootPath.c_str());
			config->SetValue(PluginName, "settings", "debug_log", debugLog ? "true" : "false");

			for (unsigned i = 0; i < overlays.size(); ++i)
			{
				char key[10];
				snprintf(key, 9, "%d", i);

				config->SetValue(PluginName, "overlays", key, overlays.at(i).c_str());
			}

			config->SetValue(PluginName, "cache", "enabled", cacheEnabled ? "true" : "false");

			char val[32];
			snprintf(val, 31, "%u", cacheSize / 0x100000);
			config->SetValue(PluginName, "cache", "size", val);

			snprintf(val, 31, "%u", cachePurgeDelay);
			config->SetValue(PluginName, "cache", "max_age", val);

			config->Save(PluginName, ConfigPath);
			dirty = false;
		}
	}
}
