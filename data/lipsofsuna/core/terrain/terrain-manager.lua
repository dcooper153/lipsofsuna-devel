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
local PhysicsTerrain = require("system/physics-terrain")
local PhysicsConsts = require("core/physics/consts")
local Program = require("system/core")
local Terrain = require("system/terrain")
local TerrainChunk = require("core/terrain/terrain-chunk")
local TerrainMaterialSpec = require("core/specs/terrain-material")
local Time = require("system/time")
local Image = require("system/image")
local Render = require("system/render")
local Texture = require("system/texture")

--- Manages terrain chunks.
-- @type TerrainManager
local TerrainManager = Class("TerrainManager", ChunkManager)

--- Creates a new terrain manager.
-- @param clss TerrainManager class.
-- @param chunk_size Chunk size.
-- @param grid_size Grid size.
-- @param database Database, or nil.
-- @param unloading True to unable unloading.
-- @param graphics True to enable graphics.
-- @return TerrainManager.
TerrainManager.new = function(clss, chunk_size, grid_size, database, unloading, graphics)
	local create = function(m, x, z) return TerrainChunk(m, x, z) end
	local self = ChunkManager.new(clss, chunk_size, grid_size, create)
	self.database = database
	self.graphics = graphics
	self.unload_time = unloading and 10
	self.unload_time_model = 3
	self.__view_distance = 48
	self.__load_priorities = {}
	self.terrain = Terrain(chunk_size, grid_size)
	self.texture = Texture("stickterrain1")
	self.image_size = 1024
	self.image_tile_size = 128
	self.image = Image(self.image_size, self.image_size)
	self.image:fill(255, 255, 255, 0)
	self.image_tile_count = self.image_size / self.image_tile_size
	self.image_tile_total = self.image_tile_count * self.image_tile_count
	self.material_tile_next = 0
	self.material_tile_by_name = {}
	-- Initialize the materials.
	for k,v in pairs(TerrainMaterialSpec.dict_id) do
		self:add_terrain_material(k, v)
	end
	-- Initialize physics.
	self.physics = PhysicsTerrain(self.terrain)
	self.physics:set_collision_group(PhysicsConsts.GROUP_TERRAIN)
	self.physics:set_collision_mask(PhysicsConsts.MASK_TERRAIN)
	self.physics:set_visible(true)
	self.physics:set_friction(0.5)
	self.physics:set_unloaded_collision(true)
	-- Initialize the database tables needed by us.
	if self.database then
		self.database:query([[
			CREATE TABLE IF NOT EXISTS terrain_chunks
			(id INTEGER PRIMARY KEY,data BLOB);]]);
	end
	return self
end

TerrainManager.add_terrain_material_texture = function(self, name)
	local tile = self.material_tile_by_name[name]
	if tile then
		return tile
	end
	tile = self.material_tile_next
	self.material_tile_by_name[name] = tile
	self.material_tile_next = self.material_tile_next + 1

	--print("Terrain texture '" .. name .. "' to tile " .. tile)

	local img = Image(name .. ".png")
	local tx = tile % self.image_tile_count
	local ty = math.floor(tile / self.image_tile_count)
	local x = self.image_tile_size * tx
	local y = self.image_tile_size * ty
	if img then
		self.image:blit_rect(img, x, y, img:get_width(), img:get_height(), 0, 0)
	else
		print ("No image found for:" .. name)
	end
	return tile
end

TerrainManager.add_terrain_material = function(self, k, v)
	--print("TerrainManager: ", k, v)
	self.terrain:set_material_decoration_type(k, v.decoration_type)
	self.terrain:set_material_stick_type(k, v.stick_type)
	local t_all = v.texture and self:add_terrain_material_texture(v.texture)
	local t_top = t_all
	local t_side = t_all
	local t_bottom = t_all
	--todo: Sanity check all faces are set.
	--todo: Sanity check the case of t_all being set but all other faces have different faces (i.e. t_all is redundant).
	local t_decoration = v.texture_decoration and self:add_terrain_material_texture(v.texture_decoration)
	self.terrain:set_material_textures(k, t_top, t_bottom, t_side, t_decoration)
	self.texture:set_image(self.image)
end

--- Increases the timestamp of the chunks inside the given sphere.
-- @param self TerrainManager.
-- @param point Point in world space.
-- @param radius Radius in world units.
TerrainManager.refresh_chunks_by_point = function(self, point, radius)
	local xc,zc = self:get_chunk_xz_by_point(point.x, point.z)
	local x0,z0,x1,z1 = self:get_chunk_xz_range_by_point(point, radius)
	local t = Time:get_secs()
	for z = z0,z1,self.chunk_size do
		for x = x0,x1,self.chunk_size do
			-- Load the chunk.
			local id = self:get_chunk_id_by_xz(x, z)
			if not self:load_chunk(x, z) then
				self.chunks[id].time = t
			end
			-- Set the loading priority.
			local c = self.chunks[id]
			if c and c.loader then
				local p = math.sqrt((x-xc)^2 + math.abs(z-zc)^2)
				table.insert(self.__load_priorities, {p, id})
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
	local t = Time:get_secs()
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

--- Saves all active chunks to the database.
-- @param self TerrainManager.
-- @param db The database to save to, will fall back to internal database if nil.
-- @param erase True to completely erase the old map.
TerrainManager.save_all = function(self, db, erase)
	local database = db or self.database
	if not database then return end
	database:query([[BEGIN TRANSACTION;]])
	-- Erase old world from the database.
	if erase then
		database:query([[DELETE FROM terrain_chunks;]])
	end
	-- Write each chunk.
	for id,chunk in pairs(self.chunks) do
		chunk:save()
	end
	database:query([[END TRANSACTION;]])
end

--- Updates the state of the chunks.
-- @param self TerrainManager.
-- @param secs Seconds since the last update.
TerrainManager.update = function(self, secs)
	-- Load chunks on demand.
	Main.timing:start_action("terrainL")
	table.sort(self.__load_priorities, function(a, b) return a[1] < b[1] end)
	for k,v in ipairs(self.__load_priorities) do
		local c = self.chunks[v[2]]
		if c then c:load(secs) end
		if v[1] > 0 and k > 3 then break end
	end
	self.__load_priorities = {}
	-- Unload chunks on demand.
	Main.timing:start_action("terrainC")
	ChunkManager.update(self, secs)
	-- Build chunk models on demand.
	if self.graphics and self.__view_center then
		Main.timing:start_action("terrainG")
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

--- Gets the terrain chunk generator function.
-- @param self TerrainManager.
-- @return Function or nil.
TerrainManager.get_chunk_generator = function(self)
	return self.generate
end

--- Sets the terrain chunk generator function.
-- @param self TerrainManager.
-- @param func Function or nil.
TerrainManager.set_chunk_generator = function(self, func)
	self.generate = func
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
