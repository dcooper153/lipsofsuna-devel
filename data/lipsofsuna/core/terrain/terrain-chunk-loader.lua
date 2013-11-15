--- Implements asynchronous terrain chunk loading.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.terrain_chunk_loader
-- @alias TerrainChunkLoader

local Class = require("system/class")

--- Implements asynchronous terrain chunk loading.
-- @type TerrainChunkLoader
local TerrainChunkLoader = Class("TerrainChunkLoader")

--- Creates a new terrain chunk loader.
-- @param clss TerrainChunkLoader class.
-- @param manager TerrainManager.
-- @param id Chunk ID.
-- @param x Chunk offset.
-- @param z Chunk offset.
-- @return TerrainChunkLoader.
TerrainChunkLoader.new = function(clss, manager, id, x, z)
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
-- @param clss TerrainChunkLoader class.
-- @param yield Function.
TerrainChunkLoader.execute = function(self, yield)
	-- Read the chunk from the database.
	local loaded
	if self.manager.database then
		local rows = self.manager.database:query([[
			SELECT * FROM terrain_chunks
			WHERE id=?;]], {self.id})
		yield()
		if rows and #rows ~= 0 then
			self.manager.terrain:load_chunk(self.x, self.z)
			for k,v in ipairs(rows) do
				local data = v[2]
				data:read()
				if self.manager.terrain:set_chunk_data(self.x, self.z, data) then
					loaded = true
					break
				end
			end
		end
	end
	-- Create new chunk contents.
	if not loaded then
		if self.manager.generate then
			-- Generate a random chunk.
			self.manager.terrain:load_chunk(self.x, self.z)
			self.manager.generate_hooks:call(self)
			yield()
		else
			-- Create an empty chunk.
			self.manager.terrain:load_chunk(self.x, self.z)
		end
	end
	-- Chain load the object chunk.
	if Main.objects then
		local p = self.manager:get_chunk_center_by_xz(self.x, self.z)
		local id = Main.objects.chunks:get_chunk_id_by_point(p.x, p.z)
		Main.objects:load_chunk(id)
	end
end

--- Forces the loader to run until finished.
-- @param self TerrainChunkLoader.
TerrainChunkLoader.finish = function(self)
	repeat until not self:update(1)
	self.routine = nil
end

--- Updates the sector loader.
-- @param self TerrainChunkLoader.
-- @param secs Seconds since the last update.
-- @return True if still loading, false if finished.
TerrainChunkLoader.update = function(self, secs)
	if not self.routine then return end
	local ret,err = coroutine.resume(self.routine, secs)
	if not ret then print(debug.traceback(self.routine, err)) end
	if coroutine.status(self.routine) == "dead" then
		self.routine = nil
		return
	end
	return true
end

return TerrainChunkLoader
