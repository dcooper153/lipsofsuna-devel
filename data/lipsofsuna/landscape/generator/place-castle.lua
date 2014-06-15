--- Castle generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.place_castle
-- @type PlaceCastle

local Class = require("system/class")
local MapUtils = require("core/utils/map")
local PlaceGenerator = require("landscape/generator/place-generator")
local TerrainChunk = require("system/terrain-chunk")

local yield = coroutine.yield

--- Castle generator.
-- @type PlaceCastle
local PlaceCastle = Class("PlaceCastle", PlaceGenerator)

--- Creates a new castle generator.
-- @param clss PlaceCastle class.
-- @param generator TerrainGenerator.
-- @param planner WorldPlanner.
-- @return PlaceCastle.
PlaceCastle.new = function(clss, generator, planner)
	local self = PlaceGenerator.new(clss, generator, planner)
	self.__shapes =
	{
		["castle 2x4"] = {-1, -1, 2, 4},
		["castle 3x3"] = {-2, -2, 3, 3},
		["castle 4x2"] = {-2, -2, 4, 2}
	}
	return self
end

--- Checks whether the place fits in the given position.
-- @param self PlaceGenerator.
-- @param x Chunk X coordinate.
-- @param z Chunk Z coordinate.
-- @return Dictionary of places that fit.
PlaceCastle.check = function(self, x, z)
	local res = {}
	for name,shape in pairs(self.__shapes) do
		-- Check for intersections.
		local rx = shape[1] + x
		local rz = shape[2] + z
		local rw = shape[3]
		local rh = shape[4]
		if not self.__planner:intersects(rx-1, rz-1, rw+2, rh+2) then
			-- Check for flat terrain.
			local s = self.__generator.terrain.chunk_size
			local h00 = self.__planner:get_height(rx * s, rz * s)
			local h10 = self.__planner:get_height((rx+rw) * s, rz * s)
			local h01 = self.__planner:get_height(rx * s, (rz+rh) * s)
			local h11 = self.__planner:get_height((rx+rw) * s, (rz+rh) * s)
			local hmin = math.min(h00, h10, h01, h11)
			local hmax = math.max(h00, h10, h01, h11)
			if hmax - hmin <= 10 then
				res[name] = (hmin + hmax) / 2
			end
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
PlaceCastle.plan = function(self, x, z, place, params)
	-- Get the place information.
	local shape = self.__shapes[place]
	local y = params
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
	local params = {1, y, 10}
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
PlaceCastle.generate = function(self, chunk, params)
	-- TODO
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local ms = Main.specs:find_by_name("TerrainMaterialSpec", "brick")
	local m = ms and ms.id
	local chk = TerrainChunk(w)
	-- Create the floor.
	chk:add_box(0, 0, w-1, w-1, 0, params[2], 3)
	chk:add_box(0, 0, w-1, w-1, params[2], 1, m)
	yield()
	-- Create the west wall.
	local t1,p1 = self.__planner:get_chunk_type(chunk.x / w - 1, chunk.z / w)
	if t1 ~= "castle" then
		for z = 0,w-1 do
			chk:add_stick(0, z, params[2], params[3] + (z % 2), m)
		end
	end
	yield()
	-- Create the east wall.
	local t2,p2 = self.__planner:get_chunk_type(chunk.x / w + 1, chunk.z / w)
	if t2 ~= "castle"  then
		for z = 0,w-1 do
			chk:add_stick(w-1, z, params[2], params[3] + (z % 2), m)
		end
	end
	-- Create the south wall.
	local t3,p3 = self.__planner:get_chunk_type(chunk.x / w, chunk.z / w - 1)
	if t3 ~= "castle"  then
		for x = 0,w-1 do
			chk:add_stick(x, 0, params[2], params[3] + (x % 2), m)
		end
	end
	yield()
	-- Create the north wall.
	local t4,p4 = self.__planner:get_chunk_type(chunk.x / w, chunk.z / w + 1)
	if t4 ~= "castle"  then
		for x = 0,w-1 do
			chk:add_stick(x, w-1, params[2], params[3] + (x % 2), m)
		end
		local door = math.floor(w/2)
		chk:add_box(door, w-1, door+1, w-1, params[2] + 1, params[3]/2, 0)
	end
	yield()
	-- Write the chunk.
	t:set_chunk(chunk.x, chunk.z, chk)
	yield()
	-- Create NPCs.
	-- TODO: Plan what NPCs to place in which sector.
	local p = Vector(chunk.x + math.random(1, w-2), 0.0, chunk.z + math.random(1, w-2))
	p:multiply(chunk.manager.grid_size):add_xyz(0, params[2], 0)
	local obj = MapUtils:place_actor_by_category("castle", p, math.random() * math.pi * 2)
	if obj then
		obj:set_important(true)
	end
	yield()
end

return PlaceCastle
