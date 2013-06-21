-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "inventory weight",
	server_to_client_encode = function(self, weight, limit)
		return {"uint16", weight, "uint16", limit}
	end,
	server_to_client_decode = function(self, packet)
		local ok,weight,limit = packet:read("uint16", "uint16")
		if not ok then return end
		return {weight, limit}
	end,
	server_to_client_handle = function(self, weight, limit)
		--TODO
	end}
