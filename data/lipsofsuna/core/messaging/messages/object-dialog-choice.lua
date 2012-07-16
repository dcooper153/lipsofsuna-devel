Message{
	name = "object dialog choice",
	server_to_client_encode = function(self, id, mine, choices)
		local data = {"uint32", id, "bool", mine, "uint8", #args.choices}
		for k,v in ipairs(choices) do
			table.insert(data, "string")
			table.insert(data, v)
		end
		return data
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,mine,num_choices = packet:read("uint32", "bool", "uint8")
		if not ok then return end
		local choices = {}
		for i = 1,num_choices do
			local ok,m = packet:resume("string")
			if not ok then return end
			table.insert(choices, m)
		end
		return {id, mine, choices}
	end,
	server_to_client_handle = function(self, id, mine, choices)
		-- Get the object.
		local obj = Object:find{id = id}
		if not obj then return end
		-- Update the dialog.
		obj:set_dialog("choice", choices)
		if init and (Ui.state == "play" or Ui.state == "world/object") then
			Client.active_dialog_object = obj
			Ui.state = "dialog"
		end
	end}
