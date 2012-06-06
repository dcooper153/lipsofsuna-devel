local count_monsters = function(sector, objects)
	if not Generator.inst:is_overworld_sector_by_id(sector) then return 0 end
	local monsters = math.random(2, 4)
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
	duration = 600,
	sector_created = function(self, event, sector, loaded, objects)
		if Generator.inst:is_overworld_sector_by_id(sector) then
			local monsters = count_monsters(sector, objects)
			spawn_monsters(sector, objects, monsters)
		end
	end}
