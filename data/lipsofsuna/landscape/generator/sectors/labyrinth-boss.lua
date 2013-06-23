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
	local o1 = Actor(Main.objects)
	o1:set_spec(Actorspec:find_by_name("dragon"))
	o1:set_position(pos + Vector(a/2,c,a/2))
	o1:randomize()
	o1:set_visible(true)
	local o2 = Item(Main.objects)
	o2:set_spec(Itemspec:find_by_name("treasure chest"))
	o2:set_position(pos + Vector(3,c,3))
	o2:randomize()
	o2:set_visible(true)
	local o3 = Item(Main.objects)
	o3:set_spec(Itemspec:find_by_name("treasure chest"))
	o3:set_position(pos + Vector(10,c,6))
	o3:randomize()
	o3:set_visible(true)
end
