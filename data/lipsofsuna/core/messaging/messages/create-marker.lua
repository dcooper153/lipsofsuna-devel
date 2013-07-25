-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "create marker",
	server_to_client_encode = function(self, name, point)
		return {"string", name, "float", point.x, "float", point.y, "float", point.z}
	end,
	server_to_client_decode = function(self, packet)
		local ok,name,x,y,z = packet:read("string", "float", "float", "float")
		if not ok then return end
		return {name, Vector(x,y,z)}
	end,
	server_to_client_handle = function(self, name, point)
		local m = Main.markers:find_by_name(name)
		if not m then
			m = Main.markers:create(name)
			if not Operators.play:is_startup_period() then
				Client:append_log("Location added: " .. name)
			end
		end
		m.position = point:copy()
	end}
