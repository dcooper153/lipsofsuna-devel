-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local Quaternion = require("system/math/quaternion")

Main.messaging:register_message{
	name = "drop from inventory",
	client_to_server_encode = function(self, index, count)
		return {"uint32", index, "uint32", count}
	end,
	client_to_server_decode = function(self, packet)
		local ok,index,count = packet:read("uint32", "uint32")
		if not ok then return end
		return {index, count}
	end,
	client_to_server_handle = function(self, client, index, count)
		-- Find the player.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if player.dead then return end
		-- Read the inventory index and drop count.
		-- Get the item.
		local object = player.inventory:get_object_by_index(index)
		if not object then return end
		-- Split the dropped stack.
		count = math.min(object:get_count(), count)
		local split = object:split(count)
		-- Drop the item.
		-- TODO: Better positioning.
		split:set_position(player:get_position() + player:get_rotation() * Vector(0,0,-1) + Vector(0,1))
		split:set_velocity(Vector())
		split:set_rotation(Quaternion())
		split:set_visible(true)
	end}
