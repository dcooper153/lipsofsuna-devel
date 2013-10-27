--- Procedural world layout planner.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.world_planner
-- @alias WorldPlanner

local Class = require("system/class")
local DiamondSquare = require("landscape/generator/diamond-square")
local MapUtils = require("core/server/map-utils")
local Noise = require("system/noise")
local PlaceCastle = require("landscape/generator/place-castle")
local PlaceDefault = require("landscape/generator/place-default")
local PlaceDungeon = require("landscape/generator/place-dungeon")
local SurfaceGenerator = require("landscape/generator/surface-generator")
local TerrainMaterialSpec = require("core/specs/terrain-material")
local Vector = require("system/math/vector")

--- Procedural world layout planner.
-- @type WorldPlanner
local WorldPlanner = Class("WorldPlanner")

--- Creates a new terrain generator.
-- @param clss WorldPlanner class.
-- @param terrain TerrainManager.
-- @param generator TerrainGenerator.
-- @return WorldPlanner.
WorldPlanner.new = function(clss, terrain, generator)
	local self = Class.new(clss)
	self.__region_size = 16
	self.__region_places = 32
	self.__chunks = {}
	self.__regions = {}
	self.__terrain = terrain
	self.__generator = generator
	self.__heights = DiamondSquare(1024)
	self.__place_types =
	{
		["castle"] = PlaceCastle(self.__generator, self),
		["default"] = PlaceDefault(self.__generator, self),
		["dungeon"] = PlaceDungeon(self.__generator, self)
	}
	self.__chunk_types =
	{
		[1] = "castle",
		[2] = "dungeon",
		[3] = "dungeon"
	}
	return self
end

--- Marks a chunk as planned.
-- @param x Chunk X coordinate.
-- @param z Chunk Z coordinate.
-- @param params Chunk parameters.
WorldPlanner.create_chunk = function(self, x, z, params)
	local id = self:__xz_to_id(x, z)
	self.__chunks[id] = params or {999}
end

--- Positions an individual place on the map.
-- @param self WorldPlanner.
-- @param x Chunk X coordinate.
-- @param z Chunk Z coordinate.
WorldPlanner.create_place = function(self, x, z)
	-- Find all the fitting place types.
	local fit = {}
	for k,v in pairs(self.__place_types) do
		local types = v:check(x, z)
		if types then
			for k1,v1 in pairs(types) do
				table.insert(fit, {k,k1,v1})
			end
		end
	end
	-- Choose a random place type.
	local num = #fit
	if #fit == 0 then return end
	local type = fit[math.random(1, num)]
	-- Plan the chosen place.
	local generator = self.__place_types[type[1]]
	return generator:plan(x, z, type[2], type[3])
end

--- Populates the regions around the given chunk.
-- @param self WorldPlanner.
-- @param x Chunk X coordinate.
-- @param z Chunk Z coordinate.
WorldPlanner.create_region = function(self, x, z)
	-- Calculate the range of regions.
	local s = self.__region_size
	local maxx = math.floor(x / s) + 1
	local maxz = math.floor(z / s) + 1
	local minx = math.max(0, maxx - 2)
	local minz = math.max(0, maxz - 2)
	-- Create the regions.
	local queued = self.__recurse or {}
	for rz = minz,maxz do
		local id = minx + rz * 0xFFFF
		for rx = minx,maxx do
			if not self.__regions[id] then
				self.__regions[id] = true
				queued[id] = {rx,rz}
			end
			id = id + 1
		end
	end
	-- Create the places of the regions.
	if not self.__recurse then
		self.__recurse = {}
		while true do
			local more = false
			for id,region in pairs(queued) do
				self:create_region_places(region[1], region[2])
				more = true
			end
			if not more then break end
			queued = self.__recurse
			self.__recurse = {}
		end
		self.__recurse = nil
	end
end

--- Creates the places of the region.
-- @param self WorldPlanner.
-- @param x Region X coordinate.
-- @param z Region Z coordinate.
WorldPlanner.create_region_places = function(self, x, z)
	local s = self.__region_size
	for i = 1,self.__region_places do
		local px = x*s + math.random(0,s-1)
		local pz = z*s + math.random(0,s-1)
		self:create_place(px, pz)
	end
end

--- Generates the initial plan at game start.
-- @param self TerrainGenerator.
WorldPlanner.initialize = function(self)
	self.__heights:calculate(self.__generator.seeds)
end

--- Checks if any places intersect with the given rectangle.
-- @param self WorldPlanner.
-- @param x Rectangle offset in chunks.
-- @param z Rectangle offset in chunks.
-- @param sx Rectangle size in chunks.
-- @param sz Rectangle size in chunks.
-- @return True if intersects. False otherwise.
WorldPlanner.intersects = function(self, x, z, sx, sz)
	for cz = z,z+sz-1 do
		for cx = x,x+sx-1 do
			local id = self:__xz_to_id(cx, cz)
			if self.__chunks[id] then return true end
		end
	end
end

--- Gets the generator for the given chunk type.
-- @param self WorldPlanner.
-- @param x Chunk offset.
-- @param z Chunk offset.
-- @return PlaceGenerator and parameters if applicable. Nil otherwise.
WorldPlanner.get_chunk_generator = function(self, x, z)
	local name,params = self:get_chunk_type(x, z)
	return self.__place_types[name or "default"], params
end

--- Gets the surface heights of the chunk.
-- @param self WorldPlanner.
-- @param chunk Chunk.
-- @return Surface heights.
WorldPlanner.get_chunk_surface = function(self, chunk)
	local s = SurfaceGenerator(chunk.manager.chunk_size)
	s:generate(chunk, self)
	return s
end

--- Gets the type of the chunk.
-- @param self WorldPlanner.
-- @param x Chunk offset.
-- @param z Chunk offset.
-- @return Chunk name if special. Nil for a normal chunk.
WorldPlanner.get_chunk_type = function(self, x, z, create)
	if create ~= false then
		self:create_region(x, z)
	end
	local id = self:__xz_to_id(x, z)
	local t = self.__chunks[id]
	if not t then return end
	local c = self.__chunk_types[t[1]]
	return c, t
end

--- Gets the exact surface height of the default terrain at the given point.
-- @param self WorldPlanner.
-- @param x X coordinate in grid units.
-- @param z Z coordinate in grid units.
-- @return Total height, rock height, soil height, grass height.
WorldPlanner.get_height = function(self, x, z)
	local w = self.__terrain.terrain.chunk_size
	local seeds = self.__generator.seeds
	-- Choose the bumpiness of the region.
	-- This is affected by the slope steepness.
	-- p=0.7: Very smooth.
	-- p=0.9: Very bumpy.
	local gx,gy,gv = self.__heights:get_gradient(x / w, z / w)
	local g = math.min(gv/50, 1)
	local r = math.abs(Noise:plasma_noise_2d(seeds[1] + 0.01 * x, seeds[2] + 0.01 * z, 2))
	local p = math.min(0.9, 0.7 + 0.1 * r + 0.25 * g)
	-- Choose the height of the region.
	-- This is affected by both the position and the bumpiness.
	local n1a = self.__heights:get_height(x / w, z / w)
	local n1b = Noise:harmonic_noise_2d(seeds[1] + 0.001 * x, seeds[2] + 0.001 * z, 6, 1.3, p)
	local n1 = n1a + 20 * n1b
	-- Choose the soil layer height.
	-- This is affected by the bumpiness and the slope steepness.
	local n2a = Noise:harmonic_noise_2d(seeds[3] + 0.02 * x, seeds[4] + 0.02 * z, 3, 1.3, 0.5 + g)
	local n2b = Noise:plasma_noise_2d(seeds[1] + 0.02 * x, seeds[2] + 0.02 * z, 3 - r)
	local n2 = 10 * math.max(0, 1 - 0.5 * g - 0.8 + 0.2 * n2a)
	-- Choose the grass layer height.
	-- This is mostly just random on any areas with soil.
	local gr = Noise:plasma_noise_2d(seeds[1] + 0.03 * x, seeds[2] + 0.03 * z, 3 - r)
	local n3 = math.max(0, gr) * n2
	-- Return the heights.
	local a = n1
	local b = 2 * n2
	local c = 0.5 * n3
	return a+b+c, a, b, c
end

--- Loads the planner state.
-- @param self WorldPlanner.
-- @param db Database.
WorldPlanner.load = function(self, db)
	-- Load regions.
	local rows = db:query([[
		SELECT x,z
		FROM planned_regions;]])
	if rows then
		for k,v in ipairs(rows) do
			local x,z = v[1],v[2]
			self.__regions[self:__xz_to_id(x, z)] = true
		end
	end
	-- Load chunks.
	local rows = db:query([[
		SELECT x,z,type,a,b,c,d,e,f,g,h
		FROM planned_chunks;]])
	if rows then
		for k,v in ipairs(rows) do
			local x,z = v[1],v[2]
			table.remove(v, 2)
			table.remove(v, 1)
			self.__chunks[self:__xz_to_id(x, z)] = v
		end
	end
	-- Load heights.
	local rows = db:query([[
		SELECT x,z,data
		FROM planned_heights;]])
	if rows then
		for k,v in ipairs(rows) do
			v[3]:read()
			self.__heights:set_data(v[3])
		end
	end
end

--- Saves the planner state.
-- @param self WorldPlanner.
-- @param db Database.
-- @param erase True to erase existing data. False otherwise.
WorldPlanner.save = function(self, db, erase)
	db:query("BEGIN TRANSACTION;")
	if erase then
		-- Erase regions.
		db:query([[DROP TABLE IF EXISTS planned_regions;]])
		db:query([[CREATE TABLE planned_regions (
			x INTEGER NOT NULL,
			z INTEGER NOT NULL,
			PRIMARY KEY (x,z));]])
		-- Erase chunks.
		db:query([[DROP TABLE IF EXISTS planned_chunks;]])
		db:query([[CREATE TABLE planned_chunks (
			x INTEGER NOT NULL,
			z INTEGER NOT NULL,
			type INTEGER,
			a REAL, b REAL, c REAL, d REAL,
			e REAL, f REAL, g REAL, h REAL,
			PRIMARY KEY (x,z));]])
		-- Erase heights.
		db:query([[DROP TABLE IF EXISTS planned_heights;]])
		db:query([[CREATE TABLE planned_heights (
			x INTEGER NOT NULL,
			z INTEGER NOT NULL,
			data BLOB,
			PRIMARY KEY (x,z));]])
	end
	-- Save regions.
	for k,v in pairs(self.__regions) do
		local x,z = self:__id_to_xz(k)
		db:query([[
			REPLACE INTO planned_regions
			(x,z)
			VALUES (?,?);]],
			{x, z})
	end
	-- Save chunks.
	for k,v in pairs(self.__chunks) do
		local x,z = self:__id_to_xz(k)
		db:query([[
			REPLACE INTO planned_chunks
			(x,z,type,a,b,c,d,e,f,g,h)
			VALUES (?,?,?,?,?,?,?,?,?,?,?);]],
			{x, z, v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9]})
	end
	-- Save heights.
	db:query([[
		REPLACE INTO planned_heights
		(x,z,data)
		VALUES (?,?,?);]],
		{0, 0, self.__heights:get_data()})
	db:query("END TRANSACTION;")
end

WorldPlanner.__xz_to_id = function(self, x, z)
	return x + 0xFFFF * z
end

WorldPlanner.__id_to_xz = function(self, id)
	return id % 0xFFFF, math.floor(id / 0xFFFF)
end

return WorldPlanner
