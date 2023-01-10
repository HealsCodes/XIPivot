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

#pragma once

#include "ADK_v4/Ashita.h"

#include "Redirector.h"
#include "UserInterface.h"
#include <filesystem>

namespace XiPivot
{
	namespace Pol
	{
		static constexpr auto PluginName = "pivot";
		static constexpr auto PluginAuthor = "Heals";
		static constexpr auto PluginVersion = 4.1500;
		static constexpr auto PluginUrl = "https://github.com/Shirk/XIPivot";
		static constexpr auto PluginDescr = "Runtime DAT, sfx and bgm replacement manager.";
		static constexpr auto PluginCommand = "pivot";

		class AshitaInterface : public IPolPlugin, public Core::IDelegate
		{
		public:
			AshitaInterface(const char* args);
			virtual ~AshitaInterface(void) {};

			/* Ashita plugin requirements */
			bool Initialize(IAshitaCore* core, ILogManager* log, uint32_t id) override;
			void Release(void) override;

			const char* GetName(void) const override;
			const char* GetAuthor(void) const override;
			const char* GetDescription(void) const override;
			const char* GetLink(void) const override;

			double GetVersion(void) const override;
			uint32_t GetFlags(void) const override;

			/* Required handlers based an flags */
			bool HandleCommand(int32_t type, const char* command, bool injected) override;
			void HandleEvent(const char*, const void*, const uint32_t) override;

			void Direct3DEndScene(bool isRenderingBackBuffer) override;
			void Direct3DPresent(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) override;

			/* IDelegate */
			void logMessage(Core::IDelegate::LogLevel level, std::string msg) override;
			void logMessageF(Core::IDelegate::LogLevel level, std::string msg, ...) override;

		protected:
			virtual bool runCFWHook(const wchar_t* path) override;

		private:

			bool CreateConfigPath(const std::filesystem::path& path, const std::filesystem::path& moveOld);

			struct Settings
			{
				Settings();

				bool load(IConfigurationManager* config, const std::filesystem::path& relPath);
				void save(IConfigurationManager* config, const std::filesystem::path& relPath, const std::filesystem::path& absPath);
				void dump(Core::IDelegate* log);

				bool debugLog;
				std::filesystem::path rootPath;
				std::vector<std::string> overlays;

				bool cacheEnabled;
				uint32_t cacheSize;
				uint32_t cachePurgeDelay;

				bool redirectCFW;

				bool dirty;
			};

			IAshitaCore*             m_ashitaCore = nullptr;
			ILogManager*             m_logManager = nullptr;

			std::vector<std::string> m_pluginArgs;
			std::filesystem::path    m_settingsRelPath;
			std::filesystem::path    m_settingsPath;
			Settings                 m_settings;
			UserInterface            m_ui;
		};
	}
}

