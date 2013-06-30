-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "disenchant",
	client_to_server_encode = function(self)
		return {}
	end,
	client_to_server_decode = function(self, packet)
		return {}
	end,
	client_to_server_handle = function(self, client, action, modifiers)
		-- Get the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Get the weapon.
		local weapon = player:get_weapon()
		if not weapon then return end
		-- TODO: Validate.
		Main.crafting_utils:disenchant_item(weapon)
		Main.messaging:server_event("message", client, "You have disenchanted your " .. weapon.spec.name .. ".")
	end}
