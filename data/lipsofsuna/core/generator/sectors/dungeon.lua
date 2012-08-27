local Class = require("system/class")
local Material = require("system/material")
local Noise = require("system/noise")

Generator.sector_types.Dungeon = Class("Generator.Dungeon")

Generator.sector_types.Dungeon.init = function(self)
	self.scale1 = Vector(0.6,0.6,0.6)
	self.scale2 = Vector(0.3,0.6,0.3)
	self.mats = {
		Material:find{name = "ferrostone1"},
		Material:find{name = "sand1"},
		Material:find{name = "soil1"}}
end

--- Generates a dungeon area.
-- @param self Dungeon generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
-- @param yield Yield function.
Generator.sector_types.Dungeon.generate = function(self, pos, size, yield)
	-- Create granite.
	local m1 = Material:find{name = "granite1"}
	Voxel:fill_region{point = pos, size = size, tile = m1:get_id()}
	yield()
	-- Create common tiles.
	-- FIXME: The distribution should be much more random.
	local m = self.mats[math.random(1,#self.mats)]
	if m then
		Noise:perlin_terrain(pos, pos + size, m:get_id(), 0.5, self.scale1, 4, 4, 0.25, Server.generator.seed2)
	end
	yield()
	-- Create caverns.
	Noise:perlin_terrain(pos, pos + size, 0, 0.1, self.scale2, 5, 5, 0.1, Server.generator.seed1)
	-- Create plants and ores.
	Server.generator:generate_resources(pos, size, 3, yield)
end
