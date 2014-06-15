--- Stores information on an object chunk.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.terrain_chunk
-- @alias ObjectChunk

local Class = require("system/class")
local Program = require("system/core")
local ObjectChunkLoader = require("core/objects/object-chunk-loader")
local Time = require("system/time")

--- Stores information on an object chunk.
-- @type ObjectChunk
local ObjectChunk = Class("ObjectChunk")

--- Creates a new terrain chunk.
-- @param clss ObjectChunk class.
-- @param manager ObjectManager.
-- @return ObjectChunk.
ObjectChunk.new = function(clss, manager, x, z)
	local self = Class.new(clss)
	self.manager = manager
	self.x = x
	self.z = z
	self.id = manager:get_chunk_id_by_xz(x, z)
	self.time = Time:get_secs()
	-- Create a sector loader.
	if Server.initialized then
		self.loader = ObjectChunkLoader(manager, self.id, x, z)
	end
	return self
end

--- Detaches the chunk from the scene.
-- @param self ObjectChunk.
ObjectChunk.detach = function(self)
	for k,v in pairs(self.manager.manager:find_by_sector(self.id)) do
		v:detach()
	end
end

--- Saves the chunk.
-- @param self ObjectChunk.
ObjectChunk.save = function(self)
	-- Check if saving is enabled.
	if self.loader then return end
	local db = self.manager.database
	if not db then return end
	-- Delete old objects.
	db:query([[DELETE FROM object_sectors WHERE sector=?;]], {self.id})
	-- Write new objects.
	local objs = self.manager.manager:find_by_sector(self.id)
	for k,object in pairs(objs) do
		if object.serializer then
			object.serializer:write(object, db)
		end
	end
end

--- Updates the chunk.
-- @param self ObjectChunk.
-- @param secs Seconds since the last update.
ObjectChunk.update = function(self, secs)
	local t = Time:get_secs()
	-- Unload unused chunks.
	if self.manager.unload_time and t - self.time > self.manager.unload_time then
		self:save()
		self:detach()
		self.manager.chunks[self.id] = nil
		return
	end
	-- Progess chunk loading.
	if self.loader then
		if not self.loader:update(secs) then
			self.loader = nil
		end
		return
	end
end

return ObjectChunk
