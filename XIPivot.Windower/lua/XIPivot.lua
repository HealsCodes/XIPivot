--[[
 	Copyright © 2019, Renee Koecher
 	All rights reserved.
 
 	Redistribution and use in source and binary forms, with or without
 	modification, are permitted provided that the following conditions are met :
 
 	* Redistributions of source code must retain the above copyright
 	  notice, this list of conditions and the following disclaimer.
 	* Redistributions in binary form must reproduce the above copyright
 	  notice, this list of conditions and the following disclaimer in the
 	  documentation and/or other materials provided with the distribution.
 	* Neither the name of XIPivot nor the
 	  names of its contributors may be used to endorse or promote products
 	  derived from this software without specific prior written permission.
 
 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 	DISCLAIMED.IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
]]

_addon.name = 'XIPivot'
_addon.author = 'Heals'
_addon.version = '0.1.1'
_addon.command = 'pivot'

config = require('config')
require('pack')
require('lists')

-- try to figure out wherever we are..
addon_dir = string.match(debug.getinfo(1,"S").source, "^@(.+/)[%a%-%d_]+%.lua$")

defaults = T{}
defaults.enabled   = true
defaults.root_path = addon_dir .. 'data/DATs'
defaults.overlays  = L{}

settings = config.load(defaults)
config.save(settings, 'all')

if settings.enabled == true then

	package.cpath = package.cpath .. ';' .. addon_dir .. '/libs/?.dll'
	require("_XIPivot")
end

windower.register_event('load', function()
	if settings.enabled then
		print('XIPivot: initialising..')
		_XIPivot.set_root_path(settings.root_path)
		for i,path in ipairs(settings.overlays) do
			if _XIPivot.add_overlay(path) == true then
				print('  registered overlay[' .. i .. '] - "' .. path .. '"')
			else
				print('  failed to register overlay "' .. path .. '"')
			end
		end

		print('XIPivot: enabling overlays..')
		_XIPivot.enable()
	end
end)

windower.register_event('unload', function()
	if settings.enabled then
		print('XIPivot: disabling overlays..')
		_XIPivot.disable()
	end
end)

windower.register_event('addon command', function(command, ...)
	command = command and command:lower() or 'help'
	local args = L{...}

	if settings.enabled == false then
		print(_addon.name .. ' v.' .. _addon.version)
		print('   loaded, but disabled -- check your settings.xml')
		return
	end

	if command == 'help' or command == 'h' then
		print(_addon.name .. ' v.' .. _addon.version)
		print('   \\cs(51, 153, 255)a\\cs(153, 204, 255)dd overlay_dir\\cr - Adds a path to be searched for DAT overlays')
		print('   \\cs(51, 153, 255)r\\cs(153, 204, 255)emove overlay_dir\\cr - Removes a path from the DAT overlays')
		print('   \\cs(51, 153, 255)s\\cs(153, 204, 255)tatus\\cr - Print status and diagnostic info')
		print(' ')
		print('   \\cs(51, 153, 255)Adding or removing overlays at runtime can cause all kinds unexpected behaviour.\\cr')
		print('   \\cs(51, 153, 255)It is recommended to use settings.xml instead - you have been warned.\\cr')

	elseif command == 'add' or command == 'a' then
		if not args[1] then
			error('Invalid syntax: //pivot add <relative overlay path>')
			return
		end

	  	print('adding "' .. args[1] .. '" to overlays..')
		if _XIPivot.add_overlay(args[1]) == true then
			list.append(settings.overlays, args[1])
			config.save(settings, 'all')
		else
			print('failed to add "' .. args[1] .. '"')
		end
	elseif command == 'remove' or command == 'r' then
		if not args[1] then
			error('Invalid syntax: //pivot remove <relative overlay path>')
			return
		end

		_XIPivot.remove_overlay(args[1])
		for i,path in ipairs(settings.overlays) do
			if path == args[1] then
	  			print('removing "' .. args[1] .. '" from overlays')
				list.remove(settings.overlays, i)
				break
			end
		end
		config.save(settings, 'all')

	elseif command == 'status' or command == 's' then
		local stats = _XIPivot.diagnostics()
		print('- diagnostics')
		if stats['enabled'] then
			print('-  enabled  : true')
		else
			print('-  enabled  : false')
		end
		print('-  root_path: "' .. stats['root_path'] .. '"')
		print('-  overlays :')
		for prio, path in ipairs(stats['overlays']) do
			print('-      [' .. prio .. ']: ' .. path)
		end
	end
end)

