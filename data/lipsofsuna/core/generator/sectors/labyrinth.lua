local Class = require("system/class")
local Item = require("core/objects/item")
local Material = require("system/material")
local Noise = require("system/noise")

Generator.sector_types.Labyrinth = Class("Generator.Labyrinth")

Generator.sector_types.Labyrinth.init = function(self)
	self.scale1 = Vector(1.0,1.0,1.0)
	self.scale2 = Vector(0.15,0.3,0.15)
	self.mats = {
		Material:find{name = "granite1"},
		Material:find{name = "brick1"}}
end

--- Generates a labyrinth sector.
-- @param self Labyrinth generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.sector_types.Labyrinth.generate = function(self, pos, size)
	-- A/////|  |/////  +-------------
	-- A/////|R3|/////  |C //////////|
	--  -----+  +-----  +------------+
	-- B  R4  R1  R2     D
	--  -----+  +-----  +------------+
	-- A/////|R5|/////  |C //////////|
	-- A/////|  |/////  +------------+
	--  AAAAA BB AAAAA
	local t1 = 0.1
	local t2 = -0.2
	local a,b,c,d,a2,b2,d2 = 3,6,3,6,5,2,4
	local r2 = Noise:perlin_noise(pos + Vector(size.x,0,size.z/2), self.scale1, 1, 3, 0.5, Server.generator.seed1)
	local r3 = Noise:perlin_noise(pos + Vector(size.x/2,0,size.z), self.scale1, 1, 3, 0.5, Server.generator.seed1)
	local r4 = Noise:perlin_noise(pos + Vector(0,0,size.z/2), self.scale1, 1, 3, 0.5, Server.generator.seed1)
	local r5 = Noise:perlin_noise(pos + Vector(size.x/2,0,0), self.scale1, 1, 3, 0.5, Server.generator.seed1)
	Voxel:fill_region{point = pos, size = size, tile = self.mats[2]:get_id()}
	Voxel:fill_region{point = pos + Vector(a,c,a), size = Vector(b,d,b), tile = 0}
	if r2 > t1 then
		Voxel:fill_region{point = pos + Vector(a+b,c,a), size = Vector(a,d,b), tile = 0}
	elseif r2 > t2 then
		Voxel:fill_region{point = pos + Vector(a+b,c,a2), size = Vector(a,d2,b2), tile = 0}
	end
	if r3 > t1 then
		Voxel:fill_region{point = pos + Vector(a,c,a+b), size = Vector(b,d,a), tile = 0}
	elseif r3 > t2 then
		Voxel:fill_region{point = pos + Vector(a2,c,a+b), size = Vector(b2,d2,a), tile = 0}
	end
	if r4 > t1 then
		Voxel:fill_region{point = pos + Vector(0,c,a), size = Vector(a,d,b), tile = 0}
	elseif r4 > t2 then
		Voxel:fill_region{point = pos + Vector(0,c,a2), size = Vector(a,d2,b2), tile = 0}
	end
	if r5 > t1 then
		Voxel:fill_region{point = pos + Vector(a,c,0), size = Vector(b,d,a), tile = 0}
	elseif r5 > t2 then
		Voxel:fill_region{point = pos + Vector(a2,c,0), size = Vector(b2,d2,a), tile = 0}
	end
	local rnd = math.random()
	if rnd > 0.8 then
		Item{
			spec = "treasure chest",
			position = (pos + Vector(b,c,b)) * Voxel.tile_size,
			random = true,
			realized = true}
	elseif rnd > 0.6 then
		Item{
			spec = "chest",
			position = (pos + Vector(b,c,b)) * Voxel.tile_size,
			random = true,
			realized = true}
	end
end
