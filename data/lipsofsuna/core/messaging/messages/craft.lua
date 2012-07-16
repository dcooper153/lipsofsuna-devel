Message{
	name = "craft",
	client_to_server_encode = function(self, id, name)
		return {"uint32", id, "string", name}
	end,
	client_to_server_decode = function(self, packet)
		local ok,id,name = packet:read("uint32", "string")
		if not ok then return end
		return {id, name}
	end,
	client_to_server_handle = function(self, client, id, name)
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		if player.id ~= id then return end
		local o = Crafting:craft{name = name, user = player}
		if not o then return end
		player.inventory:merge_or_drop_object(o)
	end}
