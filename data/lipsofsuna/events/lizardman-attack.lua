local count_monsters = function(sector, objects)
	if not Generator.inst:is_overworld_sector_by_id(sector) then return 0 end
	local monsters = math.random(1, 2)
	for k,obj in pairs(objects) do
		if obj.spec.type == "lizardman" then
			monsters = monsters - 1
		end
	end
	return math.max(0, monsters)
end

local spawn_monsters = function(sector, objects, monsters)
	local org = Serialize.sectors:get_sector_offset(sector)
	for i = 1,monsters do
		for j = 1,15 do
			local c = Vector()
			c.x = org.x + math.random(4, Voxel.tiles_per_line - 4)
			c.y = org.y + math.random(4, Voxel.tiles_per_line - 4)
			c.z = org.z + math.random(4, Voxel.tiles_per_line - 4)
			local p = Utils:find_spawn_point(c * Voxel.tile_size)
			if p then
				local d = Utils:get_spawn_point_difficulty(p, true)
				Voxel:place_actor{point = p * Voxel.tile_scale, name = "lizardman"}
				break
			end
			coroutine.yield()
		end
		coroutine.yield()
	end
end

Globaleventspec{
	name = "lizardman attack",
	duration = 200,
	sector_created = function(self, event, sector, loaded, objects)
		if Generator.inst:is_overworld_sector_by_id(sector) then
			local monsters = count_monsters(sector, objects)
			spawn_monsters(sector, objects, monsters)
		end
	end,
	started = function(self, event)
		event.silence_time = nil
	end,
	stopped = function(self, event)
		event.silence_time = math.random(1000, 3000)
	end,
	update = function(self, event, secs)
		-- Wait for the silence time to end.
		if event.start_time then return end
		if event.silence_time and event.silence_time > 0 then return end
		-- Start the event at a suitable moment.
		local check = function(player, status)
			-- FIXME: The condition is too arbitrary.
			if not Generator.inst:is_overworld_sector_by_point(player.position) then return end
			if #status.explored < 5 then return end
			if #status.eaten < 3 then return end
			if status.eaten[3] < status.explored[1]  then return end
			local t = Program.time
			if t - status.explored[2] < 30 then return end
			if t - status.eaten[3] > 200 then return end
			return true
		end
		for k,v in pairs(Globaleventmanager.player_states) do
			if check(k, v) then
				Globaleventmanager:start_event("lizardman attack")
				break
			end
		end
	end}
