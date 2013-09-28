--- Generic map chunk manager.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.chunk_manager
-- @alias ChunkManager

local Class = require("system/class")

--- Generic map chunk manager.
-- @type ChunkManager
local ChunkManager = Class("ChunkManager")

--- Creates a new chunk manager.
-- @param clss ChunkManager class.
-- @param chunk_size Number of grid points per chunk.
-- @param grid_size Grid point spacing in world units.
-- @return ChunkManager.
ChunkManager.new = function(clss, chunk_size, grid_size)
	local self = Class.new(clss)
	self.chunk_size = chunk_size
	self.grid_size = grid_size
	return self
end

--- Maps a world space point to a chunk ID.
-- @param self ChunkManager.
-- @param x X coordinate in world units.
-- @param z Z coordinate in world units.
-- @return Chunk ID.
ChunkManager.get_chunk_id_by_point = function(self, x, z)
	local chunk_width = self.chunk_size * self.grid_size
	local x = math.floor(x / chunk_width)
	local z = math.floor(z / chunk_width)
	return x + z * 0xFFFF
end

--- Maps grid coordinates to an internal chunk ID.
-- @param self ChunkManager.
-- @param x X coordinate in grid units.
-- @param z Z coordinate in grid units.
-- @return Chunk ID.
ChunkManager.get_chunk_id_by_xz = function(self, x, z)
	return math.floor(x / self.chunk_size) + math.floor(z / self.chunk_size) * 0xFFFF
end

--- Maps an internal chunk ID to grid coordinates.
-- @param self ChunkManager.
-- @param id Chunk ID.
-- @return Grid coordinates.
ChunkManager.get_chunk_xz_by_id = function(self, id)
	return (id % 0xFFFF) * self.chunk_size, math.floor(id / 0xFFFF) * self.chunk_size
end

--- Maps a world space point to grid coordinates.
-- @param self ChunkManager.
-- @param x X coordinate in world units.
-- @param z Z coordinate in world units.
-- @return Grid coordinates.
ChunkManager.get_chunk_xz_by_point = function(self, x, z)
	local chunk_width = self.chunk_size * self.grid_size
	local x = math.floor(x / chunk_width) * self.chunk_size
	local z = math.floor(z / chunk_width) * self.chunk_size
	return x, z
end

--- Gets the XZ grid point range of chunks inside the given sphere.
-- @param self ChunkManager.
-- @param point Point in world space.
-- @param radius Radius in world units.
-- @return X min, Z min, X max, Z max
ChunkManager.get_chunk_xz_range_by_point = function(self, point, radius)
	local chunk_width = self.chunk_size * self.grid_size
	local x0 = math.max(0, math.floor((point.x - radius) / chunk_width)) * self.chunk_size
	local z0 = math.max(0, math.floor((point.z - radius) / chunk_width)) * self.chunk_size
	local x1 = math.max(0, math.floor((point.x + radius) / chunk_width)) * self.chunk_size
	local z1 = math.max(0, math.floor((point.z + radius) / chunk_width)) * self.chunk_size
	return x0, z0, x1, z1
end

--- Maps a world space point to grid coordinates.
-- @param self ChunkManager.
-- @param x X coordinate in world units.
-- @param z Z coordinate in world units.
-- @return Grid coordinates.
ChunkManager.get_grid_xz_by_point = function(self, x, z)
	local x = math.floor(x / self.grid_size)
	local z = math.floor(z / self.grid_size)
	return x, z
end

return ChunkManager
