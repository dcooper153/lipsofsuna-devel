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
	if self.manager.database then
		local rows = self.manager.database:query([[
			SELECT * FROM terrain_chunks
			WHERE id=?;]], {self.id})
		yield()
		if rows and #rows ~= 0 then
			self.manager.terrain:load_chunk(self.x, self.z)
			for k,v in ipairs(rows) do
				if self.manager.terrain:set_chunk_data(self.x, self.z, v[2]) then
					return
				end
			end
		end
	end
	-- Create new chunk contents.
	if self.manager.generate then
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
	local Noise = require("system/noise")
	local bh = 1700
	local get_height_at = function(x, z)
		-- Choose the bumpiness of the region.
		-- p=0.7: Very smooth.
		-- p=0.9: Very bumpy
		local r = Noise:plasma_noise_2d(0.01 * x, 0.01 * z, 2)
		local p = 0.75 + 0.15 * r
		-- Choose the height of the region.
		-- This is affected by both the position and the bumpiness.
		local n1 = Noise:harmonic_noise_2d(0.001 * x, 0.001 * z, 6, 1.3, p)
		-- Choose the soil layer height.
		-- This is affected by the height and the bumpiness.
		local s_base = 0.3 - 0.7 * (r + n1)
		local s_rand = Noise:harmonic_noise_2d(412 + 0.02 * x, 903 + 0.02 * z, 3, 1.3, 1 - 0.25 * (n1 + r))
		local n2 = math.max(0, 0.5 * (s_base + s_rand))
		-- Choose the grass layer height.
		-- This is mostly just random on any areas with soil.
		local g = Noise:plasma_noise_2d(0.03 * x, 0.03 * z, 3 - r)
		local n3 = math.max(0, g) * n2
		-- Return the combined heights.
		return bh + 100 * n1, 2 * n2, 0.5 * n3
	end
	local w = self.manager.chunk_size
	local t = self.manager.terrain
	for x = self.x,self.x+w-1 do
		for z = self.z,self.z+w-1 do
			local a00,b00,c00 = get_height_at(x, z)
			local a10,b10,c10 = get_height_at(x + 1, z)
			local a01,b01,c01 = get_height_at(x, z + 1)
			local a11,b11,c11 = get_height_at(x + 1, z + 1)
			if c00 > 0 or c10 > 0 or c01 > 0 or c11 > 0 then
				t:add_stick_corners(x, z, 0, 0, 0, 0, a00 + b00 + c00, a10 + b10 + c10, a01 + b01 + c01, a11 + b11 + c11, 2)
			end
			if b00 > 0 or b10 > 0 or b01 > 0 or b11 > 0 then
				t:add_stick_corners(x, z, 0, 0, 0, 0, a00 + b00, a10 + b10, a01 + b01, a11 + b11, 1)
			end
			t:add_stick_corners(x, z, 0, 0, 0, 0, a00, a10, a01, a11, 3)
		end
	end
	for x = self.x-1,self.x+w do
		for z = self.z-1,self.z+w do
			t:calculate_smooth_normals(x, z)
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
