-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "update terrain",
	server_to_client_encode = function(self, packet)
		return packet
	end,
	server_to_client_decode = function(self, packet)
		return {packet}
	end,
	server_to_client_handle = function(self, packet)
		local ok,x,z = packet:read("uint32", "uint32")
		if not ok then return end
		Client.terrain_sync:add_chunk(x, z, packet)
	end}
