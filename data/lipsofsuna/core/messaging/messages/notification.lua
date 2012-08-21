Message{
	name = "notification",
	server_to_client_encode = function(self, text)
		return {"string", text}
	end,
	server_to_client_decode = function(self, packet)
		local ok,text = packet:read("string")
		if not ok then return end
		return {text}
	end,
	server_to_client_handle = function(self, text)
		local hud = Ui:get_hud("notification")
		if not hud then return end
		hud.widget:set_text(text)
	end}
