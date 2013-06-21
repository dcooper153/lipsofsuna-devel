Message{
	name = "object hidden",
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
		local obj = Main.objects:find_by_id(id)
		if not obj then return end
		-- Hide the object.
		if obj:get_static() then return end
		if obj.render then
			obj.render:clear()
		end
		if not obj:has_server_data() then
			obj:detach()
		end
	end}
