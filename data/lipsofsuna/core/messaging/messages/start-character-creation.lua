-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "start character creation",
	server_to_client_encode = function(self)
		return {}
	end,
	server_to_client_decode = function(self, packet)
		return {}
	end,
	server_to_client_handle = function(self)
		Client:set_player_object(nil)
		Client.chargen:init()
		Ui:set_state("chargen")
	end}
