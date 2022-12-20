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

#include <vector>
#include <string>

#include "ADK_v4/Ashita.h"

#include "MemCache.h"

namespace XiPivot
{
	namespace Pol
	{
		class UserInterface final
		{
			public:
				UserInterface();
				virtual ~UserInterface();

				void setCachePurgeDelay(time_t maxAge);
				time_t getCachePurgeDelay(void) const { return m_cachePurgeDelay; }

				bool HandleCommand(IAshitaCore* const core, std::vector<std::string>& args);

				void ProcessUI(bool &settingsChanged);
				void RenderUI(IGuiManager* const imgui);

			private:
				std::vector<std::string> ListAllOverlays(const std::string& rootPath);

				void PrintHelp(IChatManager* const chat);

				void RenderOverlayConfigUI(IGuiManager* const imgui);
				void RenderMemCacheConfigUI(IGuiManager* const imgui);
				void RenderAdvancedConfigUI(IGuiManager* const imgui);
				void RenderCacheStatsUI(IGuiManager* const imgui);

			private:
				struct
				{
					struct
					{
						bool debugLog;
						bool cacheEnable;
						bool redirectCFW;
					} flags;

					struct
					{
						int32_t                            cacheSizeMB;
						int32_t                            cachePurgeDelay;
						struct Core::MemCache::CacheStatus cacheStats;
					} values;

					struct
					{
						std::string              rootPath;
						std::vector<std::string> allOverlays;
						std::vector<std::string> activeOverlays;
					} constants;

					struct
					{
						std::string addOverlayName;
						std::string deleteOverlayName;

						bool showConfigWindow;
						bool showCacheOverlay;
						bool applyCacheChanges;
						bool applyCLIChanges;

						int  activeTab;
					} state;

				} m_guiState;

				time_t m_cachePurgeDelay;
				time_t m_cacheNextPurge;
		};
	}
}

