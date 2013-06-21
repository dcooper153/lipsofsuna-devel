-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "object speech",
	server_to_client_encode = function(self, id, message)
		return {"uint32", id, "string", message}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,message = packet:read("uint32", "string")
		if not ok then return end
		return {id, message}
	end,
	server_to_client_handle = function(self, id, message)
		local o = Main.objects:find_by_id(id)
		if not o then return end
		local n = o.name or o.spec.name
		Client:add_speech_text{object = o, name = o.name or o.spec.name, text = message}
	end}
