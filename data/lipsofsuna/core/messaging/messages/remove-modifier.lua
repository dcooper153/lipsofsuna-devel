Message{
	name = "remove modifier",
	server_to_client_encode = function(self, name)
		return {"string", name}
	end,
	server_to_client_decode = function(self, packet)
		local ok,name = packet:read("string")
		if not ok then return end
		return {name}
	end,
	server_to_client_handle = function(self, name)
		Client.data.modifiers[name] = nil
		local hud = Ui:get_hud("modifier")
		if hud then hud.widget:remove(name) end
	end}
