Message{
	name = "add modifier",
	server_to_client_encode = function(self, name, time)
		return {"string", name, "float", time}
	end,
	server_to_client_decode = function(self, packet)
		local ok,name,time = packet:read("string", "float")
		if not ok then return end
		return {name, time}
	end,
	server_to_client_handle = function(self, name, time)
		Client.data.modifiers[name] = time
		local hud = Ui:get_hud("modifier")
		if hud then hud.widget:add(name, time) end
	end}
