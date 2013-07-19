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
local Hooks = require("system/hooks")
local PhysicsTerrain = require("system/physics-terrain")
local Packet = require("system/packet")
local Program = require("system/core")
local Terrain = require("system/terrain")
local TerrainChunk = require("core/terrain/terrain-chunk")
local TerrainChunkLoader = require("core/terrain/terrain-chunk-loader")
local TerrainMaterialSpec = require("core/specs/terrain-material")

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
	self.unload_time_model = 3
	self.chunk_size = chunk_size
	self.grid_size = grid_size
	self.generate_hooks = Hooks()
	self.__view_distance = 48
	self.terrain = Terrain(chunk_size, grid_size)
	-- Initialize the materials.
	for k,v in pairs(TerrainMaterialSpec.dict_id) do
		self.terrain:set_material_decoration_type(k, v.decoration_type)
		self.terrain:set_material_stick_type(k, v.stick_type)
		self.terrain:set_material_textures(k, v.top_texture, v.bottom_texture, v.side_texture, v.decoration_texture)
	end
	-- Initialize physics.
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

--- Returns true if the point has finished loading.
-- @param self TerrainManager.
-- @param point Point vector in world units.
TerrainManager.is_point_loaded = function(self, point)
	local id = self:get_chunk_id_by_point(point.x, point.z)
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
TerrainManager.refresh_chunks_by_point = function(self, point, radius, model)
	local x0,z0,x1,z1 = self:get_chunk_xz_range_by_point(point, radius)
	local t = Program:get_time()
	for z = z0,z1,self.chunk_size do
		for x = x0,x1,self.chunk_size do
			if not self:load_chunk(x, z) then
				local id = self:get_chunk_id_by_xz(x, z)
				self.chunks[id].time = t
			end
		end
	end
end

--- Increases the timestamp of the chunks inside the given sphere.
-- @param self TerrainManager.
-- @param point Point in world space.
-- @param radius Radius in world units.
TerrainManager.refresh_models_by_point = function(self, point, radius)
	local x0,z0,x1,z1 = self:get_chunk_xz_range_by_point(point, radius)
	local t = Program:get_time()
	for z = z0,z1,self.chunk_size do
		for x = x0,x1,self.chunk_size do
			local id = self:get_chunk_id_by_xz(x, z)
			local chunk = self.chunks[id]
			if chunk then
				chunk.time_model = t
			end
		end
	end
end

--- Registers a terrain chunk generator hook.
-- @param self TerrainManager.
-- @param priority Priority.
-- @param hook Hook.
TerrainManager.register_generate_hook = function(self, priority, hook)
	self.generate_hooks:register(priority, hook)
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
	local data = Packet(1)
	if not self.terrain:get_chunk_data(x, z, data) then
		print("ERROR: Could not save terrain")
	end
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
		local x,z = self:get_chunk_xz_by_id(id)
		self:save_chunk(x, z)
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
	-- Update chunk loaders.
	for i = 1,3 do
		local key,loader = next(self.loaders, self.loaders_iterator)
		self.loaders_iterator = key
		if loader and not loader:update(secs) then
			self.loaders[key] = nil
		end
	end
	-- Update chunk models.
	if self.graphics and self.__view_center then
		-- Mark chunks that require models.
		self:refresh_models_by_point(self.__view_center, self.__view_distance)
		-- Find the closest chunk that needs a model built.
		local fx,fz = self:get_chunk_xz_by_point(self.__view_center.x, self.__view_center.z)
		local gx,gz = self.terrain:get_nearest_chunk_with_outdated_model(fx, fz)
		-- Build the model of the chunk.
		if gx then
			local id = self:get_chunk_id_by_xz(gx, gz)
			local chunk = self.chunks[id]
			if chunk and not self.loaders[id] then
				if chunk.object or chunk.time_model then
					chunk:create_render_object()
				end
			end
		end
	end
	-- Unload unused chunks.
	local t = Program:get_time()
	for i = 1,8 do
		local key,chunk = next(self.chunks, self.chunks_iterator)
		self.chunks_iterator = key
		if chunk then
			if self.unload_time and t - chunk.time > self.unload_time then
				-- Save fully loaded chunks.
				local x,z = self:get_chunk_xz_by_id(key)
				if self.loaders[key] then
					self.loaders[key] = nil
					self.loaders_iterator = nil
				else
					self:save_chunk(x, z)
				end
				-- Detach the render object.
				if self.graphics then
					chunk:detach_render_object()
				end
				-- Unload the chunk data.
				self.terrain:unload_chunk(x, z)
				self.chunks[key] = nil
			elseif chunk.time_model and t - chunk.time_model >= self.unload_time_model then
				-- Detach the render object.
				local x,z = self:get_chunk_xz_by_id(key)
				chunk:detach_render_object()
				self.terrain:clear_chunk_model(x, z)
			end
		end
	end
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

--- Gets the XZ grid point range of chunks inside the given sphere.
-- @param self TerrainManager.
-- @param point Point in world space.
-- @param radius Radius in world units.
-- @return X min, Z min, X max, Z max
TerrainManager.get_chunk_xz_range_by_point = function(self, point, radius)
	local chunk_width = self.chunk_size * self.grid_size
	local x0 = math.max(0, math.floor((point.x - radius) / chunk_width)) * self.chunk_size
	local z0 = math.max(0, math.floor((point.z - radius) / chunk_width)) * self.chunk_size
	local x1 = math.max(0, math.floor((point.x + radius) / chunk_width)) * self.chunk_size
	local z1 = math.max(0, math.floor((point.z + radius) / chunk_width)) * self.chunk_size
	return x0, z0, x1, z1
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
