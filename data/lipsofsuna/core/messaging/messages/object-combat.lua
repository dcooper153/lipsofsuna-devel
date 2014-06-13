-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "object combat",
	server_to_client_encode = function(self, id, value)
		return {"uint32", id, "uint8", value and 0 or 1}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,value = packet:read("uint32", "uint8")
		if not ok then return end
		return {id, value == 1}
	end,
	server_to_client_handle = function(self, id, value)
		-- Get the object.
		local o = Main.objects:find_by_id(id)
		if not o then return end
		-- Set the combat hint.
		Main.music_manager:set_combat_hint(o, value)
	end}
