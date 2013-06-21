-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
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
		local obj = Main.objects:find_by_id(id)
		if not obj then return end
		-- Update the dialog.
		obj:set_dialog("message", {character = char, message = text})
		if init and (Ui:get_state() == "play" or Ui:get_state() == "world/object") then
			Client.active_dialog_object = obj
			Ui:set_state("dialog")
		end
	end}
