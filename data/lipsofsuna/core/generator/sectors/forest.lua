local Class = require("system/class")
local Material = require("system/material")
local Noise = require("system/noise")

Generator.sector_types.Forest = Class("Generator.Forest")

Generator.sector_types.Forest.init = function(self)
	self.scale1 = Vector(0.1,0.5,0.1)
	self.scale2 = Vector(0.2,0.2,0.2)
	self.mats = {
		Material:find{name = "granite1"},
		Material:find{name = "soil1"},
		Material:find{name = "grass1"}}
end

--- Generates a forest area.
-- @param self Forest generator.
-- @param pos Offset of the generated area.
-- @param size Size of the generated area.
Generator.sector_types.Forest.generate = function(self, pos, size)
	-- Create terrain.
	Voxel:fill_region{point = pos, size = size, tile = 0}
	Noise:perlin_terrain(pos, pos + size, self.mats[1]:get_id(), 0.15, self.scale1, 4, 4, 0.1, Server.generator.seed1)
	Noise:perlin_terrain(pos, pos + size, self.mats[2]:get_id(), 0.35, self.scale1, 4, 4, 0.15, Server.generator.seed1)
	Noise:perlin_terrain(pos, pos + size, self.mats[3]:get_id(), 0.45, self.scale1, 4, 4, 0.2, Server.generator.seed1)
	-- Spawn plants.
	local ord = {1,3,5,7,9}
	for x = pos.x+1,pos.x+size.x-2,2 do
		for z = pos.z+1,pos.z+size.z-2,2 do
			for j=1,5 do
				local i = math.random(1,5)
				ord[j],ord[i] = ord[i],ord[j]
			end
			for k,v in ipairs(ord) do
				if math.random(1,4)==1 then
					if Utils:spawn_plant_or_item(Vector(x,pos.y+v,z)) then
						break
					end
				end
			end
		end
	end
end
