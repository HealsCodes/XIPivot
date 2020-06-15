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

#pragma once

#include <string>
#include <vector>

namespace XiPivot
{
	namespace Plugin
	{
		class PolRemoteInterface
		{
			public:
				struct CacheStatus
				{
					size_t   used;
					size_t   allocation;

					unsigned cacheHits;
					unsigned cacheMisses;

					unsigned activeObjects;
				};

				typedef PolRemoteInterface* (__stdcall* pFnXiPivotPolRemote)(void);

				virtual bool AddOverlay(const std::string& path);
				virtual bool RemoveOverlay(const std::string& path);
	
				virtual bool GetDebugLogState(void) const;
				virtual std::string GetRootPath(void) const;
				virtual const std::vector<std::string> GetOverlays(void) const;

				virtual void SetDebugLogState(bool state);

				virtual bool GetCacheState(void) const;
				virtual struct CacheStatus GetCacheStats(void) const;
				virtual uint32_t GetCacheSize(void) const;
				virtual uint32_t GetCachePurgeDelay(void) const;
	
				virtual void PurgeCacheObjects(time_t maxAge);
				virtual void SetCacheParams(bool state, uint32_t size, uint32_t maxAge);

#ifdef POLPLUGIN
			public:
				/* access or create the actual Redirector instance */
				static PolRemoteInterface& instance(void);

			protected:
				/* globally unique instance pointer */
				static PolRemoteInterface* s_instance;
#endif
		};
	}
}
