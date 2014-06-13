-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "object effect",
	server_to_client_encode = function(self, id, name)
		if not name then return end
		return {"uint32", id, "string", name}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,name = packet:read("uint32", "string")
		return {id, name}
	end,
	server_to_client_handle = function(self, id, name)
		local object = Main.objects:find_by_id(id)
		if not object then return end
		Main.effect_manager:play_object(name, object)
	end}
