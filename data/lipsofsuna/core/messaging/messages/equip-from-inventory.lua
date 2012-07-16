Message{
	name = "equip from inventory",
	client_to_server_encode = function(self, index, slot)
		return {"uint32", index, "string", slot}
	end,
	client_to_server_decode = function(self, packet)
		local ok,index,slot = packet:read("uint32", "string")
		if not ok then return end
		return {index, slot}
	end,
	client_to_server_handle = function(self, client, index, slot)
		-- Find the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Get the item and validate the equip.
		local object = player.inventory:get_object_by_index(index)
		if not object then return end
		if object.spec.equipment_slot ~= slot then return end
		-- Equip the item in the slot.
		player.inventory:equip_index(index, slot)
	end}
