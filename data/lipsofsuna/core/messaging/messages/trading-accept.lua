Message{
	name = "trading accept",
	client_to_server_encode = function(self)
		return {}
	end,
	client_to_server_decode = function(self, packet)
		return {}
	end,
	client_to_server_handle = function(self, client)
		-- Find the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		if not player.trading then return end
		-- Accept trading.
		Trading:accept(player)
	end,
	server_to_client_encode = function(self, deal)
		return {"bool", deal}
	end,
	server_to_client_decode = function(self, packet)
		local ok,deal = packet:read("bool")
		if not ok then return end
		return {deal}
	end,
	server_to_client_handle = function(self, deal)
		-- Change the shopkeeper's approval.
		Client.data.trading.accepted = deal
		-- Update the user interface.
		if Ui.state == "trading" then
			Ui:restart_state()
		end
	end}
