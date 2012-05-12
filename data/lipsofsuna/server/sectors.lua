--- Gets the center of the sector in world space coordinates.
-- @param self Sectors.
-- @return Position vector, in world space.
Sectors.get_sector_center = function(self, sector)
	local t = self:get_sector_offset(sector)
	local w = Voxel.tiles_per_line / 2
	return (t + Vector(w,w,w)) * Voxel.tile_size
end

--- Gets the offset of the sector in tiles.
-- @param self Sectors.
-- @param sector Sector index.
-- @return Vector.
Sectors.get_sector_offset = function(self, sector)
	local w = 128
	local sx = sector % w
	local sy = math.floor(sector / w) % w
	local sz = math.floor(sector / w / w) % w
	return Vector(sx, sy, sz) * Voxel.tiles_per_line
end

--- Called when a sector is created by the game.
-- @param self Sectors.
-- @param sector Sector index.
-- @param terrain True if terrain was loaded.
-- @param objects Array of objects.
Sectors.created_sector = function(self, sector, terrain, objects)
	-- Create fractal terrain for newly found sectors.
	if not terrain then Generator.inst:generate_sector(sector) end
	-- Don't spawn monsters in town sectors.
	local s = Generator.inst.sectors[sector]
	if s == "Town" then return end
	-- Decide how many monsters to spawn.
	-- The sector size is quite small so we spawn 1 monster per sector most
	-- of the time, none second often and 2 least often.
	local org = self:get_sector_offset(sector)
	local r = math.random()
	local monsters
	if org.y > 1000 then
		local dist = (self:get_sector_center(sector) - Utils:get_player_spawn_point()).length
		local threshold = math.min(dist / 40 - 1, 0.5)
		if r > threshold then return end
		monsters = 1
	else
		if r > 0.65 then return end
		monsters = (r > 0.80) and 1 or 2
	end
	-- Count monsters.
	-- If the sector already contains monsters, reduce the number of newly
	-- spawned monsters accodingly to not cramp the sector.
	for k,obj in pairs(objects) do
		if obj.spec.type == "actor" then
			monsters = monsters - 1
			if monsters == 0 then return end
		end
	end
	-- Spawn monsters.
	-- This is done in a thread to reduce pauses when lots of sectors are
	-- being loaded. It's useful since sectors are often loaded in clusters.
	Coroutine(function(thread)
		for i = 1,monsters do
			for j = 1,15 do
				local c = Vector()
				c.x = org.x + math.random(4, Voxel.tiles_per_line - 4)
				c.y = org.y + math.random(4, Voxel.tiles_per_line - 4)
				c.z = org.z + math.random(4, Voxel.tiles_per_line - 4)
				local p = Utils:find_spawn_point(c * Voxel.tile_size)
				if p then
					local d = Utils:get_spawn_point_difficulty(p, true)
					Voxel:place_actor{point = p * Voxel.tile_scale, category = "enemy", difficulty = d}
					break
				end
				coroutine.yield()
			end
			coroutine.yield()
		end
	end)
end
