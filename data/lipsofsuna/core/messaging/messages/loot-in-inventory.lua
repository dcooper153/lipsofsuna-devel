Message{
	name = "loot in inventory",
	client_to_server_encode = function(self, id, index)
		return {"uint32", id, "uint32", index}
	end,
	client_to_server_decode = function(self, packet)
		local ok,id,index = packet:read("uint32", "uint32")
		if not ok then return end
		return {id, index}
	end,
	client_to_server_handle = function(self, client, id, index)
		-- Get the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Get the object.
		local parent = SimulationObject:find{id = id}
		if not parent.inventory:is_subscribed(player) then return end
		if not player:can_reach_object(parent) then return end
		local object = parent.inventory:get_object_by_index(index)
		if not object then return end
		-- Loot the object.
		object:loot(player)
	end}
