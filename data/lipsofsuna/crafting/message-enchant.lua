-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "enchant",
	client_to_server_encode = function(self, action, modifiers)
		local data = {"string", action, "uint8", 0}
		for k,v in pairs(modifiers) do
			table.insert(data, "string")
			table.insert(data, k)
			table.insert(data, "uint32")
			table.insert(data, v)
			data[4] = data[4] + 1
		end
		return data
	end,
	client_to_server_decode = function(self, packet)
		local ok,action,count = packet:read("string", "uint8")
		if not ok then return end
		local modifiers = {}
		for i = 1,count do
			local ok,k,v = packet:resume("string", "uint32")
			if not ok then return end
			modifiers[k] = v
		end
		return {action, modifiers}
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
		Main.crafting_utils:enchant_item(weapon, action, modifiers)
		Main.messaging:server_event("message", client, "You have enchanted your " .. weapon.spec.name .. ".")
	end,
	server_to_client_encode = function(self, enable)
		return {"uint8", enable and 1 or 0}
	end,
	server_to_client_decode = function(self, packet)
		local ok,enable = packet:read("uint8")
		if not ok then return end
		return {enable == 1}
	end,
	server_to_client_handle = function(self, enable)
		if enable then
			Ui:set_state("enchant")
		else
			Ui:set_state("play")
		end
	end}
