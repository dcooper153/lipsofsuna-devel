-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "world effect",
	server_to_client_encode = function(self, point, name)
		return {"string", name, "float", point.x, "float", point.y, "float", point.z}
	end,
	server_to_client_decode = function(self, packet)
		local ok,t,x,y,z = packet:read("string", "float", "float", "float")
		if not ok then return end
		return {Vector(x,y,z), t}
	end,
	server_to_client_handle = function(self, point, name)
		Main.effect_manager:play_world(name, point)
	end}
