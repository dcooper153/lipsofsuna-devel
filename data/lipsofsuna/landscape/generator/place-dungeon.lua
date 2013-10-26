--- Dungeon generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.dungeon_castle
-- @alias PlaceDungeon

local Class = require("system/class")
local PlaceDefault = require("landscape/generator/place-default")
local TerrainMaterialSpec = require("core/specs/terrain-material")

--- Dungeon generator.
-- @alias PlaceDungeon
local PlaceDungeon = Class("PlaceDungeon", PlaceDefault)

--- Creates a new dungeon generator.
-- @param clss PlaceDungeon class.
-- @param generator TerrainGenerator.
-- @param planner WorldPlanner.
-- @return PlaceDungeon.
PlaceDungeon.new = function(clss, generator, planner)
	local self = PlaceDefault.new(clss, generator, planner)
	self.__shapes =
	{
		["right"] = {-1, 0, 3, 1}
	}
	return self
end

--- Checks whether the place fits in the given position.
-- @param self PlaceGenerator.
-- @param x Chunk X coordinate.
-- @param z Chunk Z coordinate.
-- @return Dictionary of places that fit.
PlaceDungeon.check = function(self, x, z)
	local res = {}
	for name,shape in pairs(self.__shapes) do
		-- Check for intersections.
		local rx = shape[1] + x
		local rz = shape[2] + z
		local rw = shape[3]
		local rh = shape[4]
		if not self.__planner:intersects(rx-1, rz-1, rw+2, rh+2) then
			-- Check for mountain wall.
			local s = self.__generator.terrain.chunk_size
			local h00 = self.__planner:get_height(rx * s, rz * s)
			local h10 = self.__planner:get_height((rx+rw) * s, rz * s)
			local h01 = self.__planner:get_height(rx * s, (rz+rh) * s)
			local h11 = self.__planner:get_height((rx+rw) * s, (rz+rh) * s)
			local hmin = math.min(h00, h10, h01, h11)
			local hmax = math.max(h00, h10, h01, h11)
			-- TODO
			--if hmax - hmin <= 10 then
				res[name] = hmin
			--end
		end
	end
	return res
end

--- Plans the place.
-- @param self PlaceGenerator.
-- @param x Chunk X coordinate.
-- @param z Chunk Z coordinate.
-- @param place The type of the place as set by check().
-- @param params The parameters of the place as set by check().
-- @return True if planning succeded. False otherwise.
PlaceDungeon.plan = function(self, x, z, place, params)
	-- Get the place information.
	local shape = self.__shapes[place]
	local rx = shape[1] + x
	local rz = shape[2] + z
	local rw = shape[3]
	local rh = shape[4]
	-- Create the padding.
	for pz = rz-1,rz+rh do
		self.__planner:create_chunk(rx-1, pz)
		self.__planner:create_chunk(rx+rw, pz)
	end
	for px = rx,rx+rw-1 do
		self.__planner:create_chunk(px, rz-1)
		self.__planner:create_chunk(px, rz+rh)
	end
	-- Create the content.
	local params = {2, params, 10}
	for pz = rz,rz+rh-1 do
		for px = rx,rx+rw-1 do
			self.__planner:create_chunk(px, pz, params)
		end
	end
	return true
end

--- Generates a chunk of the place.
-- @param self PlaceGenerator.
-- @param chunk TerrainChunk.
-- @param params Place parameters as set by plan().
PlaceDungeon.generate = function(self, chunk, params)
	-- TODO
	self:generate_entrance(chunk, params)
end

--- Generates a dungeon entrance chunk.
-- @param self PlaceGenerator.
-- @param chunk TerrainChunk.
-- @param params Place parameters as set by plan().
PlaceDungeon.generate_entrance = function(self, chunk, params)
	-- TODO
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local ms = TerrainMaterialSpec:find_by_name("brick")
	local m = ms and ms.id
	-- Create the floor.
	for z = 0,w-1 do
		for x = 0,w-1 do
			t:add_stick(chunk.x + x, chunk.z + z, 0, params[2], 3)
			t:add_stick(chunk.x + x, chunk.z + z, params[2], 1, m)
		end
	end
end

--- Generates a dungeon interior chunk.
-- @param self PlaceGenerator.
-- @param chunk TerrainChunk.
-- @param params Place parameters as set by plan().
PlaceDungeon.generate_interior = function(self, chunk)
	-- Generate the surface normally.
	local surface = self.__planner:get_chunk_surface(chunk)
	self:generate_terrain(chunk, surface)
	self:generate_plants(chunk, surface)
	-- Calculate smooth floor vertices.
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local dt,dy1,dy2,dy3,dy4,dh1,dh2,dh3,dh4 = self.__planner:get_dungeon_type(chunk.manager, chunk.x, chunk.z)
	local dt1,dt2,dt3,dt4 = dy1+dh1,dy2+dh2,dy3+dh3,dy4+dh4
	local bot = {}
	local top = {}
	local s = w + 1
	for z = 0,w do
		local az = z/w
		local bl = dy3 * (1-az) + dy1 * az
		local br = dy4 * (1-az) + dy2 * az
		local tl = dt3 * (1-az) + dt1 * az
		local tr = dt4 * (1-az) + dt2 * az
		for x = 0,w do
			local ax = x/w
			bot[x + z * s] = bl * (1-ax) + br * ax
			top[x + z * s] = tl * (1-ax) + tr * ax
		end
	end
	-- Generate the floor.
	for z = 0,w-1 do
		for x = 0,w-1 do
			t:add_stick_corners(chunk.x + x, chunk.z + z,
				bot[x +  z    * s], bot[(x+1) +  z    * s],
				bot[x + (z+1) * s], bot[(x+1) + (z+1) * s],
				top[x +  z    * s], top[(x+1) +  z    * s],
				top[x + (z+1) * s], top[(x+1) + (z+1) * s],
				0)
		end
	end
end

return PlaceDungeon
