-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "block",
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
		-- Store the control state.
		player.control_left = value and true or nil
		-- Perform the action.
		if value then
			local action = Main.combat_utils:get_combat_action_for_actor(player, "left")
			if action then
				player:action(action.name)
			end
		end
	end}
