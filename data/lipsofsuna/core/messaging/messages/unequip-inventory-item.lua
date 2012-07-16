Message{
	name = "unequip inventory item",
	server_to_client_encode = function(self, id, index)
		return {"uint32", id, "uint8", index}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,index = packet:read("uint32", "uint8")
		if not ok then return end
		return {id, index}
	end,
	server_to_client_handle = function(self, id, index)
		-- Find the object.
		local object = Object:find{id = id}
		if not object then return end
		-- Unequip the index.
		if not object:has_server_data() then
			object.inventory:unequip_index(index)
		end
		-- Update the user interface.
		if Ui.state == "inventory" and object == Client.player_object then
			Ui:restart_state()
		elseif Ui.state == "loot" and object.id == Client.data.inventory.id then
			Ui:restart_state()
		elseif Ui.state == "store" and object == Client.player_object then
			Ui:restart_state()
		end
	end}
