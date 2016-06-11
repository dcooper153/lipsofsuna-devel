--- House generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.place_house
-- @type PlaceHouse

local Class = require("system/class")
local MapUtils = require("core/utils/map")
local PlaceGenerator = require("landscape/generator/place-generator")
local TerrainChunk = require("system/terrain-chunk")

local yield = coroutine.yield

--- House generator.
-- @type PlaceHouse
local PlaceHouse = Class("PlaceHouse", PlaceGenerator)

--- Creates a new house generator.
-- @param clss PlaceHouse class.
-- @param generator TerrainGenerator.
-- @param planner WorldPlanner.
-- @return PlaceHouse.
PlaceHouse.new = function(clss, generator, planner)
	local self = PlaceGenerator.new(clss, generator, planner)
	self.__shapes =
	{
		["house 1x1"] = {-0, -0, 1, 1},
		--["house 2x2"] = {-1, -1, 2, 2},
		--["house 3x3"] = {-2, -2, 3, 3}
	}
	return self
end

--- Checks whether the place fits in the given position.
-- @param self PlaceGenerator.
-- @param x Chunk X coordinate.
-- @param z Chunk Z coordinate.
-- @return Dictionary of places that fit.
PlaceHouse.check = function(self, x, z)
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
PlaceHouse.plan = function(self, x, z, place, params)
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
	local params = {6, y, math.random(3, 5)}
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
PlaceHouse.generate = function(self, chunk, params)
	print "house"
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local wallspec = Main.specs:find_by_name("TerrainMaterialSpec", "brick")
	local wallmat = wallspec and wallspec.id
	local roofspec = Main.specs:find_by_name("TerrainMaterialSpec", "plank")
	local roofmat = roofspec and roofspec.id
	
	local chk = TerrainChunk(w)

	local base = math.floor(4 * (params[2] + 1)) * 0.25
	-- Create the floor.
	chk:add_box(0, 0, w-1, w-1, 0, base - 1, 3)
	chk:add_box(0, 0, w-1, w-1, base - 1, 1, wallmat)
	yield()

	local wallsteps = w - 2 - 3
	local h1 = base + 4
	local h2 = base + 4 + w / 4
	local hdiff = h2 - h1
	local h1b = h1 + hdiff / (w/2)

	chk:add_box_corners(
		0, 0, w / 2 - 1, w - 1,
		h1, h2, h1, h2,
		h1 + 0.5, h2 + 0.5, h1 + 0.5, h2 + 0.5,
		roofmat)
	yield()
	chk:add_box_corners(
		w / 2, 0, w - 1, w - 1,
		h2, h1, h2, h1,
		h2 + 0.5, h1 + 0.5, h2 + 0.5, h1 + 0.5,
		roofmat)
	yield()
	--east wall
	chk:add_box_corners(0, 1, 0, w - 2, 
			    base, base, base, base,
			    h1, h1b, h1, h1b,
			    wallmat)
	yield()
	--west wall
	chk:add_box_corners(w - 1, 1, w - 1, w - 2, 
			    base, base, base, base,
			    h1b, h1, h1b, h1,
			    wallmat)
	yield()
	--south wall
	chk:add_box_corners(0, 0, w / 2 - 1, 0, 
			    base, base, base, base,
			    h1, h2, h1, h2,
			    wallmat)
	yield()
	chk:add_box_corners(w / 2, 0, w - 1, 0, 
			    base, base, base, base,
			    h2, h1, h2, h1,
			    wallmat)
	yield()
	--north wall
	chk:add_box_corners(0, w - 1, w / 2 - 1, w - 1, 
			    base, base, base, base,
			    h1, h2, h1, h2,
			    wallmat)
	yield()
	chk:add_box_corners(w / 2, w - 1, w - 1, w - 1, 
			    base, base, base, base,
			    h2, h1, h2, h1,
			    wallmat)
	yield()

	--create door in north wall
	local door = math.floor(w/2)
	chk:add_box(door-1, w-1, door, w-1, base, 3 , 0)
	yield()

	--fireplace
	chk:add_box(0, 4, 2, 7, base, 8, wallmat)
	chk:add_box(1, 5, 1, 6, base, 8, 0)
	--Make the fireplace tall enough for the player to walk through, just in case they climb down the chimney.
	chk:add_box(1, 5, 2, 6, base, 2.25, 0)
	yield()

	-- Write the chunk.
	t:set_chunk(chunk.x, chunk.z, chk)
	yield()

	--Add some fire to the fireplace
	local p = Vector(chunk.x + 1.5, base, chunk.z + 5.5)
	MapUtils:place_item_by_name("torch", p, math.pi * 0.5)
	p = Vector(chunk.x + 1.5, base, chunk.z + 6.5)
	MapUtils:place_item_by_name("torch", p, math.pi * 1.5)

	--And some furniture to the room.
	p = Vector(chunk.x + 10.5, base, chunk.z + 10)
	MapUtils:place_item_by_name("bookcase", p, math.pi * 1)

	p = Vector(chunk.x + 6, base, chunk.z + 6)
	MapUtils:place_item_by_name("wood table", p, math.pi * 0)

	p = Vector(chunk.x + 3, base, chunk.z + 7.5)
	MapUtils:place_item_by_name("wood chair", p, math.pi * 0)
	p = Vector(chunk.x + 9, base, chunk.z + 7.5)
	MapUtils:place_item_by_name("wood chair", p, math.pi * 1)
	p = Vector(chunk.x + 3, base, chunk.z + 4.5)
	MapUtils:place_item_by_name("wood chair", p, math.pi * 0)
	p = Vector(chunk.x + 9, base, chunk.z + 4.5)
	MapUtils:place_item_by_name("wood chair", p, math.pi * 1)

	if math.random() < 0.5 then
		p = Vector(chunk.x + 10.5, base, chunk.z + 1.5)
		MapUtils:place_item_by_name("treasure chest", p, math.random() * math.pi * 2)
	end
end

return PlaceHouse
