Message{
	name = "block",
	client_to_server_encode = function(self, value)
		return {"bool", value}
	end,
	client_to_server_decode = function(self, packet)
		local ok,value = packet:read("bool")
		if not ok then return end
		return {value}
	end,
	client_to_server_handle = function(self, client, value)
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		if value then
			player:attack_charge_start(true)
		else
			player:attack_charge_end(true)
		end
	end}
