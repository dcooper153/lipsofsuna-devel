local Actor = require("core/objects/actor")
local Class = require("system/class")
local Item = require("core/objects/item")
local Material = require("system/material")

Generator.sector_types.LabyrinthBoss = Class("Generator.LabyrinthBoss")

Generator.sector_types.LabyrinthBoss.init = function(self)
	self.scale1 = Vector(0.3,0.3,0.3)
	self.scale2 = Vector(0.15,0.3,0.15)
	self.mats = {
		Material:find{name = "granite1"},
		Material:find{name = "brick1"}}
end

--- Generates a labyrinth boss sector.
-- @param self Labyrinth boss generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
-- @param yield Yield function.
Generator.sector_types.LabyrinthBoss.generate = function(self, pos, size, yield)
	local a,c,d = size.x,3,6
	Voxel:fill_region{point = pos, size = size, tile = self.mats[2]:get_id()}
	Voxel:fill_region{point = pos + Vector(0,c,0), size = Vector(a,d,a), tile = 0}
	yield()
	Actor{
		spec = "dragon",
		position = (pos + Vector(a/2,c,a/2)) * Voxel.tile_size,
		random = true,
		realized = true}
	Item{
		spec = "treasure chest",
		position = (pos + Vector(3,c,3)) * Voxel.tile_size,
		random = true,
		realized = true}
	Item{
		spec = "treasure chest",
		position = (pos + Vector(10,c,6)) * Voxel.tile_size,
		random = true,
		realized = true}
end
