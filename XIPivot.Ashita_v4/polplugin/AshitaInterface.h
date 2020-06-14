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
#include "Redirector.h"

namespace XiPivot
{
	namespace Pol
	{
		class AshitaInterface : public IPolPlugin, public Core::ILogProvider, private Core::Redirector
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

			/* ILogProvider */
			void logMessage(Core::ILogProvider::LogLevel level, std::string msg);
			void logMessageF(Core::ILogProvider::LogLevel level, std::string msg, ...);

		private:

			struct Settings
			{
				Settings();

				bool load(IConfigurationManager* config);
				void save(IConfigurationManager* config);

				bool debugLog;
				std::string rootPath;
				std::vector<std::string> overlays;

				bool cacheEnabled;
				uint32_t cacheSize;
				uint32_t cachePurgeDelay;
			};

			std::string m_pluginArgs;
			Settings    m_settings;

			time_t m_nextCachePurge;
		};
	}
}

