-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "object animated",
	server_to_client_encode = function(self, id, name, time, variant)
		return {"uint32", id, "string", name, "float", time, "uint8", variant or 0}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,name,time,variant = packet:read("uint32", "string", "float", "uint8")
		if not ok then return end
		return {id, name, time, variant}
	end,
	server_to_client_handle = function(self, id, name, time, variant)
		-- Get the object.
		local o = Main.objects:find_by_id(id)
		if not o then return end
		-- Set the client side animation.
		-- FIXME: Weapon slot assumption to hack functional charge attacks.
		local weapon = o and o.inventory:get_object_by_slot("hand.R")
		o.render:add_animation(name, time, variant, weapon)
	end}
