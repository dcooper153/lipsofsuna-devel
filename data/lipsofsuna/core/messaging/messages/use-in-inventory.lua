-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

Main.messaging:register_message{
	name = "use in inventory",
	client_to_server_encode = function(self, id, index, action)
		return {"uint32", id, "uint32", index, "string", action}
	end,
	client_to_server_decode = function(self, packet)
		local ok,id,index,action = packet:read("uint32", "uint32", "string")
		if not ok then return end
		return {id, index, action}
	end,
	client_to_server_handle = function(self, client, id, index, action)
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
		-- Validate the action.
		if not object.spec.usages[action] then return end
		-- Perform the action.
		player:action(action, object)
	end}
