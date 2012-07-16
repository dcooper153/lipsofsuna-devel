Message{
	name = "object animated",
	server_to_client_encode = function(self, id, name, time)
		return {"uint32", id, "string", name, "float", time}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,name,time = packet:read("uint32", "string", "float")
		if not ok then return end
		return {id, name, time}
	end,
	server_to_client_handle = function(self, id, name, time)
		-- Get the object.
		local o = Object:find{id = id}
		if not o then return end
		-- Set the client side animation.
		o.render:add_animation(name, time)
	end}
