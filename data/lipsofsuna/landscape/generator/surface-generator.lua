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
-- @param planner WorldPlanner.
-- @param seeds Array of seeds.
SurfaceGenerator.generate = function(self, chunk, planner)
	local w = chunk.manager.chunk_size
	for z = 0,w do
		local cz = chunk.z + z
		for x = 0,w do
			local cx = chunk.x + x
			local t,a,b,c = planner:get_height(cx, cz)
			self.__a:set(x, z, a)
			self.__b:set(x, z, b)
			self.__c:set(x, z, c)
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
