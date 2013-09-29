--- Implements partitioning and swapping of map objects.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.object_chunk_manager
-- @alias ObjectChunkManager

local Class = require("system/class")
local ChunkManager = require("system/chunk-manager")
local ObjectChunk = require("core/objects/object-chunk")
local ObjectChunkLoader = require("core/objects/object-chunk-loader")
local Program = require("system/core")

--- Implements partitioning and swapping of map objects.
-- @type ObjectChunkManager
local ObjectChunkManager = Class("ObjectChunkManager", ChunkManager)

--- Creates a new sector manager.
-- @param clss ObjectChunkManager class.
-- @param manager Object manager.
-- @param chunk_size Chunk size.
-- @param grid_size Grid size.
-- @return ObjectChunkManager.
ObjectChunkManager.new = function(clss, manager, chunk_size, grid_size)
	local create = function(m, x, z) return ObjectChunk(m, x, z) end
	local self = ChunkManager.new(clss, chunk_size, grid_size, create)
	self.manager = manager
	return self
end

--- Increases the timestamp of the sectors inside the given sphere.
-- @param self ObjectChunkManager.
-- @param point Position vector, in world units.
-- @param radius Refresh radius, in world units.
ObjectChunkManager.refresh = function(self, point, radius)
	local x0,z0,x1,z1 = self:get_chunk_xz_range_by_point(point, radius or 10)
	for x = x0,x1 do
		for z = z0,z1 do
			local id = self:get_chunk_id_by_xz(x, z)
			local chunk = self.chunks[id]
			if not chunk then
				self:load_chunk(x, z)
			else
				chunk.time = Program:get_time()
			end
		end
	end
end

--- Saves all active sectors to the database.
-- @param self ObjectChunkManager.
ObjectChunkManager.save_world = function(self)
	if not self.database then return end
	self.database:query("BEGIN TRANSACTION;")
	for k,v in pairs(self.chunks) do
		v:save()
	end
	self.database:query("END TRANSACTION;")
end

--- Waits for a sector to finish loading.
-- @param self ObjectChunkManager.
-- @param id Chunk ID.
ObjectChunkManager.wait_sector_load = function(self, id)
	local chunk = self.chunks[sector]
	if not chunk then return end
	if not chunk.loader then return end
	chunk.loader:finish()
	chunk.loader = nil
end

--- Gets the idle time of the chunk.
-- @param self ObjectChunkManager.
-- @param id Chunk ID.
-- @return Idle time in seconds.
ObjectChunkManager.get_sector_idle = function(self, id)
	local chunk = self.chunks[sector]
	if not chunk then return end
	return Program:get_time() - chunk.time
end

return ObjectChunkManager
