Message{
	name = "object speech",
	server_to_client_encode = function(self, id, message)
		return {"uint32", id, "string", message}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,message = packet:read("uint32", "string")
		if not ok then return end
		return {id, message}
	end,
	server_to_client_handle = function(self, id, message)
		local o = Game.objects:find_by_id(id)
		if not o then return end
		local n = o.name or o.spec.name
		Client:add_speech_text{object = o, name = o.name or o.spec.name, text = message}
	end}
