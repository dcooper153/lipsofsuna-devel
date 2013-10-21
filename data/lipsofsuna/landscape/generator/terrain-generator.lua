--- Procedural terrain generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.terrain_generator
-- @alias TerrainGenerator

local Class = require("system/class")
local MapUtils = require("core/server/map-utils")
local Noise = require("system/noise")
local TerrainMaterialSpec = require("core/specs/terrain-material")
local Vector = require("system/math/vector")
local WorldPlanner = require("landscape/generator/world-planner")

--- Procedural terrain generator.
-- @type TerrainGenerator
local TerrainGenerator = Class("TerrainGenerator")

--- Creates a new terrain generator.
-- @param clss TerrainGenerator class.
-- @return TerrainGenerator.
TerrainGenerator.new = function(clss, terrain)
	local self = Class.new(clss)
	self.seeds =
	{
		math.random(100, 600),
		math.random(100, 600),
		math.random(100, 600),
		math.random(100, 600),
		math.random(100, 600),
		math.random(100, 600),
		math.random(100, 600),
		math.random(100, 600),
		math.random(100, 600),
		math.random(100, 600)
	}
	self.world_planner = WorldPlanner(terrain, self)
	return self
end

--- Generates a terrain chunk.
-- @param self TerrainGenerator.
-- @param chunk TerrainChunk.
TerrainGenerator.generate = function(self, chunk)
	-- Choose the chunk type.
	local w = chunk.manager.chunk_size
	local t,params = self.world_planner:get_chunk_type(chunk.x / w, chunk.z / w)
	if t == "castle" then
		self:__generate_castle(chunk, params)
	else
		-- Generate the surface.
		local surface = self.world_planner:get_chunk_surface(chunk)
		self:__generate_default(chunk, surface)
		self:__generate_default_plants(chunk, surface)
		-- Generate a civilization obstacle.
		if math.random() < 0.05 then
			-- Calculate the position.
			local civ_x = math.random(0, w-1)
			local civ_z = math.random(0, w-1)
			local civ_y = surface:get_height_bilinear(civ_x + 0.5, civ_z + 0.5)
			local p = Vector(chunk.x + civ_x + 0.5, 0.0, chunk.z + civ_z + 0.5)
			p:multiply(chunk.manager.grid_size)
			p:add_xyz(0, civ_y, 0)
			-- Choose and create the obstacle.
			MapUtils:place_obstacle{point = p, category = "civilization"}
		end
	end
end

--- Generates the initial terrain data at game start.
-- @param self TerrainGenerator.
-- @param db Database.
-- @param erase True to erase existing data. False otherwise.
TerrainGenerator.initialize = function(self)
	self.world_planner:initialize()
end

--- Loads the terrain generator state.
-- @param self TerrainGenerator.
-- @param db Database.
TerrainGenerator.load = function(self, db)
	local r1 = db:query("SELECT key,value FROM generator_settings;")
	local f1 =
	{
		seed1 = function(v) self.seeds[1] = tonumber(v) end,
		seed2 = function(v) self.seeds[2] = tonumber(v) end,
		seed3 = function(v) self.seeds[3] = tonumber(v) end,
		seed4 = function(v) self.seeds[4] = tonumber(v) end,
		seed5 = function(v) self.seeds[5] = tonumber(v) end,
		seed6 = function(v) self.seeds[6] = tonumber(v) end,
		seed7 = function(v) self.seeds[7] = tonumber(v) end,
		seed8 = function(v) self.seeds[8] = tonumber(v) end,
		seed9 = function(v) self.seeds[9] = tonumber(v) end,
		seed10 = function(v) self.seeds[10] = tonumber(v) end
	}
	for k,v in ipairs(r1) do
		local f = f1[v[1]]
		if f then f(v[2]) end
	end
	self.world_planner:load(db)
end

--- Saves the terrain generator state.
-- @param self TerrainGenerator.
-- @param db Database.
-- @param erase True to erase existing data. False otherwise.
TerrainGenerator.save = function(self, db, erase)
	db:query("BEGIN TRANSACTION;")
	if erase then
		db:query([[DROP TABLE IF EXISTS generator_settings;]])
		db:query([[CREATE TABLE generator_settings (
			key TEXT PRIMARY KEY,
			value TEXT);]])
	end
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed1", tostring(self.seeds[1])})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed2", tostring(self.seeds[2])})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed3", tostring(self.seeds[3])})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed4", tostring(self.seeds[4])})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed5", tostring(self.seeds[5])})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed6", tostring(self.seeds[6])})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed7", tostring(self.seeds[7])})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed8", tostring(self.seeds[8])})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed9", tostring(self.seeds[9])})
	db:query("REPLACE INTO generator_settings (key,value) VALUES (?,?);", {"seed10", tostring(self.seeds[10])})
	db:query("END TRANSACTION;")
	self.world_planner:save(db, erase)
end

--- Generates a castle chunk.
-- @param self TerrainGenerator.
-- @param chunk TerrainChunk.
-- @param params Castle parameters.
TerrainGenerator.__generate_castle = function(self, chunk, params)
	-- TODO
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local m = TerrainMaterialSpec:find_by_name("brick")
	m = m and m.id
	-- Create the floor.
	for z = 0,w-1 do
		for x = 0,w-1 do
			t:add_stick(chunk.x + x, chunk.z + z, 0, params[2], 3)
			t:add_stick(chunk.x + x, chunk.z + z, params[2], 1, m)
		end
	end
	-- Create the west wall.
	local t1,p1 = self.world_planner:get_chunk_type(chunk.x / w - 1, chunk.z / w)
	if t1 ~= "castle" or p1[2] ~= params[2] then
		for z = 0,w-1 do
			t:add_stick(chunk.x, chunk.z + z, params[2], params[3] + (z % 2), m)
		end
	end
	-- Create the east wall.
	local t2,p2 = self.world_planner:get_chunk_type(chunk.x / w + 1, chunk.z / w)
	if t2 ~= "castle" or p2[2] ~= params[2]  then
		for z = 0,w-1 do
			t:add_stick(chunk.x + w - 1, chunk.z + z, params[2], params[3] + (z % 2), m)
		end
	end
	-- Create the south wall.
	local t3,p3 = self.world_planner:get_chunk_type(chunk.x / w, chunk.z / w - 1)
	if t3 ~= "castle" or p3[2] ~= params[2]  then
		for x = 0,w-1 do
			t:add_stick(chunk.x + x, chunk.z, params[2], params[3] + (x % 2), m)
		end
	end
	-- Create the north wall.
	local t4,p4 = self.world_planner:get_chunk_type(chunk.x / w, chunk.z / w + 1)
	if t4 ~= "castle" or p4[2] ~= params[2]  then
		for x = 0,w-1 do
			t:add_stick(chunk.x + x, chunk.z + w - 1, params[2], params[3] + (x % 2), m)
		end
		local door = chunk.x + math.floor(w/2)
		t:add_stick(door, chunk.z + w - 1, params[2] + 1, params[3]/2, 0)
		t:add_stick(door+1, chunk.z + w - 1, params[2] + 1, params[3]/2, 0)
	end
end

TerrainGenerator.__generate_chasm = function(self, chunk)
	-- TODO
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local get_chasm_height_at = function(x, z)
		-- Choose the dungeon amount of the region.
		--local r = Noise:plasma_noise_2d(self.seed1 + 0.001 * x, self.seed1 + 0.001 * z, 3)
		--if r < 0 then return 0, 0 end
		-- Choose the roughness of the chasm.
		local r = Noise:plasma_noise_2d(self.seeds[1] + 0.01 * x, self.seeds[1] + 0.01 * z, 2)
		local p = 0.5 + 0.35 * r
		-- Choose the vertical offset.
		local y1 = math.abs(Noise:harmonic_noise_2d(self.seeds[1] + 0.001 * x, self.seeds[1] + 0.001 * z, 6, 1.3, p))
		local y2 = math.abs(Noise:harmonic_noise_2d(self.seeds[1] + 0.0001 * x, self.seeds[1] + 0.0001 * z, 6, 1.3, 0.5))
		-- Choose the height.
		local h = math.abs(Noise:plasma_noise_2d(self.seeds[1] + 0.035 * x, self.seeds[1] + 0.035 * z, 2))
		if h < 0.15 then
			h = 0
		end
		-- Return the combined heights,
		return 80 + 70 * y1 - 70 * y2, h * 10
	end
	local get_chasm_at = function(x, z)
		local i = 5 * x + stride * z
		local y, h = h[i + 3], h[i + 4]
		--return y - h*0.1, y + h*0.9
		return y - h/2, y + h/2
	end
	-- Calculate the heights.
	local h = {}
	local i = 0
	for z = 0,w do
		for x = 0,w do
			h[i + 3], h[i + 4] = get_chasm_height_at(chunk.x + x, chunk.z + z)
			i = i + 5
		end
	end
	-- Generate the chasm.
	for z = 0,w-1 do
		for x = 0,w-1 do
			-- Calculate the chasm corner heights.
			local d00,e00 = get_chasm_at(x, z)
		local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
		local d10,e10 = get_chasm_at(x + 1, z)
			local d01,e01 = get_chasm_at(x, z + 1)
			local d11,e11 = get_chasm_at(x + 1, z + 1)
			-- Generate the chasm.
			if d00 < e00 and d10 < e10 and d01 < e01 and d11 < e11 then
				t:add_stick_corners(chunk.x + x, chunk.z + z, d00, d10, d01, d11, e00, e10, e01, e11, 0)
			end
		end
	end
end

TerrainGenerator.__generate_default = function(self, chunk, surface)
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	-- Generate the surface.
	for z = 0,w-1 do
		local cz = chunk.z + z
		for x = 0,w-1 do
			local cx = chunk.x + x
			-- Calculate the corner heights.
			local a00,b00,c00 = surface:get(x, z)
			local a10,b10,c10 = surface:get(x + 1, z)
			local a01,b01,c01 = surface:get(x, z + 1)
			local a11,b11,c11 = surface:get(x + 1, z + 1)
			-- Generate the grass layer.
			if c00 > 0 or c10 > 0 or c01 > 0 or c11 > 0 then
				local y0,y1,y2,y3 = a00 + b00 + c00, a10 + b10 + c10, a01 + b01 + c01, a11 + b11 + c11
				t:add_stick_corners(cx, cz, 0, 0, 0, 0, y0, y1, y2, y3, 2)
			end
			-- Generate the soil layer.
			if b00 > 0 or b10 > 0 or b01 > 0 or b11 > 0 then
				t:add_stick_corners(cx, cz, 0, 0, 0, 0, a00 + b00, a10 + b10, a01 + b01, a11 + b11, 1)
			end
			-- Generate the rock layer.
			t:add_stick_corners(cx, cz, 0, 0, 0, 0, a00, a10, a01, a11, 3)
			-- Generate ore veins.
			local ore = math.random() * 300
			if ore < 3 and ore > 0.1 then
				local y0,y1,y2,y3 = a00 + b00 + c00, a10 + b10 + c10, a01 + b01 + c01, a11 + b11 + c11
				local mat
				if math.random() < 0.75 then
					mat = TerrainMaterialSpec:random{category = "common ore"}
				else
					mat = TerrainMaterialSpec:random{category = "rare ore"}
				end
				t:add_stick_corners(cx, cz, y0 - ore, y1 - ore, y2 - ore, y3 - ore, y0, y1, y2, y3, mat.id)
			end
		end
	end
	-- Smoothen the surface.
	for x = chunk.x-1,chunk.x+w do
		for z = chunk.z-1,chunk.z+w do
			t:calculate_smooth_normals(x, z)
		end
	end
end

TerrainGenerator.__generate_default_plants = function(self, chunk, surface)
	local p = Vector()
	local w = chunk.manager.chunk_size
	for z = 0,w-1 do
		local cz = chunk.z + z
		for x = 0,w-1 do
			local cx = chunk.x + x
			if math.random() <= 0.01 and surface:get_column_type(x, z) == "grass" then
				--if civ_x ~= x or civ_z ~= z then
				-- Calculate the position.
				p:set_xyz(cx + 0.5, 0.0, cz + 0.5)
				p:multiply(chunk.manager.grid_size)
				p:add_xyz(0, surface:get_height_bilinear(x + 0.5, z + 0.5), 0)
				-- Calculate the forest ratio.
				local r = math.random()
				local f = Noise:plasma_noise_2d(self.seeds[5] + 0.005 * cx, self.seeds[6] + 0.005 * cz, 2)
				-- Choose and create the obstacle.
				if r > f * 0.01 then
					MapUtils:place_obstacle{point = p, category = "tree", rotation = math.random() * math.pi * 2}
				else
					MapUtils:place_obstacle{point = p, category = "small-plant", rotation = math.random() * math.pi * 2}
				end
			end
		end
	end
end

TerrainGenerator.__generate_dungeon = function(self, chunk)
	-- Generate the surface normally.
	local surface = self.world_planner:get_chunk_surface(chunk)
	self:__generate_default(chunk, surface)
	self:__generate_default_plants(chunk, surface)
	-- Calculate smooth floor vertices.
	local w = chunk.manager.chunk_size
	local t = chunk.manager.terrain
	local dt,dy1,dy2,dy3,dy4,dh1,dh2,dh3,dh4 = self.world_planner:get_dungeon_type(chunk.manager, chunk.x, chunk.z)
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

TerrainGenerator.__generate_dungeon_entrance = function(self, chunk)
	-- TODO
end

TerrainGenerator.__generate_house = function(self, chunk, params)
	-- TODO
end

return TerrainGenerator
