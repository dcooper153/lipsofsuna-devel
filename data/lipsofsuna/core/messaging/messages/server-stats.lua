-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local Database = require("system/database")

Main.messaging:register_message{
	name = "server stats",
	client_to_server_encode = function(self)
		return {}
	end,
	client_to_server_decode = function(self, packet)
		return {}
	end,
	client_to_server_handle = function(self, client)
		-- Check for permissions.
		local player = Server:get_player_by_client(client)
		if not player then return end
		if not player:get_admin() then
			Main.messaging:server_event("message", "You have no permission to do that.")
			return
		end
		-- Count objects.
		local num_players_miss = 0
		local num_players_real = 0
		local num_actors_idle = 0
		local num_actors_miss = 0
		local num_actors_real = 0
		local num_items_miss = 0
		local num_items_inv = 0
		local num_items_real = 0
		local num_obstacles_miss = 0
		local num_obstacles_real = 0
		local num_objects_miss = 0
		local num_objects_real = 0
		local num_vision_miss = 0
		local num_vision_real = 0
		for k,v in pairs(Main.objects.objects_by_id) do
			if v.class_name == "Player" then
				if v:get_visible() then
					num_players_real = num_players_real + 1
				else
					num_players_miss = num_players_miss + 1
				end
				if v.vision then
					for k1,v1 in pairs(v.vision.objects) do
						if k1:get_visible() then
							num_vision_real = num_vision_real + 1
						else
							num_vision_miss = num_vision_miss + 1
						end
					end
				end
			elseif v.class_name == "Actor" then
				if v:get_visible() then
					if v.ai and v.ai.state ~= "none" then
						num_actors_real = num_actors_real + 1
					else
						num_actors_idle = num_actors_idle + 1
					end
				else
					num_actors_miss = num_actors_miss + 1
				end
			elseif v.class_name == "Item" then
				if v:get_visible() then
					num_items_real = num_items_real + 1
				elseif v.parent then
					num_items_inv = num_items_inv + 1
				else
					num_items_miss = num_items_miss + 1
				end
			elseif v.class_name == "Obstacle" then
				if v:get_visible() then
					num_obstacles_real = num_obstacles_real + 1
				else
					num_obstacles_miss = num_obstacles_miss + 1
				end
			else
				if v:get_visible() then
					num_objects_real = num_objects_real + 1
				else
					num_objects_miss = num_objects_miss + 1
				end
			end
		end
		-- Count sectors.
		local num_sectors = 0
		for k,v in pairs(Main.game.sectors.sectors) do
			num_sectors = num_sectors + 1
		end
		-- Send stats.
		local response = string.format([[FPS: %.2f
Database memory: %d kB
Script memory: %d kB
Terrain memory: %d kB

%s

Players: %d+%d
Actors: %d+%d+%d
Items: %d+%d+%d
Obstacles: %d+%d
Others: %d+%d
Vision: %d+%d
Sectors: %d]],
			Program:get_fps(), Database:get_memory_used() / 1024, collectgarbage("count") / 1024, Voxel:get_memory_used() / 1024,
			Main.timing:get_profiling_string(),
			num_players_real, num_players_miss,
			num_actors_real, num_actors_idle, num_actors_miss,
			num_items_real, num_items_inv, num_items_miss,
			num_obstacles_real, num_obstacles_miss,
			num_objects_real, num_objects_miss,
			num_vision_real, num_vision_miss,
			num_sectors)
		Main.messaging:server_event("server stats", client, response)
	end,
	server_to_client_encode = function(self, text)
		return {"string", text}
	end,
	server_to_client_decode = function(self, packet)
		local ok,text = packet:read("string")
		if not ok then return end
		return {text}
	end,
	server_to_client_handle = function(self, text)
		Client.data.admin.server_stats = text
		Ui:set_state("admin/server-stats")
	end}
