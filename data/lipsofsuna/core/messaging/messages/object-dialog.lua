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
		local object = Object:find{id = id}
		if not object then return end
		-- Enter the dialog state.
		if Ui.state == "play" or Ui.state == "world/object" then
			Client.active_dialog_object = object
			Ui.state = "dialog"
		end
	end}
