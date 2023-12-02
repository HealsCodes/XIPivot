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

#include "UserInterface.h"

#include "AshitaInterface.h"
#include "Redirector.h"

#include <fstream>

#define IS_PARAM(arg, abbr, full) if((arg) == (abbr) || (arg) == (full))

namespace XiPivot
{
	namespace Pol
	{
		UserInterface::UserInterface()
		{
		}

		UserInterface::~UserInterface()
		{
		}

		bool UserInterface::HandleCommand(IAshitaCore* const core, std::vector<std::string>& args)
		{
			const auto chat = core->GetChatManager();

			switch (args.size())
			{
				case 0:
					/* no parameters - toggle the configuration window */
					m_guiState.state.showConfigWindow = !m_guiState.state.showConfigWindow;

					if (m_guiState.state.showConfigWindow == true)
					{
						/* lazy update the GUI values */
						m_guiState.flags.debugLog = Core::Redirector::instance().getDebugLog();
						m_guiState.flags.redirectFOpenS = Core::Redirector::instance().getRedirectFOpenS();

						m_guiState.constants.rootPath = Core::Redirector::instance().rootPath();
						m_guiState.constants.allOverlays = ListAllOverlays(m_guiState.constants.rootPath);

						m_guiState.state.addOverlayName.clear();
						m_guiState.state.deleteOverlayName.clear();
						m_guiState.state.applyCLIChanges = false;
					}
					break;

				case 1:
					IS_PARAM(args.at(0), "h", "help")
					{
						PrintHelp(chat);
					}

					IS_PARAM(args.at(0), "d", "dump")
					{
						std::ostringstream msg;
						std::string dumpPath = (std::filesystem::path(core->GetInstallPath()) / "logs" / "pivot-dump.txt").string();
						std::fstream dumpFile;

						dumpFile.open(dumpPath, std::ios_base::out | std::ios_base::trunc);
						if (dumpFile.is_open())
						{
							dumpFile << "-- pivot overlay stats --" << std::endl;
							if (Core::Redirector::instance().hooksActive()) 
							{
								auto overlayList = XiPivot::Core::Redirector::instance().overlayList();
								dumpFile << "redirector: enabled" << std::endl
									     << "active overlays: " << overlayList.size() << std::endl;

								for (const auto& overlay : overlayList)
								{
									dumpFile << "- '" << overlay << "'" << std::endl;
								}
							}
							else
							{
								dumpFile << "redirector: disabled";
							}

							dumpFile.close();

							msg << Ashita::Chat::Header(PluginCommand) << Ashita::Chat::Message("Dumped statistics to ") << Ashita::Chat::Message(dumpPath);
						}
						else
						{
							msg << Ashita::Chat::Header(PluginCommand) << Ashita::Chat::Error("Unable to write to ") << Ashita::Chat::Error(dumpPath);
						}
						chat->AddChatMessage(1, false, msg.str().c_str());
					}
					break;

				default:
					PrintHelp(chat);
					break;
			}
			return true;
		}

		void UserInterface::ProcessUI(bool& settingsChanged)
		{
			/* Apply any parameter changes and actions that were made
			 * as a result of the last RenderUI call.
			 */

			auto& redirector = Core::Redirector::instance();

			if (m_guiState.state.showConfigWindow)
			{
				if (redirector.getDebugLog() != m_guiState.flags.debugLog)
				{
					/* update debug log states */
					redirector.setDebugLog(m_guiState.flags.debugLog);

					settingsChanged = true;
				}

				if (m_guiState.state.deleteOverlayName.empty() == false)
				{
					/* remove overlays if any */
					redirector.removeOverlay(m_guiState.state.deleteOverlayName);
					m_guiState.state.deleteOverlayName.clear();

					settingsChanged = true;
				}

				if (m_guiState.state.addOverlayName.empty() == false)
				{
					/* add overlays if any */
					redirector.addOverlay(m_guiState.state.addOverlayName);
					m_guiState.state.addOverlayName.clear();

					settingsChanged = true;
				}

				if (redirector.getRedirectFOpenS() != m_guiState.flags.redirectFOpenS)
				{
					redirector.setRedirectFOpenS(m_guiState.flags.redirectFOpenS);

					m_guiState.state.showRestartNotice = true;
					settingsChanged = true;
				}
			}

			/* handle changes made through the CLI */
			if (m_guiState.state.applyCLIChanges == true)
			{
				settingsChanged = true;
			}

			/* copy the current live cache stats and overlays for RenderUI */
			m_guiState.constants.activeOverlays = redirector.overlayList();
		}

		void UserInterface::RenderUI(IGuiManager* const imgui)
		{
			if (m_guiState.state.showConfigWindow == true)
			{
				const auto configWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

				imgui->SetNextWindowBgAlpha(.9f);
				imgui->SetNextWindowSize(ImVec2(600, 580));

				if (imgui->Begin("XiPivot Setup", &m_guiState.state.showConfigWindow, configWindowFlags) == true)
				{
					const char *tabTitles[] = { "overlays", "advanced", nullptr };
					for (int i = 0; tabTitles[i] != nullptr; ++i)
					{
						imgui->PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.69f, 1.0f, m_guiState.state.activeTab == i ? 0.8f : 0.1f));
						if (imgui->Button(tabTitles[i]))
						{
							m_guiState.state.activeTab = i;
						}
						imgui->PopStyleColor();
						imgui->SameLine();
					}
					imgui->NewLine();
					imgui->Separator();

					switch (m_guiState.state.activeTab)
					{
						case 0:
							RenderOverlayConfigUI(imgui);
							break;

						case 1:
							RenderAdvancedConfigUI(imgui);
							break;
					}
				}

				if (m_guiState.state.showRestartNotice)
				{
					imgui->NewLine();
					imgui->TextColored(ImVec4(1.0, 0.75, 0.55, 1.0), "Please restart your client for changes to take effect.");
					imgui->NewLine();
				}

				imgui->End();
			}
		}

		/* private parts */
		std::vector<std::string> UserInterface::ListAllOverlays(const std::string& rootPath)
		{
			std::vector<std::string> res;
			HANDLE hRef;
			WIN32_FIND_DATAA data;
			auto pattern = rootPath + "/*";

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

		void UserInterface::PrintHelp(IChatManager* chat)
		{
			std::ostringstream msg;

			msg.str("");
			msg << Ashita::Chat::Header(PluginCommand) << Ashita::Chat::Color1(0x6, PluginName) << " v" << Ashita::Chat::Color1(0x6, "%.3f") << " by " << Ashita::Chat::Color1(0x6, PluginAuthor);
			chat->Writef(1, false, msg.str().c_str(), PluginVersion);

			msg.str("");
			msg << Ashita::Chat::Header(PluginCommand) << Ashita::Chat::Color1(0x3, "d")         << "ump                  - dump overlay and cache statistics to logs\\pivot-dump.txt.";
			chat->AddChatMessage(1, false, msg.str().c_str());


			msg.str("");
			msg << Ashita::Chat::Header(PluginCommand) << Ashita::Chat::Color1(0x3, "<no args>") << "              - open the configuration UI.";
			chat->AddChatMessage(1, false, msg.str().c_str());
		}

		void UserInterface::RenderOverlayConfigUI(IGuiManager* const imgui)
		{
			imgui->Checkbox("debug log", &m_guiState.flags.debugLog);
			imgui->LabelText("root path", m_guiState.constants.rootPath.c_str());

			imgui->Text("active overlays:");
			imgui->BeginChild("overlay_list", ImVec2(0, 200));
			{
				if (imgui->BeginTable("active_overlays_table", 3, ImGuiTableFlags_NoSavedSettings))
				{

					imgui->TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 0);
					imgui->TableSetupColumn("Priority", ImGuiTableColumnFlags_WidthFixed, 0);
					imgui->TableSetupColumn("Overlay", ImGuiTableColumnFlags_WidthStretch, 1);
					imgui->TableHeadersRow();

					if (m_guiState.state.deleteOverlayName.empty() == true)
					{
						int prio = 0;
						for (const auto& path : m_guiState.constants.activeOverlays)
						{
							/* each button in imgui needs a unique id (title##id)
							 * since this list is generated at runtime button IDs default to ASCII 'a' + prio
							 * this way the first list button will be ' - ##a', the second ' - ##b' and so on.
							 */
							char btnId[] = { ' ', '-', ' ', '#', '#', static_cast<char>(prio + 'a') };
							imgui->TableNextColumn();
							if (imgui->Button(btnId))
							{
								m_guiState.state.deleteOverlayName = path;
							}

							imgui->TableNextColumn();
							imgui->Text("%02d", prio++);

							imgui->TableNextColumn();
							imgui->Text(path.c_str());
						}
					}
					imgui->EndTable();
				}
			}
			imgui->EndChild();
			imgui->Separator();

			imgui->Text("available overlays:");
			imgui->BeginChild("available_overlays", ImVec2(0, 200));
			{
				if (imgui->BeginTable("available_overlays_table", 2, ImGuiTableFlags_NoSavedSettings))
				{
					imgui->TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 0);
					imgui->TableSetupColumn("Overlay", ImGuiTableColumnFlags_WidthStretch, 1);
					imgui->TableHeadersRow();

					if (m_guiState.state.addOverlayName.empty() == true)
					{
						int n = 0;
						const auto l = m_guiState.constants.activeOverlays;
						for (const auto& path : m_guiState.constants.allOverlays)
						{
							if (std::find(l.begin(), l.end(), path) == l.end())
							{
								// n is the index of the overlay in the list of all overlays which should be constant
								char btnId[] = { ' ', '+', ' ', '#', '#', static_cast<char>(n + 'a') };

								imgui->TableNextColumn();
								if (imgui->Button(btnId))
								{
									m_guiState.state.addOverlayName = path;
								}

								imgui->TableNextColumn();
								imgui->Text(path.c_str());
							}
							++n;
						}
					}
					imgui->EndTable();
				}
			}
			imgui->EndChild();

			imgui->Separator();
			imgui->TextDisabled("Adding or removing overlays at runtime can cause all kinds of unexpected behaviour.");
			imgui->TextDisabled("You have been warned");
		}

		void UserInterface::RenderAdvancedConfigUI(IGuiManager* const imgui)
		{
			imgui->Checkbox("redirect Ashita ResourceManager", &m_guiState.flags.redirectFOpenS);

			imgui->NewLine();
			imgui->TextDisabled("This flag causes pivot to try and redirect Ashita's access to DAT files.");
			imgui->TextDisabled("It is mainly targeted at private servers that replace spell and abitily DATs.");
			imgui->TextDisabled("You likely don't want to mess with this.");
			imgui->NewLine();
			imgui->TextDisabled("Changes require a restart of your client to take effect.");
		}
	}
}

