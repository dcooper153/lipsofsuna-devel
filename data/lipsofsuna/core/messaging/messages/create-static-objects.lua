-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local Quaternion = require("system/math/quaternion")

Main.messaging:register_message{
	name = "create static objects",
	server_to_client_encode = function(self, objects)
		local data = {}
		for k,v in pairs(objects) do
			table.insert(data, "uint32")
			table.insert(data, v[1])
			table.insert(data, "string")
			table.insert(data, v[2])
			table.insert(data, "float")
			table.insert(data, v[3].x)
			table.insert(data, "float")
			table.insert(data, v[3].y)
			table.insert(data, "float")
			table.insert(data, v[3].z)
			table.insert(data, "float")
			table.insert(data, v[4].x)
			table.insert(data, "float")
			table.insert(data, v[4].y)
			table.insert(data, "float")
			table.insert(data, v[4].z)
			table.insert(data, "float")
			table.insert(data, v[4].w)
		end
		return data
	end,
	server_to_client_decode = function(self, packet)
		local objects = {}
		packet:read()
		while true do
			local ok,id,name,x,y,z,rx,ry,rz,rw = packet:resume(
				"uint32", "string",
				"float", "float", "float",
				"float", "float", "float", "float")
			if not ok then break end
			table.insert(objects, {id, name, Vector(x,y,z), Quaternion(rx,ry,rz,rw)})
		end
		return {objects}
	end,
	server_to_client_handle = function(self, objects)
		if not Server.initialized then
			-- Create the static objects.
			local Simulation = require("core/client/simulation")
			for k,v in pairs(objects) do
				local spec = Main.specs:find_by_name("StaticSpec", v[2])
				if spec then
					local o = Simulation:create_object_by_spec(spec, v[1])
					o:set_position(v[3])
					o:set_rotation(v[4])
					o:set_visible(true)
					o.render:init(o)
				end
			end
		else
			-- Initialize render objects.
			for k,v in pairs(Main.game.static_objects_by_id) do
				v.render:init(v)
			end
		end
	end}
