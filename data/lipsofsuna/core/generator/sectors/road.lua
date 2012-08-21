local Class = require("system/class")
local Material = require("system/material")
local Noise = require("system/noise")

Generator.sector_types.Road = Class("Generator.Road")

Generator.sector_types.Road.init = function(self)
	self.scale1 = Vector(0.3,0.3,0.3)
	self.scale2 = Vector(0.15,0.3,0.15)
	self.mats = {
		Material:find{name = "soil1"},
		Material:find{name = "grass1"},
		Material:find{name = "sand1"}}
end

--- Generates a town sector.
-- @param self Town generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.sector_types.Road.generate = function(self, pos, size)
	-- Create granite.
	local m1 = Material:find{name = "granite1"}
	Voxel:fill_region{point = pos, size = size, tile = m1:get_id()}
	-- Create common tiles.
	-- FIXME: The distribution should be much more random.
	local m = self.mats[math.random(1,#self.mats)]
	if m then
		Noise:perlin_terrain(pos, pos + size, m:get_id(), 0.5, self.scale1, 4, 4, 0.25, Server.generator.seed2)
	end
	-- Create caverns.
	Noise:perlin_terrain(pos, pos + size, 0, 0.1, self.scale2, 2, 2, 0.3, Server.generator.seed1)
	-- Create plants and ores.
	Server.generator:generate_resources(pos, size, 5)
end
