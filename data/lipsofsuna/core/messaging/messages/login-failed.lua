-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "login failed",
	server_to_client_encode = function(self, message)
		return {"string", message}
	end,
	server_to_client_decode = function(self, packet)
		local ok,message = packet:read("string")
		if not ok then return end
		return {message}
	end,
	server_to_client_handle = function(self, message)
		if Ui:get_state() == "join/connect" then
			Main.join:login_failed(message)
		else
			Ui:set_state("start-game")
			Client.data.connection.text = "Authentication failed: " .. message
			Ui:restart_state()
		end
	end}
