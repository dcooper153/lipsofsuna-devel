Message{
	name = "object beheaded",
	server_to_client_encode = function(self, id)
		return {"uint32", id}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id = packet:read("uint32")
		if not ok then return end
		return {id}
	end,
	server_to_client_handle = function(self, id)
		-- Get the object.
		local object = Object:find{id = id}
		if not object then return end
		-- Remove the head.
		object.render:set_beheaded(true)
		-- Play the effect.
		Client.effects:play_object("behead1", object, "#neck")
	end}
