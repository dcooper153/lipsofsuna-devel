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
local MapUtils = require("core/utils/map")
local Vector = require("system/math/vector")
local WorldPlanner = require("landscape/generator/world-planner")

--- Procedural terrain generator.
-- @type TerrainGenerator
local TerrainGenerator = Class("TerrainGenerator")

--- Creates a new terrain generator.
-- @param clss TerrainGenerator class.
-- @param terrain Terrain.
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
	self.terrain = terrain
	self.world_planner = WorldPlanner(terrain, self)
	return self
end

--- Generates a terrain chunk.
-- @param self TerrainGenerator.
-- @param chunk TerrainChunk.
TerrainGenerator.generate = function(self, chunk)
	local w = chunk.manager.chunk_size
	local generator,params = self.world_planner:get_chunk_generator(chunk.x / w, chunk.z / w)
	if generator then
		generator:generate(chunk, params)
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

return TerrainGenerator
