local Staticobject = require("core/objects/static")

Message{
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
		-- Create the physics objects.
		if not Server.initialized then
			for k,v in pairs(objects) do
				local spec = Staticspec:find{name = v[2]}
				if spec then
					Staticobject{id = v[1], spec = spec,
						position = v[3], rotation = v[4], realized = true}
				end
			end
		end
		-- Initialize the render objects.
		for k,v in pairs(Game.static_objects_by_id) do
			v.render:init(v)
		end
	end}
