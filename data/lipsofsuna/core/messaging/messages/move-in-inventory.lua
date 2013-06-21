-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "move in inventory",
	client_to_server_encode = function(self, id, src, dst)
		return {"uint32", id, "uint32", src, "uint32", dst}
	end,
	client_to_server_decode = function(self, packet)
		local ok,id,src,dst = packet:read("uint32", "uint32", "uint32")
		if not ok then return end
		return {id, src, dst}
	end,
	client_to_server_handle = function(self, client, id, src, dst)
		-- Find the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Get the modified inventory.
		local target = Main.objects:find_by_id(id)
		if not target then return end
		if not target.inventory:is_subscribed(player) then return end
		if not player:can_reach_object(target) then return end
		-- Get the moved item and validate the move.
		local item = target.inventory:get_object_by_index(src)
		if not item then return end
		if src == dst then return end
		if dst == 0 or dst > target.inventory.size then return end
		-- Move, merge or swap the items.
		local swapped = target.inventory:get_object_by_index(dst)
		if swapped then
			local slot1 = target.inventory:get_slot_by_index(src)
			local slot2 = target.inventory:get_slot_by_index(dst)
			if slot1 or slot2 or not target.inventory:merge_object_to_index(dst, item) then
				target.inventory:set_object(dst, item)
				target.inventory:set_object(src, swapped)
				if slot1 then target.inventory:equip_index(dst, slot1) end
				if slot2 then target.inventory:equip_index(src, slot2) end
			end
		else
			local slot1 = target.inventory:get_slot_by_index(src)
			item:detach()
			target.inventory:set_object(dst, item)
			if slot1 then target.inventory:equip_index(dst, slot1) end
		end
	end}
