Message{
	name = "take from world",
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
		-- Find the object.
		local object = Main.objects:find_by_id(id)
		if not object then return end
		if object.spec.type ~= "item" then return end
		if not player:can_reach_object(object) then return end
		-- Pick up the object.
		player.inventory:merge_object(object)
		player:animate("pick up")
	end}
