-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "attack",
	client_to_server_encode = function(self, value)
		return {"bool", value}
	end,
	client_to_server_decode = function(self, packet)
		local ok,value = packet:read("bool")
		if not ok then return end
		return {value}
	end,
	client_to_server_handle = function(self, client, value)
		-- Find the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Perform the action.
		if value then
			player:action("attack")
		else
			-- FIXME: Ugly
			local action =
				player:find_action_by_name("attack") or
				player:find_action_by_name("throw")
			if action then
				action.finish = true
			end
		end
	end}
