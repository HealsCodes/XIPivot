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

#include "AshitaUI.h"
#include "..\polplugin\AshitaInterface.h"

#include <regex>

DEFINE_ENUMCLASS_OPERATORS(Ashita::PluginFlags);

namespace XiPivot
{
	namespace Plugin
	{
		AshitaUI::AshitaUI(const char* args)
			: IPlugin(),

			m_showConfigWindow(false)
		{
			m_uiConfig.debugState = false;
			m_uiConfig.purgeOverlay.clear();
		}

		const char* AshitaUI::GetName(void) const        { return u8"XIPivot"; }
		const char* AshitaUI::GetAuthor(void) const      { return u8"Heals"; }
		const char* AshitaUI::GetDescription(void) const { return u8"configuration UI for XIPivotPol"; }
		const char* AshitaUI::GetLink(void) const        { return u8"https://github.com/Shirk/XIPivot"; }
		double      AshitaUI::GetVersion(void) const     { return 4.01; }

		double      AshitaUI::GetInterfaceVersion(void) const { return ASHITA_INTERFACE_VERSION; }
		int32_t     AshitaUI::GetPriority(void) const         { return 0; }

		uint32_t AshitaUI::GetFlags(void) const { return (uint32_t)(Ashita::PluginFlags::UseCommands | Ashita::PluginFlags::UseDirect3D); }


		bool AshitaUI::Initialize(IAshitaCore* core, ILogManager* log, uint32_t id)
		{
			IPlugin::Initialize(core, log, id);
			IPolRemoteInterface* polRemote = nullptr;
			if (getPolRemote(&polRemote) == true)
			{
				return true;
			}

			std::ostringstream msg;
			msg << Ashita::Chat::Header(GetName()) << Ashita::Chat::Error("error: please ensure XIPivotPol is loaded in your polplugins");
			m_AshitaCore->GetChatManager()->Write(1, false, msg.str().c_str());
			return false;
		}

		void AshitaUI::Release(void)
		{
			IPlugin::Release();
		}

		bool AshitaUI::HandleCommand(int32_t /*type*/, const char* command, bool injected)
		{
			std::ostringstream msg;
			std::vector<std::string> args;
			Ashita::Commands::GetCommandArgs(command, &args);

			HANDLECOMMAND("/pivot")
			{
				IPolRemoteInterface* polRemote = nullptr;
				if (getPolRemote(&polRemote) == true)
				{

					if (args.size() == 3)
					{
						if (args[1] == "a" || args[1] == "add")
						{
							if (polRemote->AddOverlay(args[2]) == false)
							{
								msg.str(u8"");
								msg << Ashita::Chat::Header(GetName()) << Ashita::Chat::Error(u8" failed to add ") << Ashita::Chat::Color1(3, args[2]);
								m_AshitaCore->GetChatManager()->Write(1, false, msg.str().c_str());
							}

						}
						else if (args[1] == "r" || args[1] == "remove")
						{
							polRemote->RemoveOverlay(args[2]);
						}
					}
					else if (args.size() == 2 && (args[1] == "h" || args[1] == "help"))
					{
						msg.str(u8"");
						msg << Ashita::Chat::Color1(0x53, GetName())
							<< Ashita::Chat::Color2(0x82, u8" v")
							<< Ashita::Chat::Color1(0x53, "%.2f")
							<< Ashita::Chat::Color2(0x82, u8" by ")
							<< Ashita::Chat::Color1(0x53, GetAuthor());
						m_AshitaCore->GetChatManager()->Writef(1, false, msg.str().c_str(), GetVersion());

						msg.str(u8"");
						msg << Ashita::Chat::Color1(0x47, u8"a")
							<< Ashita::Chat::Color1(0x52, u8"dd overlay_dir")
							<< Ashita::Chat::Color2(0x82, u8" - Adds a path to be searched for DAT overlays");
						m_AshitaCore->GetChatManager()->Write(1, false, msg.str().c_str());

						msg.str(u8"");
						msg << Ashita::Chat::Color1(0x47, u8"r")
							<< Ashita::Chat::Color1(0x52, u8"emove overlay_dir")
							<< Ashita::Chat::Color2(0x82, u8" - Removes a path from the DAT overlays");
						m_AshitaCore->GetChatManager()->Write(1, false, msg.str().c_str());

						msg.str(Ashita::Chat::Color1(0x53, u8"-"));
						m_AshitaCore->GetChatManager()->Write(1, false, msg.str().c_str());

						msg.str(u8"");
						msg << Ashita::Chat::Color1(0x47, u8"c")
							<< Ashita::Chat::Color1(0x52, u8"ache")
							<< Ashita::Chat::Color2(0x82, u8" - show cache statistics overlay");
						m_AshitaCore->GetChatManager()->Write(1, false, msg.str().c_str());

						msg.str(Ashita::Chat::Color1(0x53, u8"-"));
						m_AshitaCore->GetChatManager()->Write(1, false, msg.str().c_str());

						msg.str(u8"");
						msg << Ashita::Chat::Color1(0x47, u8"<no args>")
							<< Ashita::Chat::Color2(0x82, u8" - show the configuration UI (you want this)");
						m_AshitaCore->GetChatManager()->Write(1, false, msg.str().c_str());

						msg.str(Ashita::Chat::Color1(0x53, u8"-"));
						m_AshitaCore->GetChatManager()->Write(1, false, msg.str().c_str());

						msg.str(u8"");
						msg << Ashita::Chat::Color2(0x82, u8"Adding or removing overlays at runtime can cause ")
							<< Ashita::Chat::Color1(0x53, u8"all kinds of unexpected behaviour.");
						m_AshitaCore->GetChatManager()->Write(1, false, msg.str().c_str());

						msg.str(u8"");
						msg << Ashita::Chat::Color2(0x82, u8"You have been warned.");
						m_AshitaCore->GetChatManager()->Write(1, false, msg.str().c_str());

					}
					else if (args.size() == 2 && (args[1] == "c" || args[1] == "cache") && polRemote->GetCacheState() == true)
					{
						m_showCacheWindow = !m_showCacheWindow;
					}
					else
					{
						m_uiConfig.debugState = polRemote->GetDebugLogState();
						m_uiConfig.purgeOverlay.clear();
						m_uiConfig.allOverlays = listAvailableOverlays();

						m_uiConfig.applyCacheChanges = false;
						m_uiConfig.cacheState = polRemote->GetCacheState();
						m_uiConfig.cacheSizeMB = polRemote->GetCacheSize() / 0x100000;
						m_uiConfig.cachePurgeDelay = polRemote->GetCachePurgeDelay();

						m_showConfigWindow = true;
					}
				}
				else
				{
					m_AshitaCore->GetChatManager()->Write(1, false, Ashita::Chat::Error(u8"Unable to communicate with XIPivotPol polplugin.").c_str());
				}
				return true;
			}
			return false;
		}

		bool AshitaUI::Direct3DInitialize(IDirect3DDevice8* device)
		{
			IPlugin::Direct3DInitialize(device);
			return true;
		}

		void AshitaUI::Direct3DEndScene(bool isRenderingBackBuffer)
		{
			if (isRenderingBackBuffer == false)
			{
				return;
			}

			IPolRemoteInterface* polRemote = nullptr;
			if (getPolRemote(&polRemote) == true)
			{
				if (polRemote->GetDebugLogState() != m_uiConfig.debugState)
				{
					polRemote->SetDebugLogState(m_uiConfig.debugState);
				}
				if (m_uiConfig.purgeOverlay.empty() == false)
				{
					polRemote->RemoveOverlay(m_uiConfig.purgeOverlay);
					m_uiConfig.purgeOverlay.clear();
				}

				if (m_uiConfig.applyCacheChanges == true)
				{
					m_uiConfig.applyCacheChanges = false;

					polRemote->SetCacheParams(m_uiConfig.cacheState, m_uiConfig.cacheSizeMB * 0x100000, m_uiConfig.cachePurgeDelay);
				}

				if (polRemote->GetCacheState() == true)
				{
					const time_t now = time(nullptr);
					if (now > m_nextCachePurge)
					{
						m_nextCachePurge = now + polRemote->GetCachePurgeDelay();
						polRemote->PurgeCacheObjects(polRemote->GetCachePurgeDelay());
					}
				}
			}
		}

		void AshitaUI::Direct3DPresent(const RECT* /*pSourceRect*/, const RECT* /*pDestRect*/, HWND /*hDestWindowOverride*/, const RGNDATA* /*pDirtyRegion*/)
		{
			const auto imgui = m_AshitaCore->GetGuiManager();

			if (m_showConfigWindow)
			{
				imgui->SetNextWindowBgAlpha(.9f);
				imgui->SetNextWindowSize(ImVec2(600, 550));
				if (imgui->Begin(u8"XiPivot Setup", &m_showConfigWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize) == true)
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
				imgui->SetNextWindowBgAlpha(0.33f);
				if (imgui->Begin(u8"XiPivot Cache", &m_showCacheWindow, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar) == true)
				{
					renderCacheStatsUI(imgui);
				}
				imgui->End();
			}
		}

		/* private parts below */

		bool AshitaUI::getPolRemote(IPolRemoteInterface** remoteInterface) const
		{
			try
			{
				auto polPlugin = (IPolPlugin*)m_AshitaCore->GetPolPluginManager()->Get("XIPivotPol");
				if (polPlugin != nullptr)
				{
					*remoteInterface = reinterpret_cast<Pol::AshitaInterface*>(polPlugin);
					return true;
				}
			}
			catch(...) { }
			return false;
		}

		/* GUI stuff */
#ifdef IMGUI_FORMAT_WORKAROUND
		std::string AshitaUI::formatStr(const char* format, ...)
		{
			char buffer[128];
			va_list args;

			va_start(args, format);
			vsnprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, format, args);
			va_end(args);

			return std::string(buffer);
		}
#endif

		std::vector<std::string> AshitaUI::listAvailableOverlays() const
		{
			std::vector<std::string> res;
			IPolRemoteInterface* polRemote = nullptr;

			if (getPolRemote(&polRemote) == true)
			{
				HANDLE hRef;
				WIN32_FIND_DATAA data;
				auto pattern = polRemote->GetRootPath() + "/*";

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
			}

			return res;
		}

		void AshitaUI::renderOverlayConfigUI(IGuiManager* imgui)
		{
			IPolRemoteInterface* polRemote = nullptr;
			if (getPolRemote(&polRemote) == true)
			{

				imgui->Checkbox(u8"debug log", &m_uiConfig.debugState);
				imgui->LabelText(u8"root path", polRemote->GetRootPath().c_str());
				imgui->Text(u8"active overlays:");
				imgui->BeginChild(u8"overlay_list", ImVec2(0, 200));
				{
					if (m_uiConfig.purgeOverlay.empty())
					{
						int prio = 0;
						for (const auto& path : polRemote->GetOverlays())
						{
							/* each button in imgui needs a unique id (title##id)
							 * since this list is generated at runtime button IDs default to ASCII 'a' + prio
							 * this way the first list button will be ' - ##a', the second ' - ##b' and so on.
							 */
							char btnId[] = { ' ', '-', ' ', '#', '#', static_cast<char>(prio + 'a') };
							if (imgui->Button(btnId))
							{
								m_uiConfig.purgeOverlay = path;
							}
							imgui->SameLine();
#ifdef IMGUI_FORMAT_WORKAROUND
							imgui->Text(formatStr(u8"[%02d] %s", prio++, path.c_str()).c_str());
#else
							imgui->Text(u8"[%02d] %s", prio++, path.c_str());
#endif
						}
					}
				}
				imgui->EndChild();
				imgui->Separator();

				imgui->Text(u8"available overlays:");
				imgui->BeginChild(u8"available_overlays", ImVec2(0, 200));
				{
					int n = 0;
					const auto l = polRemote->GetOverlays();
					for (const auto& path : m_uiConfig.allOverlays)
					{
						if (std::find(l.begin(), l.end(), path) == l.end())
						{
							// n is the index of the overlay in the list of all overlays which should be constant
							char btnId[] = { ' ', '+', ' ', '#', '#', static_cast<char>(n + 'a') };
							if (imgui->Button(btnId))
							{
								polRemote->AddOverlay(path);
							}
							imgui->SameLine();
							imgui->Text(path.c_str());
						}
						++n;
					}
				}
				imgui->EndChild();

				imgui->Separator();
				imgui->TextDisabled(u8"Adding or removing overlays at runtime can cause all kinds of unexpected behaviour.");
				imgui->TextDisabled(u8"You have been warned");
			}
			else
			{
				imgui->TextDisabled(u8"Unable to communicate with XIPivotPol polplugin.");
			}
		}

		void AshitaUI::renderMemCacheConfigUI(IGuiManager* imgui)
		{
			IPolRemoteInterface* polRemote = nullptr;
			if (getPolRemote(&polRemote) == true)
			{
				imgui->Checkbox(u8"use cache", &m_uiConfig.cacheState);
				imgui->SliderInt(u8"reserved size", &m_uiConfig.cacheSizeMB, 1, 4096, "%.0f mb");
				imgui->SliderInt(u8"purge interval", &m_uiConfig.cachePurgeDelay, 1, 600, "%.0f sec");

				imgui->Separator();
				if (imgui->Button(u8"apply##cache settings", ImVec2(500, 0)))
				{
					m_uiConfig.applyCacheChanges = true;
				}
				imgui->NewLine();
				imgui->TextDisabled(u8"Reducing the cache size will not instantly take effect if the currently");
				imgui->TextDisabled(u8"used cache size is larger than the new maximum.");
				imgui->TextDisabled(u8"Cached files that have no open handle will be removed after the purge delay.");

				if (polRemote->GetCacheState() == true)
				{
					imgui->NewLine();
					imgui->Separator();
					renderCacheStatsUI(imgui);
				}
			}
			else
			{
				imgui->TextDisabled(u8"Unable to communicate with XIPivotPol polplugin.");
			}
		}

		void AshitaUI::renderCacheStatsUI(IGuiManager* imgui)
		{
			IPolRemoteInterface* polRemote = nullptr;
			if (getPolRemote(&polRemote) == true)
			{
				const auto stats = polRemote->GetCacheStats();
				if (stats.cacheHits != 0 || stats.cacheMisses != 0)
				{
#ifdef IMGUI_FORMAT_WORKAROUND
					imgui->LabelText(u8"cache hits", formatStr("%d%%", stats.cacheHits * 100 / (stats.cacheHits + stats.cacheMisses)).c_str());
#else
					imgui->LabelText(u8"cache hits", "%d%%", stats.cacheHits * 100 / (stats.cacheHits + stats.cacheMisses));
#endif
					imgui->Separator();
				}
#ifdef IMGUI_FORMAT_WORKAROUND
				imgui->LabelText(u8"allocation", formatStr("%.2fmb", stats.allocation / 1048576.0f).c_str());
				imgui->LabelText(u8"used size", formatStr("%.2fmb", stats.used / 1048576.0f).c_str());
				imgui->LabelText(u8"objects", formatStr("%d", stats.activeObjects).c_str());
				imgui->Separator();

				imgui->LabelText(u8"next purge in", formatStr("%ds", m_nextCachePurge - time(nullptr)).c_str());
#else
				imgui->LabelText(u8"allocation", "%.2fmb", stats.allocation / 1048576.0f);
				imgui->LabelText(u8"used size", "%.2fmb", stats.used / 1048576.0f);
				imgui->LabelText(u8"objects", "%d", stats.activeObjects);
				imgui->Separator();

				imgui->LabelText(u8"next purge in", "%ds", m_nextCachePurge - time(nullptr));
#endif
			}
			else
			{
				imgui->TextDisabled(u8"Unable to communicate with XIPivotPol polplugin.");
			}
		}
	}
}
