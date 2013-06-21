-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "close inventory",
	client_to_server_encode = function(self, id)
		return {"uint32", id}
	end,
	client_to_server_decode = function(self, packet)
		local ok,id = packet:read("uint32")
		if not ok then return end
		return {id}
	end,
	client_to_server_handle = function(self, client, id)
		-- Find the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Find the inventory.
		if id == player:get_id() then return end
		local obj = Main.objects:find_by_id(id)
		if not obj then return end
		-- Unsubscribe.
		obj.inventory:unsubscribe(player)
	end,
	server_to_client_encode = function(self, id)
		return {"uint32", id}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id = packet:read("uint32")
		if not ok then return end
		return {id}
	end,
	server_to_client_handle = function(self, id)
		-- Remove the subscription.
		Operators.inventory:remove_inventory(id)
		-- Find the object.
		local object = Main.objects:find_by_id(id)
		if not object then return end
		-- Clear the inventory.
		if not object:has_server_data() then
			object.inventory:clear()
			object.inventory.size = 0
		end
	end}
