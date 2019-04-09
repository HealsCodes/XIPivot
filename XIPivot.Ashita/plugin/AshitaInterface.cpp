/*
 * 	Copyright © 2019, Renee Koecher
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
		
		m_pluginId(0),
	    m_ashitaCore(nullptr),
	    m_logManager(nullptr),
	    m_direct3DDevice(nullptr)
	{
		/* FIXME: does this play anywhere nice with reloads?
		 * FIXME: .. I hope it does
		 */
		Redirector::s_instance = this;
	}

	plugininfo_t AshitaInterface::GetPluginInfo(void)
	{
		return *AshitaInterface::s_pluginInfo;
	}

	bool AshitaInterface::Initialize(IAshitaCore *core, ILogManager *log, uint32_t id)
	{
		m_ashitaCore = core;
		m_logManager = log;
		m_pluginId = id;
		m_config = (core ? core->GetConfigurationManager() : nullptr);

		if (m_config != nullptr)
		{
			if (m_settings.load(m_config) && m_settings.enabled)
			{
				instance().setRootPath(m_settings.rootPath);
				for (const auto &path : m_settings.overlays)
				{
					instance().addOverlay(path);
				}
			}
			m_settings.save(m_config);
		}

		return instance().setupHooks();
	}

	void AshitaInterface::Release(void)
	{
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
					chatPrintf("$cs(19)adding '$cs(9)%s$cs(19)' to overlays..$cr", args[2].c_str());
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

					chatPrintf("$cs(19)removing '$cs(9)%s$cs(19)' from overlays..$cr", args[2].c_str());
					instance().removeOverlay(args[2]);

					it = std::find(m_settings.overlays.begin(), m_settings.overlays.end(), args[2].c_str());
					if (it != m_settings.overlays.end())
					{
						m_settings.overlays.erase(it);
						m_settings.save(m_config);
					}
				}
			}
			else if (args.size() == 2 && (args[1] == "s" || args[1] == "status"))
			{
				chatPrintf("$cs(16)diagnostics:$cr");
				chatPrintf("  $cs(16)enabled   $cs(19): %s$cr", m_settings.enabled ? "$cs(13)true" : "$cs(7)false");
				chatPrintf("  $cs(16)root_path $cs(19): '$cs(9)%s$cs(9)'$cr", m_settings.rootPath.c_str());
				chatPrintf("  $cs(16)overlays  $cs(19):$cr");

				int prio = 0;
				for (const auto& path : m_settings.overlays)
				{
					chatPrintf("      $cs(16)[$cs(19)%-2d$cs(16)]$cs(19): '$cs(9)%s$cs(19)'$cr", prio++, path.c_str());
				}
			}
			else
			{
				chatPrintf("$cs(16)%s$cs(19) v.$cs(16)%.2f$cs(19) by $cs(14)%s$cr", s_pluginInfo->Name, s_pluginInfo->PluginVersion, s_pluginInfo->Author);
				if (m_settings.enabled == false)
				{
					chatPrintf("$cs(19)   loaded, but $cs(7)disabled$cs(19) -- check your XIPivot.xml$cr");
				}
				else
				{
					chatPrintf("   $cs(9)a$cs(16)dd overlay_dir $cs(19)- Adds a path to be searched for DAT overlays$cr");
					chatPrintf("   $cs(9)r$cs(16)emove overlay_dir $cs(19)- Removes a path from the DAT overlays$cr");
					chatPrintf("   $cs(9)s$cs(16)tatus $cs(19)- Print status and diagnostic info$cr");
					chatPrintf("   $cs(16)-$cr");
					chatPrintf("   $cs(19)Adding or removing overlays at runtime can cause $cs(16)all kinds unexpected behaviour.$cr");
					chatPrintf("   $cs(19)It is recommended to use XIPivot.xml instead - $cs(16)you have been warned.$cr");
				}
			}
			return true;
		}

		return false;
	}

	/* private parts below */

	AshitaInterface::Settings::Settings() : enabled(true)
	{
		char workPath[MAX_PATH];

		GetCurrentDirectoryA(MAX_PATH, static_cast<LPSTR>(workPath));

		/* default to "plugin location"/DATs */
		rootPath = std::string(workPath) + "/DATs";
		overlays.clear();
	}

	bool AshitaInterface::Settings::load(IConfigurationManager *config)
	{
		if (config->Load("XIPivot", "XIPivot"))
		{
			const char *rP = config->get_string("XIPivot", "root_path");
			const char *oL = config->get_string("XIPivot", "overlays");

			enabled = config->get_bool("XIPivot", "enabled", true);
			rootPath = (rP ? rP : "");

			overlays.clear();
			if(oL != nullptr && strlen(oL) > 0)
			{
				overlays = split(oL, ",");
			}
			return true;
		}
		return false;
	}

	void AshitaInterface::Settings::save(IConfigurationManager *config)
	{
		//config->Remove("XIPivot");
		config->set_value("XIPivot", "enabled", enabled ? "true" : "false");
		config->set_value("XIPivot", "root_path", rootPath.c_str());
		config->set_value("XIPivot", "overlays", join(overlays.begin(), overlays.end(), ",").c_str());
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
}
