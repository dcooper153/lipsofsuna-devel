Message{
	name = "unequip",
	client_to_server_encode = function(self, index)
		return {"uint32", index}
	end,
	client_to_server_decode = function(self, packet)
		local ok,index = packet:read("uint32")
		if not ok then return end
		return {index}
	end,
	client_to_server_handle = function(self, client, index)
		-- Find the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Get the item and validate the equip.
		local object = player.inventory:get_object_by_index(index)
		if not object then return end
		-- Equip the item in the slot.
		player.inventory:unequip_index(index)
	end}
