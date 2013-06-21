-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "object dialog none",
	server_to_client_encode = function(self, id)
		return {"uint32", id}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id = packet:read("uint32")
		if not ok then return end
		return {id}
	end,
	server_to_client_handle = function(self, id)
		-- Find the object.
		local obj = Main.objects:find_by_id(id)
		if not obj then return end
		-- Update the dialog.
		obj:set_dialog("none")
		if obj == Client.active_dialog_object and Ui:get_state() == "dialog" then
			Ui:pop_state()
		end
	end}
