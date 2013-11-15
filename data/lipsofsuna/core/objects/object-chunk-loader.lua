--- Loads object chunk data asynchronously.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.object_chunk_loader
-- @alias ObjectChunkLoader

local Class = require("system/class")

--- Loads object chunk data asynchronously.
-- @type ObjectChunkLoader
local ObjectChunkLoader = Class("ObjectChunkLoader")

--- Creates a new sector loader.
-- @param clss ObjectChunkLoader class.
-- @param manager ObjectChunkManager.
-- @param id Chunk ID.
-- @param x Chunk offset.
-- @param z Chunk offset.
-- @return ObjectChunkLoader.
ObjectChunkLoader.new = function(clss, manager, id, x, z)
	local self = Class.new(clss)
	self.manager = manager
	self.id = id
	self.x = x
	self.z = z
	self.routine = coroutine.create(function()
		self:execute(coroutine.yield)
	end)
	return self
end

--- Executes the loading process.<br/>
--
-- This can be called either synchronously or asynchronously by providing
-- the desired yield function. When run in a coroutine, coroutine.yield()
-- should be used. When running synchronously, a dummy function should be
-- used.
--
-- @param self ObjectChunkLoader.
-- @param yield Function.
ObjectChunkLoader.execute = function(self, yield)
	-- Load the data.
	if not self.manager.database then return end
	local rows = self.manager.database:query(
		[[SELECT b.id,b.type,b.spec,b.dead FROM
		object_sectors AS a INNER JOIN
		object_data AS b WHERE
		a.sector=? AND a.id=b.id]], {self.id})
	yield()
	-- Create the objects.
	local objects = {}
	for k,v in ipairs(rows) do
		local obj = Server.object_database:load_object(v[1], v[2], v[3], v[4])
		if obj then
			obj:set_visible(true)
			table.insert(objects, obj)
		end
	end
	yield()
	-- Trigger global events.
	Server.events:sector_created(self.id, terrain, objects)
	yield()
end

--- Forces the loader to run until finished.
-- @param self ObjectChunkLoader.
ObjectChunkLoader.finish = function(self)
	repeat until not self:update(1)
	self.routine = nil
end

--- Updates the loader.
-- @param self ObjectChunkLoader.
-- @param secs Seconds since the last update.
-- @return True if still loading, false if finished.
ObjectChunkLoader.update = function(self, secs)
	if not self.routine then return end
	local ret,err = coroutine.resume(self.routine, secs)
	if not ret then print(debug.traceback(self.routine, err)) end
	if coroutine.status(self.routine) == "dead" then
		self.routine = nil
		return
	end
	return true
end

return ObjectChunkLoader
