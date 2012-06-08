local count_monsters = function(sector, objects)
	-- Decide how many monsters to spawn.
	-- The sector size is quite small so we spawn 1 monster per sector most
	-- of the time, none second often and 2 least often.
	local r = math.random()
	local monsters
	if Generator.inst:is_overworld_sector_by_id(sector) then
		local dist = (Serialize.sectors:get_sector_center(sector) - Utils:get_player_spawn_point()).length
		local threshold = math.min(dist / 40 - 1, 0.5)
		if r > threshold then return 0 end
		monsters = 1
	else
		if r > 0.65 then return 0 end
		monsters = (r > 0.80) and 1 or 2
	end
	-- Count monsters.
	-- If the sector already contains monsters, reduce the number of newly
	-- spawned monsters accodingly to not cramp the sector.
	for k,obj in pairs(objects) do
		if obj.spec.type == "actor" then
			monsters = monsters - 1
		end
	end
	return math.max(0, monsters)
end

Globaleventspec{
	name = "random monsters",
	sector_created = function(self, event, sector, loaded, objects)
		if Generator.inst:get_sector_type_by_id(sector) == "Town" then return end
		local count = count_monsters(sector, objects)
		local spawns = Utils:find_spawn_points_in_sector(sector, count, true)
		for k,v in pairs(spawns) do
			v:multiply(Voxel.tile_scale)
			local d = Utils:get_spawn_point_difficulty(v, true)
			Voxel:place_actor{point = v, category = "enemy", difficulty = d}
		end
	end}
