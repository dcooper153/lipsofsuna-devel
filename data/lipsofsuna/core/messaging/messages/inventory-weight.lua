Message{
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
