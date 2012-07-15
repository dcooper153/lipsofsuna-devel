Generator.sector_types.Overworld = Class()
Generator.sector_types.Overworld.class_name = "Generator.Overworld"

Generator.sector_types.Overworld.init = function(self)
	self.scale1 = Vector(0.2,0.2,0.2)
end

--- Generates an overworld area.
-- @param self Overworld generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.sector_types.Overworld.generate = function(self, pos, size)
	-- Generate random rocks.
	local m1 = Material:find{name = "granite1"}
	local p1 = Vector()
	local p2 = Vector()
	local miny = pos.y * Voxel.tile_size
	local maxy = (pos.y + size.y) * Voxel.tile_size
	for x = 0,size.x-1 do
		p1.x = pos.x + x
		p2.x = (p1.x + 0.5) * Voxel.tile_size
		for z = 0,size.z-1 do
			p1.z = pos.z + z
			p2.z = (p1.z + 0.5) * Voxel.tile_size
			local height = Map.heightmap:get_height(p2, false)
			if height and height >= miny and height < maxy then
				p1.y = math.ceil(height * Voxel.tile_scale) - 1
				local n1 = Noise:perlin_noise(p1, self.scale1, 1, 3, 0.5, Generator.inst.seed1)
				if n1 > 0.8 then
					Voxel:fill_region{point = p1, size = Vector(1, 1+(n1-0.8)/0.3, 1), tile = m1.id}
				end
			end
		end
	end
	-- Generate objects.
	local c = Vector()
	for i=1,10 do
		c.x = pos.x + math.random(1, size.x - 2)
		c.y = pos.y + math.random(1, size.y - 2)
		c.z = pos.z + math.random(1, size.z - 2)
		local p = Utils:find_spawn_point(c * Voxel.tile_size)
		if p then
			Voxel:place_obstacle{point = p * Voxel.tile_scale, category = "small-plant"}
		end
	end
end
