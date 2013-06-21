Message{
	name = "object beheaded",
	server_to_client_encode = function(self, id, beheaded)
		return {"uint32", id, "uint8", beheaded and 1 or 0}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,beheaded = packet:read("uint32", "uint8")
		if not ok then return end
		return {id, beheaded == 1}
	end,
	server_to_client_handle = function(self, id, beheaded)
		-- Get the object.
		local object = Main.objects:find_by_id(id)
		if not object then return end
		-- Remove or restore the head.
		object.render:set_beheaded(beheaded)
		-- Play the effect.
		if beheaded then
			Client.effects:play_object("behead1", object, "#neck")
		end
	end}
