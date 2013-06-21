Message{
	name = "inventory show",
	server_to_client_encode = function(self, id)
		return {"uint32", id}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id = packet:read("uint32")
		if not ok then return end
		return {id}
	end,
	server_to_client_handle = function(self, id)
		-- Find the object.
		local object = Main.objects:find_by_id(id)
		if not object then return end
		-- Show the inventory.
		if Ui:get_state() == "play" or Ui:get_state() == "world/object" then
			Client.data.inventory.id = id
			Ui:set_state("loot")
		end
	end}
