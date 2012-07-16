Message{
	name = "climb",
	client_to_server_encode = function(self)
		return {}
	end,
	client_to_server_decode = function(self, packet)
		return {}
	end,
	client_to_server_handle = function(self, client)
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		player:climb()
	end}
