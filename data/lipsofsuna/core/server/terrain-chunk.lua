--- Stores information on a terrain chunk.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.terrain_chunk
-- @alias TerrainChunk

local Class = require("system/class")
local Program = require("system/core")
local Vector = require("system/math/vector")

--- Manages terrain chunks.
-- @type TerrainChunk
local TerrainChunk = Class("TerrainChunk")

--- Creates a new terrain chunk.
-- @param clss TerrainChunk class.
-- @param manager TerrainManager.
-- @return TerrainChunk.
TerrainChunk.new = function(clss, manager, x, z)
	local self = Class.new(clss)
	self.manager = manager
	self.x = x
	self.z = z
	self.time = Program:get_time()
	return self
end

--- Creates the render object for the chunk.
-- @param self TerrainChunk.
TerrainChunk.create_render_object = function(self)
	-- Create the object.
	local size = self.manager.grid_size
	if not self.object then
		local RenderObject = require("system/render-object")
		self.object = RenderObject()
	end
	-- Create the model.
	local prev_model = self.model
	self.model = self.manager.terrain:build_chunk_model(self.x, self.z)
	-- Assign the model.
	if prev_model then
		self.object:replace_model(prev_model:get_render(), self.model:get_render())
	else
		self.object:add_model(self.model:get_render())
	end
	self.object:set_position(Vector(self.x * size, 0.0, self.z * size))
	self.object:set_visible(true)
end

--- Detaches the render object from the scene.
-- @param self TerrainChunk.
TerrainChunk.detach_render_object = function(self)
	if self.object then
		self.object:set_visible(false)
		self.object = nil
	end
	self.model = nil
end

return TerrainChunk
