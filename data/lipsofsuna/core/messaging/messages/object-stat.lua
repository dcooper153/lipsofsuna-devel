-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "object stat",
	server_to_client_encode = function(self, id, name, value, max, diff)
		return {"uint32", id, "string", name, "int32", value, "int32", max, "int32", diff}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,name,value,max,diff = packet:read("uint32", "string", "int32", "int32", "int32")
		if not ok then return end
		return {id, name, value, max, diff}
	end,
	server_to_client_handle = function(self, id, name, value, max, diff)
		-- Get the object.
		local o = Main.objects:find_by_id(id)
		if not o then return end
		-- Update the stats.
		o:set_stat(name, value, max, diff)
	end}
