Message{
	name = "store to container",
	client_to_server_encode = function(self, id, index)
		return {"uint32", id, "uint32", index}
	end,
	client_to_server_decode = function(self, packet)
		local ok,id,index = packet:read("uint32", "uint32")
		if not ok then return end
		return {id, index}
	end,
	client_to_server_handle = function(self, client, id, index)
		-- Find the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Get the item and validate the store.
		local target = Main.objects:find_by_id(id)
		if not target then return end
		if not target.inventory:is_subscribed(player) then return end
		if not player:can_reach_object(target) then return end
		local object = player.inventory:get_object_by_index(index)
		if not object then return end
		-- Store the item.
		if not target.inventory:merge_object(object) then
			player:send_message("Could not store the item.")
		end
	end}
