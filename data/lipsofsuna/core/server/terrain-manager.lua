--- Manages terrain chunks.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.terrain_manager
-- @alias TerrainManager

local Class = require("system/class")
local PhysicsTerrain = require("system/physics-terrain")
local Program = require("system/core")
local Terrain = require("system/terrain")
local TerrainChunk = require("core/server/terrain-chunk")
local TerrainChunkLoader = require("core/server/terrain-chunk-loader")

--- Manages terrain chunks.
-- @type TerrainManager
local TerrainManager = Class("TerrainManager")

--- Creates a new terrain manager.
-- @param clss TerrainManager class.
-- @param chunk_size Chunk size.
-- @param grid_size Grid size.
-- @param database Database, or nil.
-- @param unloading True to unable unloading.
-- @param generate True to enable generation of random terrain.
-- @param graphics True to enable graphics.
-- @return TerrainManager.
TerrainManager.new = function(clss, chunk_size, grid_size, database, unloading, generate, graphics)
	local self = Class.new(clss)
	self.database = database
	self.generate = generate
	self.graphics = graphics
	self.loaders = {}
	self.chunks = {}
	self.unload_time = unloading and 10
	self.chunk_size = chunk_size
	self.grid_size = grid_size
	self.terrain = Terrain(chunk_size, grid_size)
	self.physics = PhysicsTerrain(self.terrain)
	if Game then
		self.physics:set_collision_group(Game.PHYSICS_GROUP_TERRAIN)
		self.physics:set_collision_mask(Game.PHYSICS_MASK_TERRAIN)
	end
	self.physics:set_visible(true)
	-- Initialize the database tables needed by us.
	if self.database then
		self.database:query([[
			CREATE TABLE IF NOT EXISTS terrain_chunks
			(id INTEGER PRIMARY KEY,data BLOB);]]);
	end
	return self
end

--- Returns true if the chunk has finished loading.
-- @param self TerrainManager.
-- @param x X coordinate in grid units.
-- @param z Z coordinate in grid units.
TerrainManager.is_chunk_loaded = function(self, x, z)
	local id = self:get_chunk_id_by_xz(x, z)
	if not self.chunks[id] then return end
	if self.loaders[id] then return end
	return true
end

--- Reads a chunk from the database.
-- @param self TerrainManager.
-- @param x X coordinate in grid units.
-- @param z Z coordinate in grid units.
-- @return True if loaded, false if already loaded.
TerrainManager.load_chunk = function(self, x, z)
	-- Only load once.
	local id = self:get_chunk_id_by_xz(x, z)
	if self.chunks[id] then return end
	-- Create the chunk.
	self.chunks[id] = TerrainChunk(self, x, z)
	self.chunks_iterator = nil
	-- Create a chunk loader.
	if self.database or self.generate then
		self.loaders[id] = TerrainChunkLoader(self, id, x, z)
		self.loaders_iterator = nil
	end
	-- TODO: Handle client side terrain swapping.
	return true
end

--- Increases the timestamp of the chunks inside the given sphere.
-- @param self TerrainManager.
-- @param point Point in world space.
-- @param radius Radius in world units.
TerrainManager.refresh_point = function(self, point, radius)
	local chunk_width = self.chunk_size * self.grid_size
	local x0 = math.max(0, math.floor((point.x - radius) / chunk_width)) * self.chunk_size
	local z0 = math.max(0, math.floor((point.z - radius) / chunk_width)) * self.chunk_size
	local x1 = math.max(0, math.floor((point.x + radius) / chunk_width)) * self.chunk_size
	local z1 = math.max(0, math.floor((point.z + radius) / chunk_width)) * self.chunk_size
	for z = z0,z1 do
		for x = x0,x1 do
			if not self:load_chunk(x, z) then
				local id = self:get_chunk_id_by_xz(x, z)
				self.chunks[id].time = Program:get_time()
			end
		end
	end
end

--- Saves a chunk to the database.
-- @param self TerrainManager.
-- @param x X coordinate in grid units.
-- @param z Z coordinate in grid units.
TerrainManager.save_chunk = function(self, x, z)
	if not self.database then return end
	local id = self:get_chunk_id_by_xz(x, z)
	if self.loaders[id] then return end
	-- Write terrain.
	local data = self.terrain:get_chunk_data(x, z)
	self.database:query([[
		DELETE FROM terrain_chunks
		WHERE id=?;]], {id})
	self.database:query([[
		INSERT INTO terrain_chunks (id,data)
		VALUES (?,?);]], {id, data})
end

--- Saves all active chunks to the database.
-- @param self TerrainManager.
-- @param erase True to completely erase the old map.
TerrainManager.save_all = function(self, erase)
	if not self.database then return end
	self.database:query([[BEGIN TRANSACTION;]])
	-- Erase old world from the database.
	if erase then
		self.database:query([[DELETE FROM terrain_chunks;]])
	end
	-- Write each chunk.
	for id in pairs(self.chunks) do
		self:save_chunk(id)
	end
	self.database:query([[END TRANSACTION;]])
end

--- Unloads the world without saving.<br/>
-- @param self TerrainManager.
TerrainManager.unload_all = function(self)
	-- Unload the chunks.
	for id in pairs(self.chunks) do
		self.terrain:unload_chunk(self:get_chunk_xz_by_id(id))
	end
	-- Clear the dictionaries.
	self.loaders = {}
	self.loaders_iterator = nil
	self.chunks = {}
	self.chunks_iterator = nil
end

--- Unloads a chunk without saving.
-- @param self TerrainManager.
-- @param x X coordinate in grid units.
-- @param z Z coordinate in grid units.
TerrainManager.unload_chunk = function(self, x, z)
	-- Unload the chunk.
	local id = self:get_chunk_id_by_xz(x, z)
	if not self.chunks[id] then return end
	self.terrain:unload_chunk(x, z)
	-- Remove from the dictionaries.
	self.chunks[id] = nil
	self.chunks_iterator = nil
	self.loaders[id] = nil
	self.loaders_iterator = nil
end

--- Unloads chunks that have been inactive long enough.
-- @param self TerrainManager.
-- @param secs Seconds since the last update.
TerrainManager.update = function(self, secs)
	-- Update sector loaders.
	for i = 1,3 do
		local key,loader = next(self.loaders, self.loaders_iterator)
		self.loaders_iterator = key
		if loader and not loader:update(secs) then
			self.loaders[key] = nil
			if self.graphics then
				local chunk = self.chunks[key]
				if chunk then chunk:create_render_object() end
			end
		end
	end
	-- Update outdated models.
	if self.graphics and self.__view_center then
		local fx,fz = self:get_chunk_xz_by_point(self.__view_center.x, self.__view_center.z)
		local gx,gz = self.terrain:get_nearest_chunk_with_outdated_model(fx, fz)
		if gx then
			local id = self:get_chunk_id_by_xz(gx, gz)
			local chunk = self.chunks[id]
			if chunk and not self.loaders[id] then
				chunk:create_render_object()
			end
		end
	end
	-- TODO: Unload unused chunks.
	-- TODO: Remember chunk:detach_render_object().
end

--- Maps a world space point to a chunk ID.
-- @param self TerrainManager.
-- @param x X coordinate in world units.
-- @param z Z coordinate in world units.
-- @return Chunk ID.
TerrainManager.get_chunk_id_by_point = function(self, x, z)
	local chunk_width = self.chunk_size * self.grid_size
	local x = math.floor(x / chunk_width)
	local z = math.floor(z / chunk_width)
	return x + z * 0xFFFF
end

--- Maps grid coordinates to an internal chunk ID.
-- @param self TerrainManager.
-- @param x X coordinate in grid units.
-- @param z Z coordinate in grid units.
-- @return Chunk ID.
TerrainManager.get_chunk_id_by_xz = function(self, x, z)
	return math.floor(x / self.chunk_size) + math.floor(z / self.chunk_size) * 0xFFFF
end

--- Maps an internal chunk ID to grid coordinates.
-- @param self TerrainManager.
-- @param id Chunk ID.
-- @return Grid coordinates.
TerrainManager.get_chunk_xz_by_id = function(self, id)
	return (id % 0xFFFF) * self.chunk_size, math.floor(id / 0xFFFF) * self.chunk_size
end

--- Maps a world space point to grid coordinates.
-- @param self TerrainManager.
-- @param x X coordinate in world units.
-- @param z Z coordinate in world units.
-- @return Grid coordinates.
TerrainManager.get_chunk_xz_by_point = function(self, x, z)
	local chunk_width = self.chunk_size * self.grid_size
	local x = math.floor(x / chunk_width) * self.chunk_size
	local z = math.floor(z / chunk_width) * self.chunk_size
	return x, z
end

--- Sets the view center of the terrain manager.<br/>
--
-- This affects which chunks are prioritized when their models need rebuilding.
-- To allow model rebuilding at all, this must be called with a non-nil value.
--
-- @param self TerrainManager.
-- @param value Vector.
TerrainManager.set_view_center = function(self, value)
	self.__view_center = value
end

return TerrainManager
