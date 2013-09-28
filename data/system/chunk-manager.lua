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
-- @param create_func Function for creating chunks.
-- @return ChunkManager.
ChunkManager.new = function(clss, chunk_size, grid_size, create_func)
	local self = Class.new(clss)
	self.chunk_size = chunk_size
	self.grid_size = grid_size
	self.__create = create_func
	self.chunks = {}
	return self
end

--- Loads a chunk.
-- @param self ChunkManager.
-- @param x X coordinate in grid units.
-- @param z Z coordinate in grid units.
-- @return True if loaded, false if already loaded.
ChunkManager.load_chunk = function(self, x, z)
	-- Only load once.
	local id = self:get_chunk_id_by_xz(x, z)
	if self.chunks[id] then return end
	-- Create the chunk.
	self.chunks[id] = self:__create(x, z)
	self.chunks_iterator = nil
	return true
end

--- Loads or reloads a chunk.
-- @param self ChunkManager.
-- @param x X coordinate in grid units.
-- @param z Z coordinate in grid units.
ChunkManager.reload_chunk = function(self, x, z)
	local id = self:get_chunk_id_by_xz(x, z)
	local chunk = self.chunks[id]
	if chunk then chunk:detach() end
	self.chunks[id] = self:__create(x, z)
	self.chunks_iterator = nil
end

--- Unloads a chunk.
-- @param self ChunkManager.
-- @param x X coordinate in grid units.
-- @param z Z coordinate in grid units.
ChunkManager.unload_chunk = function(self, x, z)
	-- Unload the chunk.
	local id = self:get_chunk_id_by_xz(x, z)
	local chunk = self.chunks[id]
	if not chunk then return end
	chunk:detach()
	-- Remove from the dictionary.
	self.chunks[id] = nil
	self.chunks_iterator = nil
end

--- Unloads all the chunks.
-- @param self ChunkManager.
ChunkManager.unload_all_chunks = function(self)
	-- Unload the chunks.
	for id,chunk in pairs(self.chunks) do
		chunk:detach()
	end
	-- Clear the dictionaries.
	self.chunks = {}
	self.chunks_iterator = nil
end

--- Updates the state of the chunks.
-- @param self ChunkManager.
-- @param secs Seconds since the last update.
ChunkManager.update = function(self, secs)
	for i = 1,5 do
		local key,chunk = next(self.chunks, self.chunks_iterator)
		self.chunks_iterator = key
		if chunk then
			chunk:update(secs)
		end
	end
end

--- Returns true if the chunk has finished loading.
-- @param self ChunkManager.
-- @param x X coordinate in grid units.
-- @param z Z coordinate in grid units.
ChunkManager.is_chunk_loaded = function(self, x, z)
	local id = self:get_chunk_id_by_xz(x, z)
	local chunk = self.chunks[id]
	if not chunk then return end
	if chunk.loader then return end
	return true
end

--- Returns true if the point has finished loading.
-- @param self ChunkManager.
-- @param point Point vector in world units.
ChunkManager.is_point_loaded = function(self, point)
	local id = self:get_chunk_id_by_point(point.x, point.z)
	local chunk = self.chunks[id]
	if not chunk then return end
	if chunk.loader then return end
	return true
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
