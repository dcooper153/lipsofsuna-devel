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
	-- Count monsters.
	local monsters = 1
	for k,obj in pairs(objects) do
		if obj.species then
			monsters = monsters - 1
			if monsters == 0 then return end
		end
	end
	-- Spawn monsters.
	local org = self:get_sector_offset(sector)
	for i = 1,monsters do
		for j = 1,30 do
			local c = Vector()
			c.x = org.x + 5 + math.random(0, Voxel.tiles_per_line - 10)
			c.y = org.y + 5 + math.random(0, Voxel.tiles_per_line - 10)
			c.z = org.z + 5 + math.random(0, Voxel.tiles_per_line - 10)
			local t,p = Voxel:find_tile{match = "empty", point = c * Config.tilewidth, radius = 3 * Config.tilewidth}
			if t and Voxel:get_tile{point = p + Vector(0, 1, 0)} == 0 then
				p = p + Vector(0.5, 0.5, 0.5)
				Voxel:place_creature{point = p, category = "enemy"}
				break
			end
		end
	end
end

--- Called when an empty sector is created by the generator.
-- @param self Sectors.
-- @param sector Sector index.
Sectors.format_generated_sector = function(self, sector)
	local org = self:get_sector_offset(sector)
	local mats = {
		Material:find{name = "adamantium1"},
		Material:find{name = "aquanite1"},
		Material:find{name = "basalt1"},
		Material:find{name = "brittlerock1"},
		Material:find{name = "crimson1"},
		Material:find{name = "ferrostone1"},
		Material:find{name = "sand1"},
		Material:find{name = "soil1"}}
	--[[Material:find{name = "grass1"},
		Material:find{name = "ice1"},
		Material:find{name = "iron1"},
		Material:find{name = "magma1"},
		Material:find{name = "pipe1"},
		Material:find{name = "water1"},
		Material:find{name = "wood1"}]]
	local growdirs = {
		Vector(-1, 0, 0),
		Vector(1, 0, 0),
		Vector(0, -1, 0),
		Vector(0, 1, 0),
		Vector(0, 0, -1),
		Vector(0, 0, 1)}
	local growrand
	growrand = function(ctr, mat, dep)
		local p = org + ctr
		local t = Voxel:get_tile{point = p}
		if t == 0 then return end
		Voxel:set_tile{point = p, tile = mat}
		if dep > 4 then return end
		for k,v in pairs(growdirs) do
			if math.random() < (4 - dep) * 0.15 then
				local p = ctr + v
				p.x = math.max(math.min(p.x, Voxel.tiles_per_line - 1), 1)
				p.y = math.max(math.min(p.y, Voxel.tiles_per_line - 1), 1)
				p.z = math.max(math.min(p.z, Voxel.tiles_per_line - 1), 1)
				growrand(p, mat, dep + 1)
			end
		end
	end
	local p = Vector()
	for i=1,100 do
		p.x = math.random(1, Voxel.tiles_per_line - 1)
		p.y = math.random(1, Voxel.tiles_per_line - 1)
		p.z = math.random(1, Voxel.tiles_per_line - 1)
		local m = mats[math.random(1,#mats)]
		if m then growrand(p, m.id, 1) end
	end
end
