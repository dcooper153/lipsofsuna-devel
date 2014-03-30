-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "show skills",
	server_to_client_encode = function(self, enable)
		return {"uint8", enable and 1 or 0}
	end,
	server_to_client_decode = function(self, packet)
		local ok,enable = packet:read("uint8")
		if not ok then return end
		return {enable == 1}
	end,
	server_to_client_handle = function(self, enable)
		if enable then
			Ui:set_state("skills")
		else
			Ui:set_state("play")
		end
	end}
