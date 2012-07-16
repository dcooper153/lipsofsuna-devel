Generator.sector_types.Town = Class()
Generator.sector_types.Town.class_name = "Generator.Town"

Generator.sector_types.Town.init = function(self)
	self.scale1 = Vector(0.3,0.3,0.3)
	self.scale2 = Vector(0.15,0.3,0.15)
	self.mats = {
		Material:find{name = "granite1"},
		Material:find{name = "soil1"},
		Material:find{name = "grass1"}}
end

--- Generates a town sector.
-- @param self Town generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.sector_types.Town.generate = function(self, pos, size)
	-- Create terrain.
	Voxel:fill_region{point = pos, size = size, tile = 0}
	Noise:perlin_terrain(pos, pos + size, self.mats[1].id, 0.1, self.scale1, 4, 4, 0.1, Server.generator.seed1)
	Noise:perlin_terrain(pos, pos + size, self.mats[2].id, 0.35, self.scale1, 4, 4, 0.15, Server.generator.seed1)
	Noise:perlin_terrain(pos, pos + size, self.mats[3].id, 0.45, self.scale1, 4, 4, 0.2, Server.generator.seed1)
end
