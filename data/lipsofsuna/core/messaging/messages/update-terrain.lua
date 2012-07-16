Message{
	name = "update terrain",
	server_to_client_encode = function(self, packet)
		return packet
	end,
	server_to_client_decode = function(self, packet)
		return {packet}
	end,
	server_to_client_handle = function(self, packet)
		if Server.initialized then return end
		local ok,x,y,z = packet:read("uint32", "uint32", "uint32")
		if not ok then return end
		Client.terrain_sync:add_block(x, y, z, packet)
	end}
