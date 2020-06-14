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
		class IPolRemoteInterfaceBase
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

				virtual bool AddOverlay(const std::string& path) = 0;
				virtual bool RemoveOverlay(const std::string& path) = 0;
	
				virtual bool GetDebugLogState(void) const = 0;
				virtual std::string GetRootPath(void) const = 0;
				virtual const std::vector<std::string> GetOverlays(void) const = 0;

				virtual void SetDebugLogState(bool state) = 0;

				virtual bool GetCacheState(void) const = 0;
				virtual struct CacheStatus GetCacheStats(void) const = 0;
				virtual uint32_t GetCacheSize(void) const = 0;
				virtual uint32_t GetCachePurgeDelay(void) const = 0;
	
				virtual void PurgeCacheObjects(time_t maxAge) = 0;
				virtual void SetCacheParams(bool state, uint32_t size, uint32_t maxAge) = 0;
		};

		class IPolRemoteInterface : public IPolRemoteInterfaceBase
		{
			public:

				bool AddOverlay(const std::string& path) override
				{
					return false;
				}

				bool RemoveOverlay(const std::string& path) override
				{
					return false;
				}
	
				bool GetDebugLogState(void) const override
				{
					return false;
				}

				std::string GetRootPath(void) const override
				{
					return u8"/dummy/path";
				}

				const std::vector<std::string> GetOverlays(void) const override
				{
					return {};
				}
		
				bool GetCacheState(void) const override
				{
					return false;
				}

				struct CacheStatus GetCacheStats(void) const override
				{
					return {};
				};

				uint32_t GetCacheSize(void) const override
				{
					return 0;
				}

				uint32_t GetCachePurgeDelay(void) const override
				{
					return 0;
				}
		
				void PurgeCacheObjects(time_t maxAge) override
				{
				}

				void SetCacheParams(bool state, uint32_t size, uint32_t maxAge) override
				{
				}
		};
	}
}
