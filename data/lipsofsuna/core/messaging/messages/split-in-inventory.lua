Message{
	name = "split in inventory",
	client_to_server_encode = function(self, id, index, count)
		return {"uint32", id, "uint32", index, "uint32", count}
	end,
	client_to_server_decode = function(self, packet)
		local ok,id,index,count = packet:read("uint32", "uint32", "uint32")
		if not ok then return end
		return {id, index, count}
	end,
	client_to_server_handle = function(self, client, id, index, count)
		-- Get the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Get the object.
		local parent = Main.objects:find_by_id(id)
		if not parent.inventory:is_subscribed(player) then return end
		if not player:can_reach_object(parent) then return end
		local object = parent.inventory:get_object_by_index(index)
		if not object then return end
		-- Validate the split.
		if count == 0 then return end
		if not object.spec.stacking then return end
		if object:get_count() < 2 then return end
		count = math.max(1, count)
		count = math.min(count, object:get_count() - 1)
		-- Split the stack.
		local dst = parent.inventory:get_empty_index()
		if not dst then
			player:send_message("No room for the split item")
			return
		end
		local split = object:split(count)
		parent.inventory:update_index(index)
		parent.inventory:set_object(dst, split)
	end}
