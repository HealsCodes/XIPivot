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

#include "PolRemoteInterface.h"

#include "AshitaInterface.h"
#include "Redirector.h"
#include "MemCache.h"

namespace XiPivot
{
	namespace Plugin
	{
		/* static member initialisation */
		PolRemoteInterface* PolRemoteInterface::s_instance = nullptr;

		/* static interface */
		PolRemoteInterface& PolRemoteInterface::instance(void)
		{
			if (PolRemoteInterface::s_instance == nullptr)
			{
				PolRemoteInterface::s_instance = new PolRemoteInterface();
			}
			return *PolRemoteInterface::s_instance;
		}

		bool PolRemoteInterface::AddOverlay(const std::string& path)
		{
			auto instance = static_cast<Pol::AshitaInterface*>(&Core::Redirector::instance());
			if (instance != nullptr)
			{
				if (instance->addOverlay(path) == true)
				{
					instance->m_settings.overlays = instance->overlayList();
					instance->m_settings.save(instance->m_AshitaCore->GetConfigurationManager());
					return true;
				}
			}
			return false;
		}

		bool PolRemoteInterface::RemoveOverlay(const std::string& path)
		{
			auto instance = static_cast<Pol::AshitaInterface*>(&Core::Redirector::instance());
			if (instance != nullptr)
			{
				instance->removeOverlay(path);
				instance->m_settings.overlays = instance->overlayList();
				instance->m_settings.save(instance->m_AshitaCore->GetConfigurationManager());

				return true;
			}
			return false;
		}
		
		bool PolRemoteInterface::GetDebugLogState(void) const
		{
			auto instance = static_cast<Pol::AshitaInterface*>(&Core::Redirector::instance());
			if (instance != nullptr)
			{
				return instance->m_settings.debugLog;
			}
			return false;
		}

		std::string PolRemoteInterface::GetRootPath(void) const
		{
			auto instance = static_cast<Pol::AshitaInterface*>(&Core::Redirector::instance());
			if (instance != nullptr)
			{
				return instance->rootPath();
			}
			return "invalid";
		}

		const std::vector<std::string> PolRemoteInterface::GetOverlays(void) const
		{
			auto instance = static_cast<Pol::AshitaInterface*>(&Core::Redirector::instance());
			if (instance != nullptr)
			{
				return instance->overlayList();
			}
			return {};
		}

		void PolRemoteInterface::SetDebugLogState(bool state)
		{
			auto instance = static_cast<Pol::AshitaInterface*>(&Core::Redirector::instance());

			if (instance != nullptr)
			{
				instance->m_settings.debugLog = state;
				instance->setDebugLog(state);

				if (instance->m_settings.cacheEnabled == true)
				{
					Core::MemCache::instance().setDebugLog(state);
				}

				instance->m_settings.save(instance->m_AshitaCore->GetConfigurationManager());
			}
		}

		bool PolRemoteInterface::GetCacheState(void) const
		{
			auto instance = static_cast<Pol::AshitaInterface*>(&Core::Redirector::instance());

			if (instance != nullptr)
			{
				return instance->m_settings.cacheEnabled;
			}
			return false;
		}

		struct PolRemoteInterface::CacheStatus PolRemoteInterface::GetCacheStats(void) const
		{
			auto memCacheStats = Core::MemCache::instance().getCacheStats();
			CacheStatus res;

			res.activeObjects = memCacheStats.activeObjects;
			res.allocation = memCacheStats.allocation;
			res.cacheHits = memCacheStats.cacheHits;
			res.cacheMisses = memCacheStats.cacheMisses;
			res.used = memCacheStats.used;

			return res;
		}

		uint32_t PolRemoteInterface::GetCacheSize(void) const
		{
			auto instance = static_cast<Pol::AshitaInterface*>(&Core::Redirector::instance());

			if (instance != nullptr)
			{
				return instance->m_settings.cacheSize;
			}
			return false;
		}

		uint32_t PolRemoteInterface::GetCachePurgeDelay(void) const
		{
			auto instance = static_cast<Pol::AshitaInterface*>(&Core::Redirector::instance());
			if (instance != nullptr)
			{
				return instance->m_settings.cachePurgeDelay;
			}
			return 0;
		}
		
		void PolRemoteInterface::PurgeCacheObjects(time_t maxAge)
		{
			Core::MemCache::instance().purgeCacheObjects(maxAge);
		}

		void PolRemoteInterface::SetCacheParams(bool state, uint32_t size, uint32_t maxAge)
		{
			auto instance = static_cast<Pol::AshitaInterface*>(&Core::Redirector::instance());

			if (instance != nullptr)
			{
				instance->m_settings.cacheEnabled = state;
				instance->m_settings.cacheSize = size;
				instance->m_settings.cachePurgeDelay = maxAge;

				Core::MemCache::instance().setCacheAllocation(instance->m_settings.cacheSize);
				if (instance->m_settings.cacheEnabled == true && Core::MemCache::instance().hooksActive() == false)
				{
					instance->m_settings.cacheEnabled = Core::MemCache::instance().setupHooks();
				}
				else if (instance->m_settings.cacheEnabled == false && Core::MemCache::instance().hooksActive() == true)
				{
					Core::MemCache::instance().releaseHooks();
				}

				instance->m_settings.save(instance->m_AshitaCore->GetConfigurationManager());
			}
		}
	}
}

