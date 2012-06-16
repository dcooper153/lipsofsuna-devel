require "system/debug"
require "common/chatcommand"

-- Dump debug information.
ChatCommand{pattern = "^/debug_dump$", func = function(matches)
	Program:debug_dump()
end}

-- Dump debug information.
ChatCommand{pattern = "^/debug_dump_class (.*)$", func = function(matches)
	Debug:dump_paths_by_class_instance(matches[1])
end}

-- Restarts the client.
ChatCommand{pattern = "^/client_restart$", func = function(matches)
	Program:launch_mod{name = "lipsofsuna"}
	Program.quit = true
end}

-- Show client stats.
ChatCommand{pattern = "^/client_stats$", func = function(matches)
	Operators.stats:update_client_stats(true)
	Ui.state = "admin/client-stats"
end}

-- Show player position.
ChatCommand{pattern = "^/position$", func = function(matches)
	local object = Client.player_object
	if not object then return end
	local pos = object.position
	local str = string.format("Position: %.2f %.2f %.2f", pos.x, pos.y, pos.z)
	Client:append_log(str)
	print(str)
end}

-- Show server stats.
ChatCommand{pattern = "^/server_stats$", func = function(matches)
	-- Request stats from the server.
	Network:send{packet = Packet(packets.ADMIN_STATS)}
end}

-- Lua string execution.
ChatCommand{pattern = "^/lua (.*)$", func = function(matches)
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

-- Normal chat.
ChatCommand{pattern = ".*", func = function(matches)
	Network:send{packet = Packet(packets.PLAYER_CHAT, "string", matches[1])}
end}
