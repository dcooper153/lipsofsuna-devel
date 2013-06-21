-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "trading end",
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
		-- Cancel trading.
		Server.trading:cancel(player)
	end,
	server_to_client_encode = function(self)
		return {}
	end,
	server_to_client_decode = function(self, packet)
		return {}
	end,
	server_to_client_handle = function(self)
		-- Update the user interface.
		if Ui:get_state() == "trading" then
			Ui:pop_state()
		end
	end}
