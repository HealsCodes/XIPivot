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

			m_settingsPath = std::filesystem::path(m_ashitaCore->GetInstallPath()) / "config" / "pivot" / "pivot.ini";

			auto config = m_ashitaCore->GetConfigurationManager();
			if (config != nullptr)
			{
				if (!CreateConfigPath(m_settingsPath,
									  std::filesystem::path(m_ashitaCore->GetInstallPath()) / "config" / "pivot.ini"))
				{
					return false;
				}

				if (!m_settings.load(config))
				{
					logMessageF(LogLevel::Warn, "Failed to load config file, saving defaults instead");
					m_settings.save(config, m_settingsPath);
				}
				m_settings.dump(this);

				redirector.setDebugLog(m_settings.debugLog);
				redirector.setRootPath(m_settings.rootPath.string());
				redirector.setRedirectCreateFileW(m_settings.redirectCFW);

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
				m_settings.save(config, m_settingsPath);
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
				return m_ui.HandleCommand(m_ashitaCore, args);
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
					logMessageF(Core::IDelegate::LogLevel::Info, "settings changed, saving");

					/* settings have changed during the last EndScene or Present */
					m_settings.debugLog = Core::Redirector::instance().getDebugLog();
					m_settings.rootPath = Core::Redirector::instance().rootPath();
					m_settings.overlays = Core::Redirector::instance().overlayList();

					m_settings.redirectCFW = Core::Redirector::instance().getRedirectCreateFileW();

					m_settings.cacheEnabled    = Core::MemCache::instance().hooksActive();
					m_settings.cacheSize       = Core::MemCache::instance().getCacheAllocation();
					m_settings.cachePurgeDelay = static_cast<uint32_t>(m_ui.getCachePurgeDelay());

					m_settings.save(m_ashitaCore->GetConfigurationManager(), m_settingsPath);
					m_settings.dump(this);
				}

				m_ui.ProcessUI(m_settings.dirty);
			}
		}

		void AshitaInterface::Direct3DPresent(const RECT*, const RECT*, HWND, const RGNDATA*)
		{
			m_ui.RenderUI(m_ashitaCore->GetGuiManager());
		}

		/* IDelegate */

		void AshitaInterface::logMessage(Core::IDelegate::LogLevel level, std::string msg)
		{
			logMessageF(level, msg);
		}

		void AshitaInterface::logMessageF(Core::IDelegate::LogLevel level, std::string msg, ...)
		{
			if (level != Core::IDelegate::LogLevel::Discard)
			{
				char msgBuf[512];
				Ashita::LogLevel ashitaLevel = Ashita::LogLevel::None;

				switch (level)
				{
				case Core::IDelegate::LogLevel::Discard: /* never acutally reached */
					return;

				case Core::IDelegate::LogLevel::Debug:
					ashitaLevel = Ashita::LogLevel::Debug;
					break;

				case Core::IDelegate::LogLevel::Info:
					ashitaLevel = Ashita::LogLevel::Info;
					break;

				case Core::IDelegate::LogLevel::Warn:
					ashitaLevel = Ashita::LogLevel::Warn;
					break;

				case Core::IDelegate::LogLevel::Error:
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

		/* protected parts */

		bool AshitaInterface::runCFWHook(const wchar_t*)
		{
			return m_ashitaCore->GetDirect3DDevice() != nullptr;
		}

		/* private parts below */

		bool AshitaInterface::CreateConfigPath(const std::filesystem::path& path, const std::filesystem::path& moveOld)
		{
			auto configDir = std::filesystem::path(path);
			configDir.remove_filename();

			if (std::filesystem::is_directory(configDir) == false)
			{
				if (std::filesystem::exists(configDir))
				{
					logMessageF(LogLevel::Error, "path '%s' already exists and is not a directory", configDir.string().c_str());
					return false;
				}

				if (std::filesystem::create_directories(configDir) == false)
				{
					logMessageF(LogLevel::Error, "unable to create directory '%s'", configDir.string().c_str());
					return false;
				}
			}

			if (!moveOld.empty())
			{
				if (std::filesystem::exists(moveOld))
				{
					if (std::filesystem::exists(path))
					{
						logMessageF(LogLevel::Debug, "unable to move existing configuration '%s' to '%s' (file exists)", moveOld.string().c_str(), path.string().c_str());
					}
					else
					{
						logMessageF(LogLevel::Debug, "moving existing configuration '%s' to '%s'", moveOld.string().c_str(), path.string().c_str());
						std::filesystem::rename(moveOld, path);
					}
				}
			}
			return true;
		}

		AshitaInterface::Settings::Settings()
		{
			/* default to "plugin location"\\DATs */
			rootPath = std::filesystem::current_path() / "DATs";
			overlays.clear();
			debugLog = false;
			redirectCFW = false;
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
				const char* rP  = config->GetString(PluginName, "settings", "root_path");
				const bool dbg  = config->GetBool(PluginName, "settings", "debug_log", false);
				const bool rCFW = config->GetBool(PluginName, "settings", "redirect_cfw", false);

				debugLog = dbg;
				rootPath = (rP ? rP : rootPath);
				redirectCFW = rCFW;

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
				cacheSize = config->GetInt32(PluginName, "cache", "size", 128) * 0x100000;
				cachePurgeDelay = config->GetInt32(PluginName, "cache", "max_age", 600);

				return true;
			}
			return false;
		}

		void AshitaInterface::Settings::dump(IDelegate* log)
		{
			log->logMessage(Core::IDelegate::LogLevel::Debug, ">> pivot settings <<");
			log->logMessageF(Core::IDelegate::LogLevel::Debug, "root_path %s", rootPath.string().c_str());
			log->logMessageF(Core::IDelegate::LogLevel::Debug, "debug_log %s", debugLog ? "true" : "false");
			log->logMessageF(Core::IDelegate::LogLevel::Debug, "redirect_cfw %s", redirectCFW ? "true" : "false");
			log->logMessage(Core::IDelegate::LogLevel::Debug, "overlays:");

			for (unsigned i = 0; i < overlays.size(); ++i)
			{
				log->logMessageF(Core::IDelegate::LogLevel::Debug, "[%02d] %s", i, overlays[i].c_str());
			}

			log->logMessageF(Core::IDelegate::LogLevel::Debug, "cache: %s", cacheEnabled ? "true" : "false");
			log->logMessageF(Core::IDelegate::LogLevel::Debug, "cache_size: %u", cacheSize);
			log->logMessageF(Core::IDelegate::LogLevel::Debug, "cache_max_age: %u", cachePurgeDelay);
			log->logMessageF(Core::IDelegate::LogLevel::Debug, "");
		}

		void AshitaInterface::Settings::save(IConfigurationManager* config, const std::filesystem::path& absPath)
		{
			config->Delete(PluginName);

			config->SetValue(PluginName, "settings", "root_path", rootPath.string().c_str());
			config->SetValue(PluginName, "settings", "debug_log", debugLog ? "true" : "false");
			config->SetValue(PluginName, "settings", "redirect_cfw", redirectCFW ? "true" : "false");

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

			if (std::filesystem::exists(absPath))
			{
				std::filesystem::remove(absPath);
			}

			config->Save(PluginName, ConfigPath);
			dirty = false;
		}
	}
}
