Message{
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
		local o = Game.objects:find_by_id(id)
		if not o then return end
		-- Update the stats.
		o:set_stat(name, value, max, diff)
	end}
