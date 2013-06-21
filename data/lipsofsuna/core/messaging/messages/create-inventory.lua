Message{
	name = "create inventory",
	server_to_client_encode = function(self, id, type, name, size, mine)
		return {"uint32", id, "string", type, "string", name, "uint8", size, "bool", mine}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,type,name,size,mine = packet:read("uint32", "string", "string", "uint8", "bool")
		if not ok then return end
		return {id, type, name, size, mine}
	end,
	server_to_client_handle = function(self, id, type, name, size, mine)
		-- Find the object.
		local object = Main.objects:find_by_id(id)
		if not object then return end
		-- Add the subscription.
		Operators.inventory:add_inventory(id)
		-- Initialize the inventory.
		if not object:has_server_data() then
			object.inventory:clear()
			object.inventory.size = size
		end
		-- Show the inventory.
		if not mine then
			Client.data.inventory.id = id
			if Ui:get_state() == "inventory" then
				Ui:push_state("loot")
			else
				Ui:set_state("loot")
			end
		end
	end}
