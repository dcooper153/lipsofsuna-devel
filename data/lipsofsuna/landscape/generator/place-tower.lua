--- Tower generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.place_tower
-- @type PlaceTower

local Class = require("system/class")
local MapUtils = require("core/utils/map")
local PlaceGenerator = require("landscape/generator/place-generator")
local TerrainChunk = require("system/terrain-chunk")

local yield = coroutine.yield

--- Tower generator.
-- @type PlaceTower
local PlaceTower = Class("PlaceTower", PlaceGenerator)

--- A helper function for creating stairs in a chunk.
--@param chk The chunk to modify.
--@param args.x1 X coordinate of the start tile.
--@param args.z1 Z coordinate of the start tile.
--@param args.y1 Y coordinate of the start's sruface.
--@param args.x2 X coordinate of the end tile, same as start if nil.
--@param args.z2 Z coordinate of the end tile, same as start if nil.
--@param args.y2 Y coordinate of the end's surface.
--@param args.x3 Each tile extends this amount in the X direction, 0 if nil.
--@param args.z3 Each tile extends this amount in the Z dirention, 0 if nil.
--@param args.y3 This is how far the step extends below the surface, 0.25 if nil.
--@param args.mat The material ID to use for the created sticks.
local placestair = function(chk, args)
	local sz = chk.chunk_size
	local xsize = (args.x2 or args.x1) - args.x1
	local ysize = (args.y2 or args.y1) - args.y1
	local zsize = (args.z2 or args.z1) - args.z1
	local x = args.x1
	local y = args.y1
	local z = args.z1
	local xcap = args.x3 or 0
	local ycap = args.y3 or 0.25
	local zcap = args.z3 or 0
	local largest = math.max(math.abs(xsize), math.abs(zsize))
	local xstep, ystep, zstep
	if xsize < 0 then 
		xstep = -1 
	elseif xsize > 0 then
		xstep = 1
	else
		xstep = 0
	end
	if zsize < 0 then 
		zstep = -1 
	elseif zsize > 0 then
		zstep = 1
	else
		zstep = 0
	end
	ystep = ysize / largest
	--todo: sanity check xstep and zstep
	--print("largest:" .. largest)
	for i = 0, largest do
		local xmin = math.min(x, x + xcap)
		local xmax = math.max(x, x + xcap)
		local zmin = math.min(z, z + zcap)
		local zmax = math.max(z, z + zcap)
		local ymin = math.min(y, y - ycap)
		local yh = math.abs(ycap)

		--print(xmin, zmin, xmax, zmax, ymin, yh, args.mat)
		chk:add_box(xmin, zmin, xmax, zmax, ymin, yh, args.mat)
		x = x + xstep
		y = y + ystep
		z = z + zstep
	end
end

--- Creates a new tower generator.
-- @param clss PlaceTower class.
-- @param generator TerrainGenerator.
-- @param planner WorldPlanner.
-- @return PlaceTower.
PlaceTower.new = function(clss, generator, planner)
	local self = PlaceGenerator.new(clss, generator, planner)
	self.__shapes =
	{
		["tower 1x1"] = {-0, -0, 1, 1},
		--["tower 2x2"] = {-1, -1, 2, 2},
		--["tower 3x3"] = {-2, -2, 3, 3}
	}
	return self
end

--- Checks whether the place fits in the given position.
-- @param self PlaceGenerator.
-- @param x Chunk X coordinate.
-- @param z Chunk Z coordinate.
-- @return Dictionary of places that fit.
PlaceTower.check = function(self, x, z)
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
PlaceTower.plan = function(self, x, z, place, params)
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
	local params = {5, y, math.random(3, 5)}
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
PlaceTower.generate = function(self, chunk, params)
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local wallspec = Main.specs:find_by_name("TerrainMaterialSpec", "brick")
	local wallmat = wallspec and wallspec.id
	local stairspec = Main.specs:find_by_name("TerrainMaterialSpec", "plank")
	local stairmat = stairspec and stairspec.id
	local floorspec = Main.specs:find_by_name("TerrainMaterialSpec", "plank")
	local floormat = floorspec and floorspec.id
	
	local chk = TerrainChunk(w)

	local base = math.floor(4 * (params[2] + 1)) * 0.25
	-- Create the floor.
	chk:add_box(0, 0, w-1, w-1, 0, base - 1, 3)
	chk:add_box(0, 0, w-1, w-1, base - 1, 1, wallmat)
	yield()

	local wallsteps = w - 2 - 3
	local h = base
	local floors = {}
	for floor = 1, params[3] do
		floors[floor] = h
		--west wall
		--print "west:"
		placestair(chk, {
				   x1 = w - 2,
				   z1 = w - 4,
				   z2 = 2,
				   x3 = -1,
				   z3 = -1,
				   y1 = h + 0.25,
				   y2 = h + 0.25 * wallsteps,
				   y3 = 0.5,
				   mat = stairmat
				})
		h = h + 0.25 * wallsteps
		yield()
		--south wall
		--print "south:"
		placestair(chk, {
				   x1 = w - 4,
				   z1 = 1,
				   x2 = 2,
				   x3 = -1,
				   z3 = 1,
				   y1 = h + 0.25,
				   y2 = h + 0.25 * wallsteps,
				   y3 = 0.5,
				   mat = stairmat
				})
		h = h + 0.25 * wallsteps
		yield()
		--east wall
		--print "east:"
		placestair(chk, {
				   x1 = 1,
				   z1 = 3,
				   z2 = w-3,
				   x3 = 1,
				   z3 = 1,
				   y1 = h + 0.25,
				   y2 = h + 0.25 * wallsteps,
				   y3 = 0.5,
				   mat = stairmat
				})
		h = h + 0.25 * wallsteps
		yield()
		--next floor
		chk:add_box(3, 3, w - 4, w - 2, h - 0.5, 0.5, floormat)
		--and landing for stair
		chk:add_box(w - 4, w - 4, w - 2, w - 2, h - 0.5, 0.5, floormat)
		yield()
	end	    

	-- Create the east wall.
	for z = 0,w-1 do
		chk:add_stick(0, z, base, h - base + 1 + (z % 2), wallmat)
	end
	yield()
	-- Create the west wall.
	for z = 0,w-1 do
		chk:add_stick(w-1, z, base, h - base + 1 + (z % 2), wallmat)
	end
	yield()
	-- Create the south wall.
	for x = 0,w-1 do
		chk:add_stick(x, 0, base, h - base + 1 + (x % 2), wallmat)
	end
	yield()
	-- Create the north wall.
	for x = 0,w-1 do
		chk:add_stick(x, w-1, base, h - base + 1 + (x % 2), wallmat)
	end
	
	--create door in north wall
	local door = math.floor(w/2)
	chk:add_box(door-1, w-1, door, w-1, base, 3 , 0)
	yield()


	--Create windows
	for floor,floorh in pairs(floors) do
		if floor > 1 and floor <= #floors then
			--north wall
			chk:add_box(3, w - 1, 4, w -1, floorh + 1, 2, 0)
			chk:add_box(w - 5, w - 1, w - 4, w -1, floorh + 1, 2, 0)
			--west wall
			chk:add_box(w - 1, 3, w - 1, 4, floorh + 1 + 1.25, 2, 0)
			chk:add_box(w - 1, w - 5, w - 1, w - 4, floorh + 1 + 0.5, 2, 0)
			if floor < #floors then
				--south wall
				chk:add_box(3, 0, 4, 0, floorh + 1 + 1.75 + 1.25, 2, 0)
				chk:add_box(w - 5, 0, w - 4, 0, floorh + 1 + 1.75 + 0.5, 2, 0)
				--east wall
				chk:add_box(0, 3, 0, 4, floorh + 1 + 3.5 + 0.5, 2, 0)
				chk:add_box(0, w - 5, 0, w - 4, floorh + 1 + 3.5 + 1.25, 2, 0)
			end
		end
	end

	-- Write the chunk.
	t:set_chunk(chunk.x, chunk.z, chk)
	yield()

	-- Add stuff
	floors[params[3] + 1] = h
	for floor,floorh in pairs(floors) do
		local r = math.random()
		if r < 0.5 then
			local p = Vector(chunk.x + w * 0.5, floorh, chunk.z + w * 0.5)
			local obj = MapUtils:place_obstacle_by_category("civilization", p, math.random() * math.pi * 2)
			--todo: sanity check the object can fit in a tower
		elseif r < 0.55 then
			local p = Vector(chunk.x + w * 0.5, floorh, chunk.z + w * 0.5)
			local obj = MapUtils:place_item_by_name("treasure chest", p, math.random() * math.pi * 2)
		end
	end
	yield()


	-- Create NPCs.
	-- TODO: Plan what NPCs to place in which sector.
	--local p = Vector(chunk.x + math.random(1, w-2), 0.0, chunk.z + math.random(1, w-2))
	--p:multiply(chunk.manager.grid_size):add_xyz(0, params[2], 0)
	--local obj = MapUtils:place_actor_by_category("tower", p, math.random() * math.pi * 2)
	--if obj then
	--	obj:set_important(true)
	--end
	--yield()
end

return PlaceTower
