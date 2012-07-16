Message{
	name = "message",
	server_to_client_encode = function(self, text)
		return {"string", text}
	end,
	server_to_client_decode = function(self, packet)
		local ok,text = packet:read("string")
		if not ok then return end
		return {text}
	end,
	server_to_client_handle = function(self, text)
		Client:append_log(text)
	end}
