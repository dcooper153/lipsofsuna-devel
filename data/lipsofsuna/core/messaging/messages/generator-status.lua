Message{
	name = "generator status",
	server_to_client_encode = function(self, message, progress)
		return {"string", message, "float", progress}
	end,
	server_to_client_decode = function(self, packet)
		local ok,message,progress = packet:read("string", "float")
		if not ok then return end
		return {message, progress}
	end,
	server_to_client_handle = function(self, message, progress)
	local ok,s,f = packet:read("string", "float")
		-- TODO: Client:terminate_world()
		Client.data.connection = {}
		Client.data.connection.text = "Map generator: " .. message .. " (" .. math.ceil(progress * 100) .. "%)"
		Client.data.connection.active = true
		Client.data.connection.waiting = true
		Ui:set_state("start-game")
	end}
