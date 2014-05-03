local Actor = require("core/objects/actor")
local Debug = require( "system/debug")
local Item = require("core/objects/item")
local ModifierSpec = require("core/specs/modifier")
local Obstacle = require("core/objects/obstacle")
local Physics = require("system/physics")
local PhysicsConsts = require("core/server/physics-consts")
local Staticobject = require("core/objects/static")

ChatCommand{
	name = "behead",
	description = "Beheading or unbehead yourself.",
	pattern = "^/behead$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		player:set_beheaded(not player:get_beheaded())
	end}

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
			local affected = Main.accounts:get_account_by_login(matches)
			if affected and affected.client then
				Main.messaging:server_event("change privilege level", affected.client, true)
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
			player.physics:set_collision_mask(PhysicsConsts.MASK_ALL)
			player.physics:set_collision_group(PhysicsConsts.GROUP_PLAYER)
			player.physics:set_gravity(player.spec.gravity)
			player.physics:set_gravity_liquid(player.spec.water_gravity)
		else
			player:send_message("/noclip mode on.")
			player.noclip = true
			player.flying = true
			player.physics:set_collision_mask(0)
			player.physics:set_collision_group(0)
			player.physics:set_gravity(Vector())
			player.physics:set_gravity_liquid(Vector())
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
	name = "prof",
	description = "Shows profiling data.",
	pattern = "^/prof$",
	handler = "client",
	func = function(player, matches)
		Ui:set_state("admin/profiling")
	end}

ChatCommand{
	name = "prof_reset",
	description = "Resets profiling data.",
	pattern = "^/prof_reset$",
	handler = "client",
	func = function(player, matches)
		Main.timing:reset_profiling()
		Client:append_log("Profiling reset")
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
			local affected = Main.accounts:get_account_by_login(matches)
			if affected and affected.client then
				Main.messaging:server_event("change privilege level", affected.client, false)
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
		Main.game:save()
	end}

ChatCommand{
	name = "shutdown",
	description = "Saves the game state and shuts down the server.",
	pattern = "^/shutdown$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		Main.game:save()
		Program:shutdown()
	end}

ChatCommand{
	name = "server_stats",
	description = "Show server stats.",
	pattern = "^/server_stats$",
	handler = "client",
	func = function(player, matches)
		-- Request stats from the server.
		Main.messaging:client_event("server stats")
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
			local o = Actor(player.manager)
			o:set_spec(spec1)
			o:set_position(player:get_position())
			o:randomize()
			o:set_visible(true)
		elseif spec2 then
			local o = Item(player.manager)
			o:set_spec(spec2)
			o:set_position(player:get_position())
			o:randomize()
			o:set_visible(true)
		elseif spec3 then
			local o = Obstacle(player.manager)
			o:set_spec(spec3)
			o:set_position(player:get_position())
			o:set_visible(true)
		elseif spec4 then
			local o = Staticobject(player.manager)
			o:set_spec(spec4)
			o:set_position(player:get_position())
			o:set_visible(true)
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
		local o = Actor(player.manager)
		o:set_spec(spec)
		o:set_position(player:get_position())
		o:randomize()
		o:set_visible(true)
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
		local o = Item(player.manager)
		o:set_spec(spec)
		o:set_position(player:get_position())
		o:randomize()
		o:set_visible(true)
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
		local o = Obstacle(player.manager)
		o:set_spec(spec)
		o:set_position(player:get_position())
		o:set_visible(true)
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
		player:action("resurrect")
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
	name = "unlock_marker",
	description = "Unlock a map marker.",
	pattern = "^/unlock_marker (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local m = Main.markers:find_by_name(matches[1])
		if m then
			m:unlock()
		else
			player:send_message(string.format("No such map marker %q.", matches[1]))
		end
	end}

-- Invalid commands.
ChatCommand{
	pattern = "^(/[^ ]*).*",
	fallback = true,
	handler = "server",
	permission = "player",
	func = function(player, matches)
		player:send_message("Unrecognized command.")
	end}

-- Normal chat.
ChatCommand{
	pattern = ".*",
	fallback = true,
	handler = "client",
	permission = "player",
	priority = 100,
	func = function(player, matches)
		Main.messaging:client_event("player chat", matches[1])
	end}
ChatCommand{
	pattern = ".*",
	fallback = true,
	handler = "server",
	permission = "player",
	priority = 100,
	func = function(player, matches)
		player:action("say", matches[1])
	end}
