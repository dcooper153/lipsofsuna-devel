-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "object dead",
	server_to_client_encode = function(self, id, value)
		return {"uint32", id, "bool", value}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,value = packet:read("uint32", "bool")
		if not ok then return end
		return {id, value}
	end,
	server_to_client_handle = function(self, id, value)
		-- Get the object.
		local obj = Main.objects:find_by_id(id)
		if not obj then return end
		-- Update death status.
		if obj.dead == value then return end
		obj.dead = value
		if obj == Client.player_object then
			Client:set_player_dead(value)
		end
	end}
