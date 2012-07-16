Message{
	name = "attack",
	client_to_server_encode = function(self, value)
		return {"bool", value}
	end,
	client_to_server_decode = function(self, packet)
		local ok,value = packet:read("bool")
		if not ok then return end
		return {value}
	end,
	client_to_server_handle = function(self, client, value)
		-- Find the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Handle auto-attack.
		if value then
			player.auto_attack = true
		else
			player.auto_attack = nil
		end
		-- Handle attack start and stop.
		if value and player.attack_charge then return end
		if not value and not player.attack_charge then return end
		if value then
			player:attack_charge_start()
		else
			player:attack_charge_end()
		end
	end}
