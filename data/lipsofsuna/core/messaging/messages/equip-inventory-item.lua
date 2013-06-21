Message{
	name = "equip inventory item",
	server_to_client_encode = function(self, id, index, slot)
		return {"uint32", id, "uint8", index, "string", slot}
	end,
	server_to_client_decode = function(self, packet)
		local ok,id,index,slot = packet:read("uint32", "uint8", "string")
		if not ok then return end
		return {id, index, slot}
	end,
	server_to_client_handle = function(self, id, index, slot)
		-- Find the object.
		local object = Main.objects:find_by_id(id)
		if not object then return end
		-- Equip the index.
		if not object:has_server_data() then
			object.inventory:equip_index(index, slot)
		end
		-- Update the user interface.
		if Ui:get_state() == "inventory" and object == Client.player_object then
			Ui:restart_state()
		elseif Ui:get_state() == "loot" and object:get_id() == Client.data.inventory.id then
			Ui:restart_state()
		elseif Ui:get_state() == "store" and object == Client.player_object then
			Ui:restart_state()
		end
	end}
