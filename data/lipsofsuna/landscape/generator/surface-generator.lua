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
local NumberArray2d = require("system/array/number-2d")

--- Procedural overworld surface generator.
-- @type SurfaceGenerator
local SurfaceGenerator = Class("SurfaceGenerator")

--- Creates a new overworld surface generator.
-- @param clss SurfaceGenerator class.
-- @return SurfaceGenerator.
SurfaceGenerator.new = function(clss, size)
	local self = Class.new(clss)
	self.__a = NumberArray2d(size+1, size+1)
	self.__b = NumberArray2d(size+1, size+1)
	self.__c = NumberArray2d(size+1, size+1)
	return self
end

--- Generates the surface for the given chunk.
-- @param chunk Chunk whose surface to generate.
-- @param heights Diamond-Square generator.
-- @param seeds Array of seeds.
SurfaceGenerator.generate = function(self, chunk, heights, seeds)
	local w = chunk.manager.chunk_size
	for z = 0,w do
		local cz = chunk.z + z
		for x = 0,w do
			local cx = chunk.x + x
			-- Choose the bumpiness of the region.
			-- This is affected by the slope steepness.
			-- p=0.7: Very smooth.
			-- p=0.9: Very bumpy.
			local gx,gy,gv = heights:get_gradient(cx / w, cz / w)
			local g = math.min(gv/50, 1)
			local r = math.abs(Noise:plasma_noise_2d(seeds[1] + 0.01 * cx, seeds[2] + 0.01 * cz, 2))
			local p = math.min(0.9, 0.7 + 0.1 * r + 0.25 * g)
			-- Choose the height of the region.
			-- This is affected by both the position and the bumpiness.
			local n1a = heights:get_height(cx / w, cz / w)
			local n1b = Noise:harmonic_noise_2d(seeds[1] + 0.001 * cx, seeds[2] + 0.001 * cz, 6, 1.3, p)
			local n1 = n1a + 20 * n1b
			-- Choose the soil layer height.
			-- This is affected by the bumpiness and the slope steepness.
			local n2a = Noise:harmonic_noise_2d(seeds[3] + 0.02 * cx, seeds[4] + 0.02 * cz, 3, 1.3, 0.5 + g)
			local n2b = Noise:plasma_noise_2d(seeds[1] + 0.02 * cx, seeds[2] + 0.02 * cz, 3 - r)
			local n2 = 10 * math.max(0, 1 - 0.5 * g - 0.8 + 0.2 * n2a)
			-- Choose the grass layer height.
			-- This is mostly just random on any areas with soil.
			local gr = Noise:plasma_noise_2d(seeds[1] + 0.03 * cx, seeds[2] + 0.03 * cz, 3 - r)
			local n3 = math.max(0, gr) * n2
			-- Store the heights.
			self.__a:set(x, z, n1)
			self.__b:set(x, z, 2 * n2)
			self.__c:set(x, z, 0.5 * n3)
		end
	end
end

--- Gets the surface heights by grid point.
-- @param self SurfaceGenerator.
-- @param x Point relative to the chunk origin.
-- @param z Point relative to the chunk origin.
-- @return Rock layer height, soil layer height, grass layer height.
SurfaceGenerator.get = function(self, x, z)
	return self.__a:get(x, z), self.__b:get(x, z), self.__c:get(x, z)
end

--- Gets the ground type of the given column.
-- @param self SurfaceGenerator.
-- @param x Point relative to the chunk origin.
-- @param z Point relative to the chunk origin.
-- @return Height in world units.
SurfaceGenerator.get_column_type = function(self, x, z)
	local c = self.__c:get_bilinear(x + 0.5, z + 0.5)
	if c > 0 then return "grass" end
	local b = self.__b:get_bilinear(x + 0.5, z + 0.5)
	if b > 0 then return "soil" end
	return "rock"
end

--- Gets the total surface height of the grid point.
-- @param self SurfaceGenerator.
-- @param x Point relative to the chunk origin.
-- @param z Point relative to the chunk origin.
-- @return Height in world units.
SurfaceGenerator.get_height = function(self, x, z)
	return self.__a:get(x, z) + self.__b:get(x, z) + self.__c:get(x, z)
end

--- Gets the total surface height of the bilinearly interpolated point.
-- @param self SurfaceGenerator.
-- @param x Point relative to the chunk origin.
-- @param z Point relative to the chunk origin.
-- @return Height in world units.
SurfaceGenerator.get_height_bilinear = function(self, x, z)
	return self.__a:get_bilinear(x, z) + self.__b:get_bilinear(x, z) + self.__c:get_bilinear(x, z)
end

return SurfaceGenerator
