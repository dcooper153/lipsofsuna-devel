-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "object hidden",
	server_to_client_encode = function(self, id)
		return {"uint32", id}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id = packet:read("uint32")
		if not ok then return end
		return {id}
	end,
	server_to_client_handle = function(self, id)
		-- Get the object.
		local obj = Main.objects:find_by_id(id)
		if not obj then return end
		-- Hide the object.
		if obj:get_static() then return end
		if obj.render then
			obj.render:clear()
		end
		if not obj:has_server_data() then
			obj:detach()
		end
		-- Disable the combat hint.
		Client.music:set_combat_hint(obj, false)
	end}
