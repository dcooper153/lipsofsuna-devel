local count_monsters = function(sector, objects)
	do return 0 end --FIXME
	if not Server.generator:is_overworld_sector_by_id(sector) then return 0 end
	local monsters = math.random(1, 2)
	for k,obj in pairs(objects) do
		if obj.spec.type == "lizardman" then
			monsters = monsters - 1
		end
	end
	return math.max(0, monsters)
end

Globaleventspec{
	name = "lizardman attack",
	duration = 200,
	sector_created = function(self, event, sector, loaded, objects)
		if not Server.generator:is_overworld_sector_by_id(sector) then return end
		local count = count_monsters(sector, objects)
		local spawns = Utils:find_spawn_points_in_sector(sector, count, true)
		for k,v in pairs(spawns) do
			v:multiply(Voxel.tile_scale)
			MapUtils:place_actor_by_name("lizardman", v)
		end
	end,
	started = function(self, event)
		event.silence_time = nil
	end,
	stopped = function(self, event)
		event.silence_time = math.random(1000, 3000)
	end,
	update = function(self, event, secs)
		do return end --FIXME
		-- Wait for the silence time to end.
		if event.start_time then return end
		if event.silence_time and event.silence_time > 0 then return end
		-- Start the event at a suitable moment.
		local check = function(player, status)
			-- FIXME: The condition is too arbitrary.
			if not Server.generator:is_overworld_sector_by_point(player:get_position()) then return end
			if #status.explored < 5 then return end
			if #status.eaten < 3 then return end
			if status.eaten[3] < status.explored[1]  then return end
			local t = Program:get_time()
			if t - status.explored[2] < 30 then return end
			if t - status.eaten[3] > 200 then return end
			return true
		end
		for k,v in pairs(Server.events.player_states) do
			if check(k, v) then
				Server.events:start_event("lizardman attack")
				break
			end
		end
	end}
