local Class = require("system/class")
local Material = require("system/material")
local Noise = require("system/noise")

Generator.sector_types.Ocean = Class("Generator.Ocean")

Generator.sector_types.Ocean.init = function(self)
	self.scale1 = Vector(0.3,0.3,0.3)
	self.scale2 = Vector(0.15,0.3,0.15)
	self.mats = {
		Material:find{name = "ice1"},
		Material:find{name = "water1"}}
end

--- Generates a dungeon area.
-- @param self Dungeon generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
-- @param yield Yield function.
Generator.sector_types.Ocean.generate = function(self, pos, size, yield)
	-- Create granite.
	local m1 = Material:find{name = "granite1"}
	Voxel:fill_region{point = pos, size = size, tile = m1.id}
	-- Create ice.
	Noise:perlin_terrain(pos, pos + size, self.mats[1]:get_id(), 0.3, self.scale1, 4, 4, 0.25, Server.generator.seed2)
	yield()
	-- Create caverns.
	Noise:perlin_terrain(pos, pos + size, 0, 0.15, self.scale2, 2, 2, 0.2, Server.generator.seed1)
	yield()
	-- Create water.
	Noise:perlin_terrain(pos, pos + size, self.mats[2]:get_id(), 0.3, self.scale1, 4, 4, 0.25, Server.generator.seed3)
	yield()
	-- Create plants and ores.
	Server.generator:generate_resources(pos, size, 5, yield)
end
