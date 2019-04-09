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

extern "C"
{
#	define LUA_BUILD_AS_DLL

#	include "lauxlib.h"
#	include "lua.h"
}

#include "Redirector.h"

namespace XiPivot
{
	/* a simple, mostly static interface addon-on to
	 * provide interopperability with the LUA-C API 
	 */
	class WindowerInterface : public Core::Redirector
	{
		public:
			~WindowerInterface(void) {};

			static int registerInterface(lua_State *L);

			/* interface methods */

			/* internally calls Redirector::setupHooks
			 *
			 * arguments: none
			 * returns: a boolean indicating the operation result
			 */
			static int lua_enable(lua_State *L);

			/* internally calls Redirector::removeHooks
			 *
			 * arguments: none
			 * returns: a boolean representing the operation result
			 */
			static int lua_disable(lua_State *L);

			/* internally calls Redirector::setRootPath
			 *
			 * arguments: [1] - string: rootPath
			 * returns: a string containing the new, active rootPath
			 */
			static int lua_setRootPath(lua_State *L);

			/* internally calls Redirector::addOverlayPath
			 *
			 * arguments: [1] - string: the relative overlay path
			 * returns: a bolean representing the operation result
			 */
			static int lua_addOverlayPath(lua_State *L);

			/* internally calls Redirector::removeOverlayPath
			 *
			 * arguments: [1] - string: the relative overlay path
			 * returns: a bolean representing the operation result
			 */
			static int lua_removeOverlayPath(lua_State *L);

			/* collects some of the internal data of the Redirector 
			 *
			 * arguments: none
			 * returns: a table of the following make-up
			 *  { 
			 *		"enabled": <boolean>,
			 *      "root_path": <string>,
			 *      "overlays": { <string>, <string>, ... }
			 *  }
			 */
			static int lua_getDiagnostics(lua_State *L);

		protected:
			WindowerInterface(void) : Redirector() {};
	};
}

