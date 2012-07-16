Message{
	name = "object dialog say",
	server_to_client_encode = function(self, id, mine, char, text)
		return {"uint32", id, "bool", mine, "string", char, "string", text}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,mine,char,text = packet:read("uint32", "bool", "string", "string")
		if not ok then return end
		return {id, mine, char, text}
	end,
	server_to_client_handle = function(self, id, mine, char, text)
		-- Find the object.
		local obj = Object:find{id = id}
		if not obj then return end
		-- Update the dialog.
		obj:set_dialog("message", {character = char, message = text})
		if init and (Ui.state == "play" or Ui.state == "world/object") then
			Client.active_dialog_object = obj
			Ui.state = "dialog"
		end
	end}
