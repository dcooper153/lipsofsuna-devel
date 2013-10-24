--- Base class for place generators.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.place_generator
-- @alias PlaceGenerator

local Class = require("system/class")

--- Base class for place generators.
-- @type PlaceGenerator
local PlaceGenerator = Class("PlaceGenerator")

--- Creates a new place generator.
-- @param clss PlaceGenerator class.
-- @param generator TerrainGenerator.
-- @param planner WorldPlanner.
-- @return PlaceGenerator.
PlaceGenerator.new = function(clss, generator, planner)
	local self = Class.new(clss)
	self.__generator = generator
	self.__planner = planner
	return self
end

--- Generates a chunk of the place.
-- @param self PlaceGenerator.
-- @param chunk TerrainChunk.
-- @param params Place parameters.
PlaceGenerator.generate = function(self, chunk, params)
end

--- Plans the place.
-- @param self PlaceGenerator.
-- @param x Chunk X coordinate.
-- @param z Chunk Z coordinate.
-- @return True if planning succeded. False otherwise.
PlaceGenerator.plan = function(self, x, z)
end

return PlaceGenerator
