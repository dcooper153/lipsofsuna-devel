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
local TerrainChunk = require("system/terrain-chunk")
local TerrainMaterialSpec = require("core/specs/terrain-material")

local yield = coroutine.yield

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
			if h_min + 15 < h_max then
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
	local check_room = function(x, z, wp, h, dir)
		local sx = math.random(1,3)
		local sz = math.random(1,3)
		local px,pz
		if dir == 1 then
			px = x - sx + 1
			pz = z - math.random(0, sz - 1)
		elseif dir == 2 then
			px = x
			pz = z - math.random(0, sz - 1)
		elseif dir == 3 then
			px = x - math.random(0, sx - 1)
			pz = z - sz + 1
		else
			px = x - math.random(0, sx - 1)
			pz = z
		end
		for cz = pz,pz+sz-1 do
			for cx = px,px+sx-1 do
				if not check_waypoint(cx, cz, wp, h) then return end
			end
		end
		return px,pz,sx,sz
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
				local rnd1 = math.abs(Noise:plasma_noise_2d(seeds[1] + wp.x, seeds[1] + wp.z, 2))
				local next_y = min_y + rnd1 * math.min(7, max_y - min_y)
				if wp.type == "room" then next_y = wp.y end
				local room_h = math.random(5,8)
				local room_x,room_z,room_sx,room_sz = check_room(next_x, next_z, wp, room_h, dir)
				if room_x and math.random() < 0.2 then
					-- Create a room.
					next_y = wp.y
					for z = room_z,room_z+room_sz-1 do
						for x = room_x,room_x+room_sx-1 do
							self.__planner:create_chunk(x, z, {4, next_y, room_h, 0})
							local wp1 = create_waypoint("room", x, z, next_y, room_h)
							if x > room_x then connect_waypoint(wp1, 1) end
							if x < room_x+room_sx-1 then connect_waypoint(wp1, 2) end
							if z > room_z then connect_waypoint(wp1, 3) end
							if z < room_z+room_sz-1 then connect_waypoint(wp1, 4) end
							if x == next_x and z == next_z then
								connect_waypoint(wp1, dirs_inv[dir])
							end
						end
					end
					connect_waypoint(wp, dir)
				else
					-- Create a corridor.
					local wp1 = create_waypoint("corridor", next_x, next_z, next_y, h)
					connect_waypoint(wp, dir)
					connect_waypoint(wp1, dirs_inv[dir])
					return true
				end
			end
		end
		delete_waypoint(i)
	end
	-- Create the entrance.
	self.__planner:create_chunk(x, z, {3, params, 10, 0})
	create_waypoint("entrance", x, z, params, 10)
	local last = nil
	local created = 1
	-- Create the content.
	while waypoints_num > 0 and created < 64 do
		-- Choose the next waypoint.
		if not last or math.random() < 0.1 then
			last = math.random(1, waypoints_num)
		end
		-- Try to expand the waypoint.
		if expand_waypoint(last, waypoints[last]) then
			created = created + 1
			last = waypoints_num
			if waypoints[last].type ~= "corridor" then
				last = nil
			end
		else
			last = nil
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
		self:generate_room(chunk, params)
	end
end

--- Generates a dungeon entrance chunk.
-- @param self PlaceDungeon.
-- @param chunk TerrainChunk.
-- @param params Place parameters as set by plan().
PlaceDungeon.generate_entrance = function(self, chunk, params)
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local ms = Main.specs:find_by_name("TerrainMaterialSpec", "brick")
	local m = ms and ms.id
	local y = params[2]
	local h = params[3]
	local chk = TerrainChunk(w)
	-- Generate the surface normally.
	local surface = self.__planner:get_chunk_surface(chunk)
	self:generate_terrain(chunk, surface, chk)
	yield()
	-- Create the floor and ceiling.
	chk:add_box(0, 0, w-1, w-1, y-1, h+2, m)
	chk:add_box(0, 0, w-1, w-1, y, h, 0)
	yield()
	-- Create the corner pillars.
	chk:add_stick(0  , 0  , y, h, m)
	chk:add_stick(w-1, 0  , y, h, m)
	chk:add_stick(0  , w-1, y, h, m)
	chk:add_stick(w-1, w-1, y, h, m)
	yield()
	-- Smoothen the surface.
	t:set_chunk(chunk.x, chunk.z, chk)
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
	local t = chunk.manager.terrain
	local w = chunk.manager.chunk_size
	local chk = TerrainChunk(w)
	local surface = self.__planner:get_chunk_surface(chunk)
	self:generate_terrain(chunk, surface, chk)
	yield()
	-- Create the dungeon space.
	local y = params[2]
	local h = params[3]
	local mask = params[4]
	local conn_xm = Bitwise:bchk(mask, 0x01)
	local conn_xp = Bitwise:bchk(mask, 0x02)
	local conn_zm = Bitwise:bchk(mask, 0x04)
	local conn_zp = Bitwise:bchk(mask, 0x08)
	local wp = {x = math.floor(chunk.x / w), z = math.floor(chunk.z / w)}
	local brick = Main.specs:find_by_name("TerrainMaterialSpec", "brick")
	brick = brick and brick.id
	yield()
	local corridor = function(x1, z1, x2, z2, b00, b10, b01, b11, t00, t10, t01, t11)
		chk:add_box_corners(x1, z1, x2, z2, b00-1, b10-1, b01-1, b11-1, t00+1, t10+1, t01+1, t11+1, brick)
		chk:add_box_corners(x1, z1, x2, z2, b00, b10, b01, b11, t00, t10, t01, t11, 0)
		yield()
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
		corridor(0, 4, 11, 8, bot0, bot1, bot0, bot1, top0, top1, top0, top1, 0)
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
		corridor(4, 0, 8, 11, bot0, bot0, bot1, bot1, top0, top0, top1, top1, 0)
	else
		-- Curve or dead end.
		corridor(4, 4, 8, 8, y, y, y, y, y + h, y + h, y + h, y + h)
		if conn_xm then
			local n,p = self.__planner:get_chunk_type(wp.x - 1, wp.z, false)
			local y0 = (n == "dungeon" and p[2] or y)
			local h0 = (n == "dungeon" and p[3] or h)
			local bot0,bot1 = (y0 + y) / 2, y
			local top0,top1 = bot0 + (h0 + h) / 2, bot1 + h
			corridor(0, 4, 3, 8, bot0, bot1, bot0, bot1, top0, top1, top0, top1, 0)
		end
		if conn_xp then
			local n,p = self.__planner:get_chunk_type(wp.x + 1, wp.z, false)
			local y1 = (n == "dungeon" and p[2] or y)
			local h1 = (n == "dungeon" and p[3] or h)
			local bot0,bot1 = y, (y + y1) / 2
			local top0,top1 = bot0 + h, bot1 + (h + h1) / 2
			corridor(9, 4, 11, 8, bot0, bot1, bot0, bot1, top0, top1, top0, top1, 0)
		end
		if conn_zm then
			local n,p = self.__planner:get_chunk_type(wp.x, wp.z - 1, false)
			local y0 = (n == "dungeon" and p[2] or y)
			local h0 = (n == "dungeon" and p[3] or h)
			local bot0,bot1 = (y0 + y) / 2, y
			local top0,top1 = bot0 + (h0 + h) / 2, bot1 + h
			corridor(4, 0, 8, 3, bot0, bot0, bot1, bot1, top0, top0, top1, top1, 0)
		end
		if conn_zp then
			local n,p = self.__planner:get_chunk_type(wp.x, wp.z + 1, false)
			local y1 = (n == "dungeon" and p[2] or y)
			local h1 = (n == "dungeon" and p[3] or h)
			local bot0,bot1 = y, (y + y1) / 2
			local top0,top1 = bot0 + h, bot1 + (h + h1) / 2
			corridor(4, 9, 8, 11, bot0, bot0, bot1, bot1, top0, top0, top1, top1, 0)
		end
	end
	-- Smoothen the surface.
	t:set_chunk(chunk.x, chunk.z, chk)
	for x = chunk.x-1,chunk.x+w do
		for z = chunk.z-1,chunk.z+w do
			t:calculate_smooth_normals(x, z)
		end
	end
	yield()
	-- Generate surface objects.
	self:generate_plants(chunk, surface)
	-- Generate items in dead ends.
	local connections = (conn_xm and 1 or 0) + (conn_xp and 1 or 0) + (conn_zm and 1 or 0) + (conn_zp and 1 or 0)
	if connections == 1 then
		local p = Vector(chunk.x + 6.5, 0.0, chunk.z + 6.5)
		p:multiply(chunk.manager.grid_size):add_xyz(0, y, 0)
		local obj = MapUtils:place_item{point = p, name = "treasure chest", rotation = math.random() * math.pi * 2}
		obj:set_important(true)
		yield()
	end
	-- Generate extra monsters.
	if connections >= 2 and math.random() < 0.5 then
		local p = Vector(chunk.x + 6.5, 0.0, chunk.z + 6.5)
		p:multiply(chunk.manager.grid_size):add_xyz(0, y, 0)
		local obj = MapUtils:place_actor{point = p, category = "enemy", rotation = math.random() * math.pi * 2}
		obj:set_important(true)
		yield()
	end
end

--- Generates a dungeon room chunk.
-- @param self PlaceDungeon.
-- @param chunk TerrainChunk.
-- @param params Place parameters as set by plan().
PlaceDungeon.generate_room = function(self, chunk, params)
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local ms = Main.specs:find_by_name("TerrainMaterialSpec", "brick")
	local m = ms and ms.id
	local y = params[2]
	local h = params[3]
	local mask = params[4]
	-- Generate the surface normally.
	local surface = self.__planner:get_chunk_surface(chunk)
	local chk = TerrainChunk(w)
	self:generate_terrain(chunk, surface, chk)
	yield()
	-- Create the floor and ceiling.
	chk:add_box(0, 0, w-1, w-1, y-1, h+2, m)
	chk:add_box(0, 0, w-1, w-1, y, h, 0)
	yield()
	-- Create the walls.
	local conn_xm = Bitwise:bchk(mask, 0x01)
	local conn_xp = Bitwise:bchk(mask, 0x02)
	local conn_zm = Bitwise:bchk(mask, 0x04)
	local conn_zp = Bitwise:bchk(mask, 0x08)
	if not conn_xm then chk:add_box(0  , 0  , 0  , w-1, y, h, m) end
	if not conn_xp then chk:add_box(w-1, 0  , w-1, w-1, y, h, m) end
	if not conn_zm then chk:add_box(0  , 0  , w-1, 0  , y, h, m) end
	if not conn_zp then chk:add_box(0  , w-1, w-1, w-1, y, h, m) end
	-- Smoothen the surface.
	local cx = chunk.x
	local cz = chunk.z
	t:set_chunk(cx, cz, chk)
	for x = cx-1,cx+w do
		for z = cz-1,cz+w do
			t:calculate_smooth_normals(x, z)
		end
	end
	yield()
	-- Generate a civilization obstacle.
	if math.random() < 0.5 then
		-- Calculate the position.
		local civ_x = math.random(1, w-2)
		local civ_z = math.random(1, w-2)
		local civ_y = y
		local p = Vector(chunk.x + civ_x + 0.5, 0.0, chunk.z + civ_z + 0.5)
		p:multiply(chunk.manager.grid_size)
		p:add_xyz(0, civ_y, 0)
		-- Choose and create the obstacle.
		MapUtils:place_obstacle{point = p, category = "civilization", rotation = math.random() * math.pi * 2}
		yield()
	end
	-- Generate extra monsters.
	if math.random() < 0.5 then
		local p = Vector(chunk.x + math.random(1, w-2), 0.0, chunk.z + math.random(1, w-2))
		p:multiply(chunk.manager.grid_size):add_xyz(0, y, 0)
		local obj = MapUtils:place_actor{point = p, category = "enemy", rotation = math.random() * math.pi * 2}
		obj:set_important(true)
		yield()
	end
end

return PlaceDungeon
