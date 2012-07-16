Message{
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
		Effect:play_world(name, point)
	end}
