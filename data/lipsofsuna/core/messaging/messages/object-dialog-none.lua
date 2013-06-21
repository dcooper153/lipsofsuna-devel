Message{
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
