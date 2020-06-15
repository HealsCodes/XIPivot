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

#pragma once

#include "ADK_v4/Ashita.h"
#include "../IPolRemoteInterface.h"

#define IMGUI_FORMAT_WORKAROUND // right now needed because v4 has broken imgui format strings

namespace XiPivot
{
	namespace Plugin
	{
		class AshitaUI : public IPlugin
		{

		public:
			AshitaUI(const char* args);
			virtual ~AshitaUI(void) {};

			/* Ashita plugin requirements */
			const char* GetName(void) const override;
			const char* GetAuthor(void) const override;
			const char* GetDescription(void) const override;
			const char* GetLink(void) const override;
			double GetVersion(void) const override;
			double GetInterfaceVersion(void) const override;
			int32_t GetPriority(void) const override;
			uint32_t GetFlags(void) const override;


			bool Initialize(IAshitaCore* core, ILogManager* log, uint32_t id) override;
			void Release(void) override;

			bool HandleCommand(int32_t mode, const char* command, bool injected) override;

			bool Direct3DInitialize(IDirect3DDevice8* device) override;
			void Direct3DEndScene(bool isRenderingBackBuffer) override;
			void Direct3DPresent(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) override;

		private:
#ifdef IMGUI_FORMAT_WORKAROUND
			std::string formatStr(const char* format, ...);
#endif
			bool getPolRemote(IPolRemoteInterface** remoteInterface) const;

			/* helpers for GUI config */
			void renderOverlayConfigUI(IGuiManager* imgui);
			void renderMemCacheConfigUI(IGuiManager* imgui);
			//void renderAboutUI(IGuiManager* imgui);
			void renderCacheStatsUI(IGuiManager* imgui);

			std::vector<std::string> listAvailableOverlays() const;

			/* UI state */
			bool                 m_showConfigWindow;
			bool                 m_showCacheWindow;

			struct
			{
				int                      activeTab;

				/* overlays */
				bool                     debugState;
				std::string              purgeOverlay;
				std::vector<std::string> allOverlays;

				/* cache */
				bool                     cacheState;
				int32_t                  cacheSizeMB;
				int32_t                  cachePurgeDelay;
				bool                     applyCacheChanges;
			} m_uiConfig;

			time_t m_nextCachePurge;
		};
	}
}

