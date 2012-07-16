Message{
	name = "change privilege level",
	server_to_client_encode = function(self, value)
		return {"bool", value}
	end,
	server_to_client_decode = function(self, packet)
		local ok,value = packet:read("bool")
		if not ok then return end
		return {value}
	end,
	server_to_client_handle = function(self, value)
		Client.data.admin.enabled = value
	end}
