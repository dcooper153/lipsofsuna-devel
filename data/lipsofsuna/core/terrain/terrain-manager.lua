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
local ChunkManager = require("system/chunk-manager")
local Hooks = require("system/hooks")
local PhysicsTerrain = require("system/physics-terrain")
local Packet = require("system/packet")
local PhysicsConsts = require("core/server/physics-consts")
local Program = require("system/core")
local Terrain = require("system/terrain")
local TerrainChunk = require("core/terrain/terrain-chunk")
local TerrainMaterialSpec = require("core/specs/terrain-material")

--- Manages terrain chunks.
-- @type TerrainManager
local TerrainManager = Class("TerrainManager", ChunkManager)

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
	local create = function(m, x, z) return TerrainChunk(m, x, z) end
	local self = ChunkManager.new(clss, chunk_size, grid_size, create)
	self.database = database
	self.generate = generate
	self.graphics = graphics
	self.unload_time = unloading and 10
	self.unload_time_model = 3
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
	self.physics:set_collision_group(PhysicsConsts.GROUP_TERRAIN)
	self.physics:set_collision_mask(PhysicsConsts.MASK_TERRAIN)
	self.physics:set_visible(true)
	self.physics:set_friction(1.2)
	self.physics:set_unloaded_collision(true)
	-- Initialize the database tables needed by us.
	if self.database then
		self.database:query([[
			CREATE TABLE IF NOT EXISTS terrain_chunks
			(id INTEGER PRIMARY KEY,data BLOB);]]);
	end
	return self
end

--- Increases the timestamp of the chunks inside the given sphere.
-- @param self TerrainManager.
-- @param point Point in world space.
-- @param radius Radius in world units.
TerrainManager.refresh_chunks_by_point = function(self, point, radius)
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
	-- Skip chunks that are still loading.
	local chunk = self.chunks[id]
	if not chunk or chunk.loader then return end
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
	for id,chunk in pairs(self.chunks) do
		self:save_chunk(chunk.x, chunk.z)
	end
	self.database:query([[END TRANSACTION;]])
end

--- Unloads the world without saving.<br/>
-- @param self TerrainManager.
TerrainManager.unload_all = function(self)
	-- Unload the chunks.
	for id,chunk in pairs(self.chunks) do
		if self.graphics then
			chunk:detach_render_object()
		end
		self.terrain:unload_chunk(self:get_chunk_xz_by_id(id))
	end
	-- Clear the dictionaries.
	self.chunks = {}
	self.chunks_iterator = nil
end

--- Updates the state of the chunks.
-- @param self TerrainManager.
-- @param secs Seconds since the last update.
TerrainManager.update = function(self, secs)
	-- Load and unload chunks on demand.
	ChunkManager.update(self, secs)
	-- Build chunk models on demand.
	if self.graphics and self.__view_center then
		-- Find the closest chunk that needs a model built.
		self:refresh_models_by_point(self.__view_center, self.__view_distance)
		local fx,fz = self:get_chunk_xz_by_point(self.__view_center.x, self.__view_center.z)
		local gx,gz = self.terrain:get_nearest_chunk_with_outdated_model(fx, fz)
		if not gx then return end
		-- Build the model of the chunk.
		local id = self:get_chunk_id_by_xz(gx, gz)
		local chunk = self.chunks[id]
		if chunk and not chunk.loader then
			if chunk.object or chunk.time_model then
				chunk:create_render_object()
			end
		end
	end
end

--- Enables or disables terrain generation.
-- @param self TerrainManager.
-- @param value True to enable. False otherwise.
TerrainManager.set_enable_generation = function(self, value)
	self.generate = true
end

--- Enables or disables terrain graphics.
-- @param self TerrainManager.
-- @param value True to enable. False otherwise.
TerrainManager.set_enable_graphics = function(self, value)
	self.graphics = true
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
