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
	local rows
	if self.manager.database then
		local rows = self.manager.database:query([[
			SELECT * FROM terrain_chunks
			WHERE id=?;]], {self.id})
		yield()
	end
	-- Create the chunk.
	if rows and #rows ~= 0 then
		-- Copy from the database query.
		self.manager.terrain:load_chunk(self.x, self.z)
		for k,v in ipairs(rows) do
			self.manager.terrain:set_chunk_data(self.x, self.z, v[2])
		end
	elseif self.manager.generate then
		-- Generate a random chunk.
		self.manager.terrain:load_chunk(self.x, self.z)
		self:generate_random()
		yield()
	else
		-- Create an empty chunk.
		self.manager.terrain:load_chunk(self.x, self.z)
	end
end

--- Forces the loader to run until finished.
-- @param self TerrainChunkLoader.
TerrainChunkLoader.finish = function(self)
	repeat until not self:update(1)
	self.routine = nil
end

--- Generates random terrain for the chunk.
-- @param self TerrainChunkLoader.
TerrainChunkLoader.generate_random = function(self)
	-- FIXME: Call the generator instead.
	local w = self.manager.chunk_size
	local t = self.manager.terrain
	local Noise = require("system/noise")
	for x = self.x,self.x+w-1 do
		for z = self.z,self.z+w-1 do
			local h1 = Noise:perlin_noise(Vector(x,z), Vector(0.1,0.1), 1, 3, 0.5, 2345235)
			local h2 = Noise:perlin_noise(Vector(555-x,344-z), Vector(0.2,0.2), 1, 3, 0.5, 43566)
			t:add_stick(x, z, 0, 490 + 9 + h1, 3)
			if h2 > 0 then
				local m = Noise:perlin_noise(Vector(234-x,435-z), Vector(0.1,0.1), 1, 3, 0.5, 1234)
				m = math.min(math.max(1, 4 * math.abs(m)), 3)
				t:add_stick(x, z, 490 + 9 + h1, 5 * h2, m)
			end
		end
	end
	for x = self.x,self.x+w-1 do
		for z = self.x,self.x+w-1 do
			t:smoothen_column(x, z, 0, 1000)
		end
	end
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
