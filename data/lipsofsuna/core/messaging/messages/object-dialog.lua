Message{
	name = "object dialog",
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
		local object = Main.objects:find_by_id(id)
		if not object then return end
		-- Enter the dialog state.
		if Ui:get_state() == "play" or Ui:get_state() == "world/object" then
			Client.active_dialog_object = object
			Ui:set_state("dialog")
		end
	end}
