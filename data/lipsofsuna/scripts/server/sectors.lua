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
		if obj.spec.type == "species" then
			monsters = monsters - 1
			if monsters == 0 then return end
		end
	end
	-- Spawn monsters.
	local org = self:get_sector_offset(sector)
	local spawn = Config.spawn * Voxel.tile_scale
	for i = 1,monsters do
		for j = 1,30 do
			local c = Vector()
			c.x = org.x + 5 + math.random(0, Voxel.tiles_per_line - 10)
			c.y = org.y + 5 + math.random(0, Voxel.tiles_per_line - 10)
			c.z = org.z + 5 + math.random(0, Voxel.tiles_per_line - 10)
			if (c - spawn).length < 50 then break end
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
	local points = {
		Vector(-1,-1,-1), Vector(0,-1,-1), Vector(1,-1,-1),
		Vector(-1,0,-1), Vector(0,0,-1), Vector(1,0,-1),
		Vector(-1,1,-1), Vector(0,1,-1), Vector(1,1,-1),
		Vector(-1,-1,0), Vector(0,-1,0), Vector(1,-1,0),
		Vector(-1,0,0), Vector(0,0,0), Vector(1,0,0),
		Vector(-1,1,0), Vector(0,1,0), Vector(1,1,0),
		Vector(-1,-1,1), Vector(0,-1,1), Vector(1,-1,1),
		Vector(-1,0,1), Vector(0,0,1), Vector(1,0,1),
		Vector(-1,1,1), Vector(0,1,1), Vector(1,1,1)}
	local create_plant_or_item = function(ctr)
		local i = 0
		repeat
			i = i + 1
			ctr.y = ctr.y - 1
			if i > 10 then return end
		until Voxel:get_tile{point = ctr} ~= 0
		ctr.y = ctr.y + 1
		local src = ctr + Vector(-1,1,-1)
		local dst = ctr + Vector(1,3,1)
		ctr = ctr + Vector (0.5, 0, 0.5)
		if math.random() < 0.1 then
			Voxel:place_item{point = ctr, category = "generate"}
		elseif math.random() < 0.4 and Voxel:check_empty(src, dst) then
			Voxel:place_obstacle{point = ctr, category = "tree"}
		else
			Voxel:place_obstacle{point = ctr, category = "small-plant"}
		end
	end
	local create_vein = function(ctr, mat)
		for k,v in pairs(points) do
			if math.random() < 0.3 then
				local t = Voxel:get_tile{point = ctr + v}
				if t ~= 0 then
					Voxel:set_tile{point = ctr + v, tile = mat}
				end
			end
		end
	end
	-- Find regions that prohibit generation.
	prohibited = {}
	local size = Vector(Voxel.tiles_per_line, Voxel.tiles_per_line, Voxel.tiles_per_line)
	local aabb = Aabb{point = org, size = size}
	for k,v in pairs(Generator.regions_dict_id) do
		if not v.random_resources and aabb:intersects(v.aabb) then
			table.insert(prohibited, v.aabb)
		end
	end
	local validate_position = function(p)
		for k,v in pairs(prohibited) do
			if v:intersects_point(p) then return end
		end
		return true
	end
	-- Generate resources.
	local p = Vector()
	for i=1,50 do
		p.x = org.x + math.random(1, Voxel.tiles_per_line - 1)
		p.y = org.y + math.random(1, Voxel.tiles_per_line - 1)
		p.z = org.z + math.random(1, Voxel.tiles_per_line - 1)
		if validate_position(p) then
			local t = Voxel:get_tile{point = p}
			if t ~= 0 then
				local m = mats[math.random(1,#mats)]
				if m then create_vein(p, m.id, 1) end
			else
				create_plant_or_item(p)
			end
		end
	end
end
