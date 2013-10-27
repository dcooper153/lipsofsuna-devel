--- Dungeon generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.dungeon_castle
-- @alias PlaceDungeon

local Bitwise = require("system/bitwise")
local Class = require("system/class")
local PlaceDefault = require("landscape/generator/place-default")
local Noise = require("system/noise")
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
		["dungeon"] = {-2, 2, 5, 5}
	}
	return self
end

--- Checks whether the place fits in the given position.
-- @param self PlaceDungeon.
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
		if not self.__planner:intersects(rx, rz, rw, rh) then
			-- Check for mountain wall.
			local fx,fz = x + 0.5, z + 0.5
			local s = self.__generator.terrain.chunk_size
			local h00 = self.__planner:get_height(x * s, z * s)
			local h10 = self.__planner:get_height((x+1) * s, z * s)
			local h01 = self.__planner:get_height(x * s, (z+1) * s)
			local h11 = self.__planner:get_height((x+1) * s, (z+1) * s)
			local h_min = math.min(h00, h10, h01, h11)
			local h = self.__planner:get_height((x-1) * s, (z-1) * s)
			local h_xm = self.__planner:get_height((fx-1) * s, fz * s)
			local h_xp = self.__planner:get_height((fx+1) * s, fz * s)
			local h_zm = self.__planner:get_height(fx * s, (fz-1) * s)
			local h_zp = self.__planner:get_height(fx * s, (fz+1) * s)
			local h_max = math.max(h_xm, h_xp, h_zm, h_zp)
			if h_min + 8 < h_max then
				res[name] = h_min
			end
		end
	end
	return res
end

--- Plans the place.
-- @param self PlaceDungeon.
-- @param x Chunk X coordinate.
-- @param z Chunk Z coordinate.
-- @param place The type of the place as set by check().
-- @param params The parameters of the place as set by check().
-- @return True if planning succeded. False otherwise.
PlaceDungeon.plan = function(self, x, z, place, params)
	-- Get the place information.
	local w = self.__generator.terrain.chunk_size
	local seeds = self.__generator.seeds
	local shape = self.__shapes[place]
	local rx = shape[1] + x
	local rz = shape[2] + z
	local rw = shape[3]
	local rh = shape[4]
	-- Drunkard's walk algorithm with branching.
	local dirs_dx = {-1,1,0,0}
	local dirs_dz = {0,0,-1,1}
	local dirs_inv = {2,1,4,3}
	local waypoints = {}
	local waypoints_num = 0
	local check_waypoint = function(x, z, wp, h)
		local t,p = self.__planner:get_chunk_type(x, z, false)
		if t then return end
		-- Check that there's enough room.
		local min_y = wp.y - 6
		local max_y = self.__planner:get_height((x+0.5)*w, (z+0.5)*w) - h - 7
		if wp.type == "entrance" then
			min_y = wp.y - 1
			max_y = math.min(max_y, wp.y)
		end
		if min_y > max_y then return end
		-- Return the y range.
		return min_y, max_y
	end
	local create_waypoint = function(type, x, z, y, h)
		local new = {x = x, z = z, y = y, h = h, dirs = {1,2,3,4}, conn = {}, type = type}
		table.insert(waypoints, new)
		waypoints_num = waypoints_num + 1
		return new
	end
	local delete_waypoint = function(i)
		table.remove(waypoints, i)
		waypoints_num = waypoints_num - 1
	end
	local connect_waypoint = function(wp, dir)
		table.insert(wp.conn, dir)
		local mask = 0
		for k,v in pairs(wp.conn) do
			mask = mask + 2^(v-1)
		end
		local _,p = self.__planner:get_chunk_type(wp.x, wp.z, false)
		self.__planner:create_chunk(wp.x, wp.z, {p and p[1] or 2, wp.y, wp.h, mask})
	end
	local expand_waypoint = function(i, wp)
		local conn = wp.conn
		local num = #wp.dirs
		while num > 0 do
			-- Choose the direction.
			local rnd = math.random(1, num)
			local dir = wp.dirs[rnd]
			table.remove(wp.dirs, rnd)
			num = num - 1
			-- Check if the destination is valid.
			local h = 5
			local next_x = wp.x + dirs_dx[dir]
			local next_z = wp.z + dirs_dz[dir]
			local min_y,max_y = check_waypoint(next_x, next_z, wp, h)
			if min_y then
				-- Create the waypoint.
				local rnd1 = math.abs(Noise:plasma_noise_2d(seeds[1] + wp.x, seeds[1] + wp.z, 2))
				local next_y = min_y + rnd1 * math.min(7, max_y - min_y)
				local wp1 = create_waypoint("corridor", next_x, next_z, next_y, h)
				-- Update the chunks.
				connect_waypoint(wp, dir)
				connect_waypoint(wp1, dirs_inv[dir])
				return true
			end
		end
		delete_waypoint(i)
	end
	-- Create the entrance.
	self.__planner:create_chunk(x, z, {3, params, 10, 0})
	create_waypoint("entrance", x, z, params, 10)
	local created = 1
	-- Create the content.
	while waypoints_num > 0 and created < 32 do
		local rnd = math.random(1, waypoints_num)
		if expand_waypoint(rnd, waypoints[rnd]) then
			created = created + 1
		end
	end
	return true
end

--- Generates a chunk of the place.
-- @param self PlaceDungeon.
-- @param chunk TerrainChunk.
-- @param params Place parameters as set by plan().
PlaceDungeon.generate = function(self, chunk, params)
	if params[1] == 2 then
		self:generate_corridor(chunk, params)
	elseif params[1] == 3 then
		self:generate_entrance(chunk, params)
	else
		-- TODO: rooms
	end
end

--- Generates a dungeon entrance chunk.
-- @param self PlaceDungeon.
-- @param chunk TerrainChunk.
-- @param params Place parameters as set by plan().
PlaceDungeon.generate_entrance = function(self, chunk, params)
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local ms = TerrainMaterialSpec:find_by_name("brick")
	local m = ms and ms.id
	local y = params[2]
	local h = params[3]
	-- Generate the surface normally.
	local surface = self.__planner:get_chunk_surface(chunk)
	self:generate_terrain(chunk, surface)
	-- Create the floor and ceiling.
	t:add_box(
		chunk.x, chunk.z,
		chunk.x + w - 1, chunk.z + w - 1,
		y - 1, h + 2, m)
	-- Create the empty space.
	t:add_box(
		chunk.x, chunk.z,
		chunk.x + w - 1, chunk.z + w - 1,
		y, h, 0)
	-- Create the corner pillars.
	t:add_stick(chunk.x        , chunk.z        , y, h, m)
	t:add_stick(chunk.x + w - 1, chunk.z        , y, h, m)
	t:add_stick(chunk.x        , chunk.z + w - 1, y, h, m)
	t:add_stick(chunk.x + w - 1, chunk.z + w - 1, y, h, m)
	-- Smoothen the surface.
	for x = chunk.x-1,chunk.x+w do
		for z = chunk.z-1,chunk.z+w do
			t:calculate_smooth_normals(x, z)
		end
	end
end

--- Generates a dungeon corridor chunk.
-- @param self PlaceDungeon.
-- @param chunk TerrainChunk.
-- @param params Place parameters as set by plan().
PlaceDungeon.generate_corridor = function(self, chunk, params)
	-- Generate the surface normally.
	local surface = self.__planner:get_chunk_surface(chunk)
	self:generate_terrain(chunk, surface)
	self:generate_plants(chunk, surface)
	-- Create the dungeon space.
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local y = params[2]
	local h = params[3]
	local mask = params[4]
	local conn_xm = Bitwise:bchk(mask, 0x01)
	local conn_xp = Bitwise:bchk(mask, 0x02)
	local conn_zm = Bitwise:bchk(mask, 0x04)
	local conn_zp = Bitwise:bchk(mask, 0x08)
	local wp = {x = math.floor(chunk.x / w), z = math.floor(chunk.z / w)}
	local brick = TerrainMaterialSpec:find_by_name("brick")
	brick = brick and brick.id
	local corridor = function(x1, z1, x2, z2, b00, b10, b01, b11, t00, t10, t01, t11)
		t:add_box_corners(x1, z1, x2, z2, b00-1, b10-1, b01-1, b11-1, t00+1, t10+1, t01+1, t11+1, brick)
		t:add_box_corners(x1, z1, x2, z2, b00, b10, b01, b11, t00, t10, t01, t11, 0)
	end
	if conn_xm and conn_xp and not conn_zm and not conn_zp then
		-- Straight corridor along the X axis.
		local n0,p0 = self.__planner:get_chunk_type(wp.x - 1, wp.z, false)
		local n1,p1 = self.__planner:get_chunk_type(wp.x + 1, wp.z, false)
		local y0 = (n0 == "dungeon" and p0[2] or y)
		local h0 = (n0 == "dungeon" and p0[3] or h)
		local y1 = (n1 == "dungeon" and p1[2] or y)
		local h1 = (n1 == "dungeon" and p1[3] or h)
		local bot0,bot1 = (y0 + y) / 2, (y + y1) / 2
		local top0,top1 = bot0 + (h0 + h) / 2, bot1 + (h + h1) / 2
		corridor(chunk.x, chunk.z + 4, chunk.x + 11, chunk.z + 8,
			bot0, bot1, bot0, bot1, top0, top1, top0, top1, 0)
	elseif not conn_xm and not conn_xp and conn_zm and conn_zp then
		-- Straight corridor along the Z axis.
		local n0,p0 = self.__planner:get_chunk_type(wp.x, wp.z - 1, false)
		local n1,p1 = self.__planner:get_chunk_type(wp.x, wp.z + 1, false)
		local y0 = (n0 == "dungeon" and p0[2] or y)
		local h0 = (n0 == "dungeon" and p0[3] or h)
		local y1 = (n1 == "dungeon" and p1[2] or y)
		local h1 = (n1 == "dungeon" and p1[3] or h)
		local bot0,bot1 = (y0 + y) / 2, (y + y1) / 2
		local top0,top1 = bot0 + (h0 + h) / 2, bot1 + (h + h1) / 2
		corridor(chunk.x + 4, chunk.z, chunk.x + 8, chunk.z + 11,
			bot0, bot0, bot1, bot1, top0, top0, top1, top1, 0)
	else
		-- Curve or dead end.
		corridor(chunk.x + 4, chunk.z + 4, chunk.x + 8, chunk.z + 8,
			y, y, y, y, y + h, y + h, y + h, y + h)
		if conn_xm then
			local n,p = self.__planner:get_chunk_type(wp.x - 1, wp.z, false)
			local y0 = (n == "dungeon" and p[2] or y)
			local h0 = (n == "dungeon" and p[3] or h)
			local bot0,bot1 = (y0 + y) / 2, y
			local top0,top1 = bot0 + (h0 + h) / 2, bot1 + h
			corridor(chunk.x, chunk.z + 4, chunk.x + 3, chunk.z + 8,
				bot0, bot1, bot0, bot1, top0, top1, top0, top1, 0)
		end
		if conn_xp then
			local n,p = self.__planner:get_chunk_type(wp.x + 1, wp.z, false)
			local y1 = (n == "dungeon" and p[2] or y)
			local h1 = (n == "dungeon" and p[3] or h)
			local bot0,bot1 = y, (y + y1) / 2
			local top0,top1 = bot0 + h, bot1 + (h + h1) / 2
			corridor(chunk.x + 9, chunk.z + 4, chunk.x + 11, chunk.z + 8,
				bot0, bot1, bot0, bot1, top0, top1, top0, top1, 0)
		end
		if conn_zm then
			local n,p = self.__planner:get_chunk_type(wp.x, wp.z - 1, false)
			local y0 = (n == "dungeon" and p[2] or y)
			local h0 = (n == "dungeon" and p[3] or h)
			local bot0,bot1 = (y0 + y) / 2, y
			local top0,top1 = bot0 + (h0 + h) / 2, bot1 + h
			corridor(chunk.x + 4, chunk.z, chunk.x + 8, chunk.z + 3,
				bot0, bot0, bot1, bot1, top0, top0, top1, top1, 0)
		end
		if conn_zp then
			local n,p = self.__planner:get_chunk_type(wp.x, wp.z + 1, false)
			local y1 = (n == "dungeon" and p[2] or y)
			local h1 = (n == "dungeon" and p[3] or h)
			local bot0,bot1 = y, (y + y1) / 2
			local top0,top1 = bot0 + h, bot1 + (h + h1) / 2
			corridor(chunk.x + 4, chunk.z + 9, chunk.x + 8, chunk.z + 11,
				bot0, bot0, bot1, bot1, top0, top0, top1, top1, 0)
		end
	end
	-- Smoothen the surface.
	for x = chunk.x-1,chunk.x+w do
		for z = chunk.z-1,chunk.z+w do
			t:calculate_smooth_normals(x, z)
		end
	end
end

--- Gets the dungeon type of the given chunk.
-- @param self PlaceDungeon.
-- @param x Chunk offset.
-- @param z Chunk offset.
-- @return Chunk type, Y offsets and heights. Nil if no dungeon exists.
PlaceDungeon.get_dungeon_type = function(self, x, z)
	local gen = self.__generator
	local seeds = gen.seeds
	if x % 2 == 0 and z % 2 == 0 then
		-- Choose the dungeon amount of the region.
		--local r = Noise:plasma_noise_2d(seeds[1] + 0.1 * x, seeds[1] + 0.1 * z, 3)
		--if r < 0 then return end
		-- Choose the room height.
		local h = math.abs(Noise:plasma_noise_2d(seeds[1] + 0.35 * x, seeds[1] + 0.35 * z, 2))
		if h < 0.15 then return end
		h = h * 8 + 8
		-- Choose the roughness of the dungeon.
		local r = Noise:plasma_noise_2d(seeds[1] + 0.1 * x, seeds[1] + 0.1 * z, 2)
		local p = 0.5 + 0.35 * r
		-- Choose the vertical offset.
		local y1 = math.abs(Noise:harmonic_noise_2d(seeds[1] + 0.02 * x, seeds[1] + 0.02 * z, 6, 1.3, p))
		local y2 = math.abs(Noise:harmonic_noise_2d(seeds[1] + 0.002 * x, seeds[1] + 0.002 * z, 6, 1.3, 0.5))
		local y = 120 + 70 * y1 - 140 * y2
		-- TODO: Choose the dungeon type.
		local t = 1
		return t,y,y,y,y,h,h,h,h
	elseif x % 2 == 0 and z % 2 == 1 then
		-- Vertical connection dungeon.
		local at,ay1,ay2,ay3,ay4,ah1,ah2,ah3,ah4 = self:get_dungeon_type(x, z + 1)
		if not at then return end
		local bt,by1,by2,by3,by4,bh1,bh2,bh3,bh4 = self:get_dungeon_type(x, z - 1)
		if not bt then return end
		-- TODO: Choose the dungeon type.
		local t = 2
		return t,ay3,ay4,by1,by2,ah3,ah4,bh1,bh2
	elseif x % 2 == 1 and z % 2 == 0 then
		-- Horizontal connection dungeon.
		local at,ay1,ay2,ay3,ay4,ah1,ah2,ah3,ah4 = self:get_dungeon_type(x + 1, z)
		if not at then return end
		local bt,by1,by2,by3,by4,bh1,bh2,bh3,bh4 = self:get_dungeon_type(x - 1, z)
		if not bt then return end
		-- TODO: Choose the dungeon type.
		local t = 3
		return t,by2,ay1,by4,ay3,bh2,ah1,bh4,ah3
	end
end

return PlaceDungeon
