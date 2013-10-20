--- Procedural overworld surface generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.surface_generator
-- @alias SurfaceGenerator

local Class = require("system/class")
local Noise = require("system/noise")

--- Procedural overworld surface generator.
-- @type SurfaceGenerator
local SurfaceGenerator = Class("SurfaceGenerator")

--- Creates a new overworld surface generator.
-- @param clss SurfaceGenerator class.
-- @param chunk Chunk whose surface to generate.
-- @param seeds Array of seeds.
-- @return SurfaceGenerator.
SurfaceGenerator.new = function(clss, chunk, seeds)
	local self = Class.new(clss)
	local w = chunk.manager.chunk_size
	local h = {}
	local i = 0
	for z = chunk.z, chunk.z + w do
		for x = chunk.x, chunk.x + w do
			-- Choose the bumpiness of the region.
			-- p=0.7: Very smooth.
			-- p=0.9: Very bumpy.
			local r = Noise:plasma_noise_2d(seeds[1] + 0.01 * x, seeds[2] + 0.01 * z, 2)
			local p = 0.75 + 0.15 * r
			-- Choose the height of the region.
			-- This is affected by both the position and the bumpiness.
			local n1 = Noise:harmonic_noise_2d(seeds[1] + 0.001 * x, seeds[2] + 0.001 * z, 6, 1.3, p)
			-- Choose the soil layer height.
			-- This is affected by the height and the bumpiness.
			local s_base = 0.3 - 0.7 * (r + n1)
			local s_rand = Noise:harmonic_noise_2d(seeds[3] + 0.02 * x, seeds[4] + 0.02 * z, 3, 1.3, 1 - 0.25 * (n1 + r))
			local n2 = math.max(0, 0.5 * (s_base + s_rand))
			-- Choose the grass layer height.
			-- This is mostly just random on any areas with soil.
			local g = Noise:plasma_noise_2d(seeds[1] + 0.03 * x, seeds[2] + 0.03 * z, 3 - r)
			local n3 = math.max(0, g) * n2
			-- Store the heights.
			h[i] = 100 + 100 * n1
			h[i + 1] = 2 * n2
			h[i + 2] = 0.5 * n3
			i = i + 3
		end
	end
	self.h = h
	self.sx = 3
	self.sz = 3 * (w + 1)
	return self
end

--- Gets the surface heights by grid point.
-- @param self SurfaceGenerator.
-- @param x Point relative to the chunk origin.
-- @param z Point relative to the chunk origin.
-- @return Rock layer height, soil layer height, grass layer height.
SurfaceGenerator.get = function(self, x, z)
	local i = x * self.sx + z * self.sz
	return self.h[i], self.h[i+1], self.h[i+2]
end

--- Gets the average height of the given column.
-- @param self SurfaceGenerator.
-- @param x Column offset in grid units relative to the chunk origin.
-- @param z Column offset in grid units relative to the chunk origin.
-- @return Height in world units.
SurfaceGenerator.get_column_height = function(self, x, z)
	local h00 = self:get_height(x, z)
	local h10 = self:get_height(x + 1, z)
	local h01 = self:get_height(x, z + 1)
	local h11 = self:get_height(x + 1, z + 1)
	return (h00 + h10 + h01 + h11) / 4
end

--- Gets the ground type of the given column.
-- @param self SurfaceGenerator.
-- @param x Point relative to the chunk origin.
-- @param z Point relative to the chunk origin.
-- @return Height in world units.
SurfaceGenerator.get_column_type = function(self, x, z)
	local a00,b00,c00 = self:get(x, z)
	local a10,b10,c10 = self:get(x + 1, z)
	local a01,b01,c01 = self:get(x, z + 1)
	local a11,b11,c11 = self:get(x + 1, z + 1)
	if c00 > 0 or c10 > 0 or c01 > 0 or c11 > 0 then return "grass" end
	if b00 > 0 or b10 > 0 or b01 > 0 or b11 > 0 then return "soil" end
	return "rock"
end

--- Gets the total surface height of the grid point.
-- @param self SurfaceGenerator.
-- @param x Point relative to the chunk origin.
-- @param z Point relative to the chunk origin.
-- @return Height in world units.
SurfaceGenerator.get_height = function(self, x, z)
	local i = x * self.sx + z * self.sz
	return self.h[i] + self.h[i+1] + self.h[i+2]
end

return SurfaceGenerator
