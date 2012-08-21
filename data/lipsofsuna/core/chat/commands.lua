local Actor = require("core/objects/actor")
local Debug = require( "system/debug")
local Item = require("core/objects/item")
local Obstacle = require("core/objects/obstacle")
local Physics = require("system/physics")
local Staticobject = require("core/objects/static")

ChatCommand{
	name = "client_restart",
	description = "Restart the client.",
	pattern = "^/client_restart$",
	handler = "client",
	func = function(player, matches)
		Program:launch_mod{name = "lipsofsuna"}
		Program:set_quit(true)
	end}

ChatCommand{
	name = "client_stats",
	description = "Show client stats.",
	pattern = "^/client_stats$",
	handler = "client",
	func = function(player, matches)
		Operators.stats:update_client_stats(true)
		Ui:set_state("admin/client-stats")
	end}

ChatCommand{
	name = "debug_dump",
	description = "Dump debug information.",
	pattern = "^/debug_dump$",
	handler = "client",
	func = function(player, matches)
		Program:debug_dump()
	end}

ChatCommand{
	name = "debug_dump_class",
	description = "Dump debug information on a specific class.",
	pattern = "^/debug_dump_class (.*)$",
	handler = "client",
	func = function(player, matches)
		Debug:dump_paths_by_class_instance(matches[1])
	end}

ChatCommand{
	name = "decay_objects",
	description = "Causes unimporant objects to decay immediately.",
	pattern = "^/decay_objects$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		Server.object_database:decay_world_now()
	end}

ChatCommand{
	name = "god",
	description = "Toggle the god mode.",
	pattern = "^/god$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		if player.god then
			player:send_message("/god mode off.")
			player.god = nil
		else
			player:send_message("/god mode on.")
			player.god = true
		end
	end}

ChatCommand{
	name = "grant_admin",
	description = "Grant admin privileges to a given account.",
	pattern = "^/grant_admin ([a-zA-Z0-9]*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		if not Server.config.admins[matches] then
			Server.config.admins[matches] = true
			Server.config:save()
			player:send_message("Admin privileges have been granted to " .. matches)
			local affected = Server.accounts_by_name[matches]
			if affected and affected.client then
				Game.messaging:server_event("change privilege level", affected.client, true)
			end
		else
			player:send_message("Admin privileges have already been granted to " .. matches)
		end
	end}

ChatCommand{
	name = "lua",
	description = "Execute a Lua command.",
	pattern = "^/lua (.*)$",
	handler = "client",
	func = function(player, matches)
		-- Request stats from the server.
		local func,err = loadstring(matches[1])
		if err then
			Client:append_log(err)
		else
			xpcall(func, function(err)
				print(debug.traceback("ERROR: " .. err))
			end)
		end
	end}

ChatCommand{
	name = "noclip",
	description = "Toggle the no clip mode.",
	pattern = "^/noclip$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		if player.noclip then
			player:send_message("/noclip mode off.")
			player.noclip = nil
			player.flying = player.spec.flying or false
			player:set_collision_mask(0xFFFF)
			player:set_collision_group(Physics.GROUP_PLAYER)
			player:set_gravity(player.spec.gravity)
			player:set_gravity_liquid(player.spec.water_gravity)
		else
			player:send_message("/noclip mode on.")
			player.noclip = true
			player.flying = true
			player:set_collision_mask(0)
			player:set_collision_group(0)
			player:set_gravity(Vector())
			player:set_gravity_liquid(Vector())
		end
	end}

ChatCommand{
	name = "position",
	description = "Show the coordinates of the player.",
	pattern = "^/position$",
	handler = "client",
	func = function(player, matches)
		local object = Client.player_object
		if not object then return end
		local pos = object:get_position()
		local str = string.format("Position: %.2f %.2f %.2f", pos.x, pos.y, pos.z)
		Client:append_log(str)
		print(str)
	end}

ChatCommand{
	name = "revoke_admin",
	description = "Revoke admin privileges from the given account.",
	pattern = "^/revoke_admin ([a-zA-Z0-9]*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		if Server.config.admins[matches] then
			Server.config.admins[matches] = nil
			Server.config:save()
			player:send_message("Admin privileges have been revoked from " .. matches)
			local affected = Server.accounts_by_name[matches]
			if affected and affected.client then
				Game.messaging:server_event("change privilege level", affected.client, false)
			end
		else
			player:send_message("Admin privileges have already been revoked from " .. matches)
		end
	end}

ChatCommand{
	name = "save",
	description = "Saves the game state to the database.",
	pattern = "^/save$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		Server.serialize:save()
	end}

ChatCommand{
	name = "shutdown",
	description = "Saves the game state and shuts down the server.",
	pattern = "^/shutdown$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		Server.serialize:save()
		Program:shutdown()
	end}

ChatCommand{
	name = "server_stats",
	description = "Show server stats.",
	pattern = "^/server_stats$",
	handler = "client",
	func = function(player, matches)
		-- Request stats from the server.
		Game.messaging:client_event("server stats")
	end}

ChatCommand{
	name = "spawn",
	description = "Spawn an object of any type.",
	pattern = "^/spawn (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local spec1 = Actorspec:find{name = matches[1]}
		local spec2 = Itemspec:find{name = matches[1]}
		local spec3 = Obstaclespec:find{name = matches[1]}
		local spec4 = Staticspec:find{name = matches[1]}
		if spec1 then
			Actor{
				spec = spec1,
				position = player:get_position(),
				random = true,
				realized = true}
		elseif spec2 then
			Item{
				spec = spec2,
				position = player:get_position(),
				random = true,
				realized = true}
		elseif spec3 then
			Obstacle{
				spec = spec3,
				position = player:get_position(),
				realized = true}
		elseif spec4 then
			Staticobject{
				spec = spec4,
				position = player:get_position(),
				realized = true}
		end
	end}

ChatCommand{
	name = "spawn_actor",
	description = "Spawn an actor",
	pattern = "^/spawn_actor (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local spec = Actorspec:find{name = matches[1]}
		if not spec then return end
		Actor{
			spec = spec,
			position = player:get_position(),
			random = true,
			realized = true}
	end}

ChatCommand{
	name = "spawn_item",
	description = "Spawn an item.",
	pattern = "^/spawn_item (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local spec = Itemspec:find{name = matches[1]}
		if not spec then return end
		Item{
			spec = spec,
			position = player:get_position(),
			random = true,
			realized = true}
	end}

ChatCommand{
	name = "spawn_obstacle",
	description = "Spawn an obstacle.",
	pattern = "^/spawn_obstacle (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local spec = Obstaclespec:find{name = matches[1]}
		if not spec then return end
		Obstacle{
			spec = spec,
			position = player:get_position(),
			realized = true}
	end}

ChatCommand{
	name = "start_global_event",
	description = "Start a global event.",
	pattern = "^/start_global_event (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		Server.events:start_event(matches[1])
	end}

ChatCommand{
	name = "stop_global_event",
	description = "Stop a global event.",
	pattern = "^/stop_global_event (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		Server.events:stop_event(matches[1])
	end}

ChatCommand{
	name = "resurrect",
	description = "Resurrect yourself.",
	pattern = "^/resurrect$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		player:resurrect()
	end}

ChatCommand{
	name = "sucide",
	description = "Make your character commit a suicide.",
	pattern = "^/suicide$",
	permission = "player",
	handler = "server",
	func = function(player, matches)
		player:die()
	end}

ChatCommand{
	name = "teleport",
	description = "Teleport to a map marker.",
	pattern = "^/teleport (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		if player:teleport{marker = matches[1]} then
			player:send_message("/teleport: Teleported to " .. matches[1] .. ".")
		else
			player:send_message("/teleport: Map marker " .. matches[1] .. " doesn't exist.")
		end
	end}

ChatCommand{
	name = "unlock_all",
	description = "Unlock all skills, spell types or spell effects.",
	pattern = "^/unlock_all$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		repeat until not Server.unlocks:unlock_random()
	end}

ChatCommand{
	name = "unlock_marker",
	description = "Unlock a map marker.",
	pattern = "^/unlock_marker (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local m = Marker:find{name = matches[1]}
		if m then
			m:unlock()
		else
			player:send_message(string.format("No such map marker %q.", matches[1]))
		end
	end}

ChatCommand{
	name = "unlock_random",
	description = "Unlock a random skill, spell type or spell effect.",
	pattern = "^/unlock_random$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		Server.unlocks:unlock_random()
	end}

ChatCommand{
	name = "unlock_skill",
	description = "Unlock a skill.",
	pattern = "^/unlock_skill (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local spec = Skillspec:find{name = matches[1]}
		if spec then
			Server.unlocks:unlock("skill", matches[1])
		else
			player:send_message(string.format("No such skill %q.", matches[1]))
		end
	end}

ChatCommand{
	name = "unlock_spell_effect",
	description = "Unlock a spell effect.",
	pattern = "^/unlock_spell_effect (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local spec = Feateffectspec:find{name = matches[1]}
		if spec then
			Server.unlocks:unlock("spell effect", matches[1])
		else
			player:send_message(string.format("No such spell effect %q.", matches[1]))
		end
	end}

ChatCommand{
	name = "unlock_spell_type",
	description = "Unlock a spell type.",
	pattern = "^/unlock_spell_type (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local spec = Feattypespec:find{name = matches[1]}
		if spec then
			Server.unlocks:unlock("spell type", matches[1])
		else
			player:send_message(string.format("No such spell type %q.", matches[1]))
		end
	end}

-- Invalid commands.
ChatCommand{
	pattern = "^(/[^ ]*).*",
	handler = "server",
	permission = "player",
	func = function(player, matches)
		player:send_message("Unrecognized command.")
	end}

-- Normal chat.
ChatCommand{
	pattern = ".*",
	handler = "client",
	permission = "player",
	func = function(player, matches)
		Game.messaging:client_event("player chat", matches[1])
	end}
ChatCommand{
	pattern = ".*",
	handler = "server",
	permission = "player",
	func = function(player, matches)
		player:say(matches[1])
	end}
