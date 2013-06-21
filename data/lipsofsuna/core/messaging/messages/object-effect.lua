Message{
	name = "object effect",
	server_to_client_encode = function(self, id, name)
		if not name then return end
		return {"uint32", id, "string", name}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,name = packet:read("uint32", "string")
		return {id, name}
	end,
	server_to_client_handle = function(self, id, name)
		local object = Main.objects:find_by_id(id)
		if not object then return end
		Client.effects:play_object(name, object)
	end}
