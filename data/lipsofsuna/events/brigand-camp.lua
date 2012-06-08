Globaleventspec{
	name = "brigand camp",
	sector_created = function(self, event, sector, loaded, objects)
		-- Check if the sector is suitable.
		if Generator.inst:get_sector_type_by_id(sector) == "Town" then return end
		-- Check the states of the nearby players.
		-- FIXME: The condition is arbitrary.
		local now = Program.time
		local players = Globaleventmanager:find_players_exploring_sector(sector)
		local failed
		for k,v in pairs(players) do
			if not v.brigand_camp_time then
				v.brigand_camp_time = now
				failed = true
			elseif now - v.brigand_camp_time < 300 then
				failed = true
			elseif #v.dialog > 0 and now - v.dialog[1] < 60 then
				failed = true
			end
		end
		if failed then return end
		-- Find the spawn points.
		local count = math.random(3,6)
		local spawns = Utils:find_spawn_points_in_sector(sector, count, true)
		if #spawns < 3 then return end
		-- Spawn the brigands.
		for k,v in pairs(spawns) do
			v:multiply(Voxel.tile_scale)
			Voxel:place_actor{point = v, name = "brigandmale"}
		end
		for k,v in pairs(players) do
			v.brigand_camp_time = now
		end
	end}
