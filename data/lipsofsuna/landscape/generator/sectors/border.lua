local Class = require("system/class")
local Material = require("system/material")

Generator.sector_types.Border = Class("Generator.Border")

Generator.sector_types.Border.init = function(self)
	self.mats = {
		Material:find{name = "basalt1"}} -- FIXME
end

--- Generates a map border area.
-- @param self Border generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
-- @param yield Yield function.
Generator.sector_types.Border.generate = function(self, pos, size, yield)
	Voxel:fill_region{point = pos, size = size, tile = self.mats[1]:get_id()}
end
