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

#include "WindowerInterface.h"
#include "MemCache.h"

#include <ctime>
#include <cstdio>

namespace
{
	inline std::string logPrefix(XiPivot::Core::ILogProvider::LogLevel level)
	{
		switch (level)
		{
			case XiPivot::Core::ILogProvider::LogLevel::Discard: return { "" };
			case XiPivot::Core::ILogProvider::LogLevel::Debug:   return { "DEBUG:" };
			case XiPivot::Core::ILogProvider::LogLevel::Info:    return { "INFO :" };
			case XiPivot::Core::ILogProvider::LogLevel::Warn:    return { "WARN :" };
			case XiPivot::Core::ILogProvider::LogLevel::Error:   return { "ERROR:" };
		}
		return { "?????:" };
	}
}

namespace XiPivot
{
	int WindowerInterface::registerInterface(lua_State *L)
	{
		const auto self = instance<WindowerInterface>(); // force instantiation early on

		struct luaL_reg api[] = {
			{ "enable"         , WindowerInterface::lua_enable },
			{ "disable"        , WindowerInterface::lua_disable },

			{ "set_debug"      , WindowerInterface::lua_setDebug },
			{ "set_root_path"  , WindowerInterface::lua_setRootPath },

			{ "add_overlay"    , WindowerInterface::lua_addOverlayPath },
			{ "remove_overlay" , WindowerInterface::lua_removeOverlayPath },

			{ "setup_cache"    , WindowerInterface::lua_setupCache },
			{ "on_tick"        , WindowerInterface::lua_onTick },

			{ "diagnostics"    , WindowerInterface::lua_getDiagnostics },

			{ NULL, NULL }
		};

		luaL_register(L, "_XIPivot", api);
		return 1;
	}

	int WindowerInterface::lua_enable(lua_State *L)
	{
		bool res = true;
		const auto self = instance<WindowerInterface>();

		if (self->m_cacheConfig.enabled)
		{
			Core::MemCache::instance().setCacheAllocation(self->m_cacheConfig.allocation);
			res &= Core::MemCache::instance().setupHooks();
		}
		else
		{
			Core::MemCache::instance().releaseHooks();
			Core::MemCache::instance().setCacheAllocation(0);
		}

		res &= instance<WindowerInterface>()->setupHooks();

		lua_pushboolean(L, res ? TRUE : FALSE);
		return 1;
	}

	int WindowerInterface::lua_disable(lua_State *L)
	{
		bool res = true;
		const auto self = instance<WindowerInterface>();

		if (self->m_cacheConfig.enabled)
		{
			res &= Core::MemCache::instance().releaseHooks();
			Core::MemCache::instance().setCacheAllocation(0);
		}

		res &= instance<WindowerInterface>()->releaseHooks();

		lua_pushboolean(L, res ? TRUE : FALSE);
		return 1;
	}

	int WindowerInterface::lua_setDebug(lua_State* L)
	{
		if (lua_gettop(L) != 1 || !lua_isboolean(L, 1))
		{
			lua_pushstring(L, "a valid boolean argument is required");
			lua_error(L);
		}

		auto self = instance<WindowerInterface>();
		if (self->m_logOut.is_open())
		{
			self->m_logOut.flush();
			self->m_logOut.close();
		}

		if (lua_toboolean(L, 1) != 0)
		{
			self->m_logOut.open("pivot.log", std::ofstream::out | std::ofstream::app);
			if (self->m_logOut.is_open())
			{
				instance<WindowerInterface>()->setLogProvider(self);
				instance<WindowerInterface>()->setDebugLog(true);
			}
		}
		else
		{
			instance<WindowerInterface>()->setLogProvider(Core::DummyLogProvider::instance());
			instance<WindowerInterface>()->setDebugLog(false);

			if (self->m_logOut.is_open())
			{
				self->m_logOut.flush();
				self->m_logOut.close();
			}
		}

		lua_pushboolean(L, self->m_logOut.is_open() ? TRUE : FALSE);
		return 1;
	}

	int WindowerInterface::lua_setRootPath(lua_State *L)
	{
		if (lua_gettop(L) != 1 || !lua_isstring(L, 1))
		{
			lua_pushstring(L, "a valid path argument is required");
			lua_error(L);
		}

		instance<WindowerInterface>()->setRootPath(lua_tostring(L, 1));

		lua_pushstring(L, instance<WindowerInterface>()->rootPath().c_str());
		return 1;
	}

	int WindowerInterface::lua_addOverlayPath(lua_State *L)
	{
		if (lua_gettop(L) != 1 || !lua_isstring(L, 1))
		{
			lua_pushstring(L, "a valid path argument is required");
			lua_error(L);
		}
		lua_pushboolean(L, instance<WindowerInterface>()->addOverlay(lua_tostring(L, 1)) ? TRUE : FALSE);
		return 1;
	}

	int WindowerInterface::lua_removeOverlayPath(lua_State *L)
	{
		if (lua_gettop(L) != 1 || !lua_isstring(L, 1))
		{
			lua_pushstring(L, "a valid path argument is required");
			lua_error(L);
		}
		instance<WindowerInterface>()->removeOverlay(lua_tostring(L, 1));
		return 0;
	}

	int WindowerInterface::lua_getDiagnostics(lua_State *L)
	{
		/* push a table to hold the diagnostics as a whole */
		lua_createtable(L, 1, 2);

		lua_pushboolean(L, instance<WindowerInterface>()->hooksActive() ? TRUE : FALSE);
		lua_setfield(L, -2, "enabled");

		lua_pushstring(L, instance<WindowerInterface>()->rootPath().c_str());
		lua_setfield(L, -2, "root_path");

		/* push a table to hold the current active overlays */
		lua_createtable(L, 0, instance<WindowerInterface>()->overlayList().size());

		int i = 0;
		for (const auto& p : instance<WindowerInterface>()->overlayList())
		{
			lua_pushstring(L, p.c_str());
			lua_rawseti(L, -2, ++i);
		}

		lua_setfield(L, -2, "overlays");
		return 1;
	}

	int WindowerInterface::lua_setupCache(lua_State* L)
	{
		if (lua_gettop(L) != 3 || !lua_isboolean(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			lua_pushstring(L, "invalid arguments, expected `bool`,`number`,`number`");
			lua_error(L);
		}
		auto self = instance<WindowerInterface>();
		self->m_cacheConfig.enabled = lua_toboolean(L, 1) == TRUE;
		self->m_cacheConfig.allocation = lua_tointeger(L, 2);
		self->m_cacheConfig.maxAge = lua_tointeger(L, 3);

		return 0;
	}

	int WindowerInterface::lua_onTick(lua_State* L)
	{
		auto self = instance<WindowerInterface>();

		if (self->m_cacheConfig.enabled)
		{
			time_t now = time(nullptr);
			if (now > self->m_cacheConfig.nextPurge)
			{
				self->m_cacheConfig.nextPurge = now + self->m_cacheConfig.maxAge;
				Core::MemCache::instance().purgeCacheObjects(self->m_cacheConfig.maxAge);
			}
		}

		return 0;
	}

	void WindowerInterface::logMessage(LogLevel level, std::string message)
	{
		if (level == LogLevel::Discard || m_logOut.is_open() == false)
		{
			return;
		}
		m_logOut << logPrefix(level) << " " << message << std::endl;
	}

	void WindowerInterface::logMessageF(LogLevel level, std::string fmt, ...)
	{
		if (level == LogLevel::Discard || m_logOut.is_open() == false)
		{
			return;
		}

		char msgBuf[512];
		va_list args;
		__crt_va_start(args, fmt);
		vsnprintf_s(msgBuf, sizeof(msgBuf), fmt.c_str(), args);
		__crt_va_end(args);

		m_logOut << logPrefix(level) << " " << msgBuf << std::endl;
	}
}

