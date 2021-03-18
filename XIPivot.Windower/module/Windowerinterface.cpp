/*
 * 	Copyright © 2019-2021, Renee Koecher
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

namespace XiPivot
{
	int WindowerInterface::registerInterface(lua_State *L)
	{
		struct luaL_reg api[] = {
			{ "enable"         , WindowerInterface::lua_enable },
			{ "disable"        , WindowerInterface::lua_disable },
 
			{ "set_root_path"  , WindowerInterface::lua_setRootPath },

			{ "add_overlay"    , WindowerInterface::lua_addOverlayPath },
			{ "remove_overlay" , WindowerInterface::lua_removeOverlayPath },

			{ "diagnostics"    , WindowerInterface::lua_getDiagnostics },

			{ NULL, NULL }
		};

		luaL_register(L, "_XIPivot", api);
		return 1;
	}

	int WindowerInterface::lua_enable(lua_State *L)
	{
		lua_pushboolean(L, instance().setupHooks() ? TRUE : FALSE);
		return 1;
	}

	int WindowerInterface::lua_disable(lua_State *L)
	{
		lua_pushboolean(L, instance().releaseHooks() ? TRUE : FALSE);
		return 1;
	}

	int WindowerInterface::lua_setRootPath(lua_State *L)
	{
		if (lua_gettop(L) != 1 || !lua_isstring(L, 1))
		{
			lua_pushstring(L, "a valid path argument is required");
			lua_error(L);
		}

		instance().setRootPath(lua_tostring(L, 1));

		lua_pushstring(L, instance().rootPath().c_str());
		return 1;
	}

	int WindowerInterface::lua_addOverlayPath(lua_State *L)
	{
		if (lua_gettop(L) != 1 || !lua_isstring(L, 1))
		{
			lua_pushstring(L, "a valid path argument is required");
			lua_error(L);
		}
		lua_pushboolean(L, instance().addOverlay(lua_tostring(L, 1)) ? TRUE : FALSE);
		return 1;
	}

	int WindowerInterface::lua_removeOverlayPath(lua_State *L)
	{
		if (lua_gettop(L) != 1 || !lua_isstring(L, 1))
		{
			lua_pushstring(L, "a valid path argument is required");
			lua_error(L);
		}
		instance().removeOverlay(lua_tostring(L, 1));
		return 0;
	}

	int WindowerInterface::lua_getDiagnostics(lua_State *L)
	{
		/* push a table to hold the diagnostics as a whole */
		lua_createtable(L, 1, 2);

		lua_pushboolean(L, instance().hooksActive() ? TRUE : FALSE);
		lua_setfield(L, -2, "enabled");

		lua_pushstring(L, instance().rootPath().c_str());
		lua_setfield(L, -2, "root_path");

		/* push a table to hold the current active overlays */
		lua_createtable(L, 0, instance().overlayList().size());

		int i = 0;
		for (const auto& p : instance().overlayList())
		{
			lua_pushstring(L, p.c_str());
			lua_rawseti(L, -2, ++i);
		}

		lua_setfield(L, -2, "overlays");
		return 1;
	}
}

