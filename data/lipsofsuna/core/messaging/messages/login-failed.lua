Message{
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
		Ui.state = "start-game"
		Client.data.connection.text = "Authentication failed: " .. message
		Ui:restart_state()
	end}
