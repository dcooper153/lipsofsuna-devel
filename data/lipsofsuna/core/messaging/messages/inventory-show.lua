-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
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
