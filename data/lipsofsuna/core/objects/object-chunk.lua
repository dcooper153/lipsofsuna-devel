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
	self.time = Program:get_time()
	-- Create a sector loader.
	if Server.initialized then
		self.loader = ObjectChunkLoader(self, self.id)
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
	if self.loader then return end
	if not self.manager.database then return end
	if not Server.initialized then return end
	Server.object_database:save_sector_objects(self.id)
end

--- Updates the chunk.
-- @param self ObjectChunk.
-- @param secs Seconds since the last update.
ObjectChunk.update = function(self, secs)
	local t = Program:get_time()
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
