--- Default terrain chunk generator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.generator.place_default
-- @alias PlaceDefault

local Class = require("system/class")
local MapUtils = require("core/utils/map")
local Noise = require("system/noise")
local PlaceGenerator = require("landscape/generator/place-generator")
local TerrainChunk = require("system/terrain-chunk")

local yield = coroutine.yield

--- Default terrain chunk generator.
-- @type PlaceDefault
local PlaceDefault = Class("PlaceDefault", PlaceGenerator)

--- Creates a new default chunk generator.
-- @param clss PlaceDefault class.
-- @param generator TerrainGenerator.
-- @param planner WorldPlanner.
-- @return PlaceDefault.
PlaceDefault.new = function(clss, generator, planner)
	local self = PlaceGenerator.new(clss, generator, planner)
	return self
end

--- Generates a chunk of the place.
-- @param self PlaceDefault.
-- @param chunk TerrainChunk.
-- @param params Place parameters as set by plan().
PlaceDefault.generate = function(self, chunk, params)
	-- Generate the surface.
	local t = chunk.manager.terrain
	local w = chunk.manager.chunk_size
	local surface = self.__planner:get_chunk_surface(chunk)
	local chk = TerrainChunk(w)
	self:generate_terrain(chunk, surface, chk)
	-- Smoothen the surface.
	t:set_chunk(chunk.x, chunk.z, chk)
	for x = chunk.x-1,chunk.x+w do
		for z = chunk.z-1,chunk.z+w do
			t:calculate_smooth_normals(x, z)
		end
	end
	-- Generate object.
	self:generate_plants(chunk, surface)
	self:generate_objects(chunk, surface)
end

--- Generates the default objects.
-- @param self PlaceDefault.
-- @param chunk TerrainChunk.
-- @param surface Surface data.
PlaceDefault.generate_objects = function(self, chunk, surface)
	-- Generate a civilization obstacle.
	if math.random() < 0.05 then
		-- Calculate the position.
		local w = chunk.manager.chunk_size
		local civ_x = math.random(0, w-1)
		local civ_z = math.random(0, w-1)
		local civ_y = surface:get_height_bilinear(civ_x + 0.5, civ_z + 0.5)
		local p = Vector(chunk.x + civ_x + 0.5, 0.0, chunk.z + civ_z + 0.5)
		p:multiply(chunk.manager.grid_size)
		p:add_xyz(0, civ_y, 0)
		-- Choose and create the obstacle.
		MapUtils:place_obstacle_by_category("civilization", p, math.random() * math.pi * 2)
		yield()
	end
end

--- Generates the default plants.
-- @param self PlaceDefault.
-- @param chunk TerrainChunk.
-- @param surface Surface data.
PlaceDefault.generate_plants = function(self, chunk, surface)
	local p = Vector()
	local w = chunk.manager.chunk_size
	local seeds = self.__generator.seeds
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
				local f = Noise:plasma_noise_2d(seeds[5] + 0.005 * cx, seeds[6] + 0.005 * cz, 2)
				-- Choose and create the obstacle.
				if r > f * 0.01 then
					MapUtils:place_obstacle_by_category("tree", p, math.random() * math.pi * 2)
				else
					MapUtils:place_obstacle_by_category("small-plant", p, math.random() * math.pi * 2)
				end
			end
		end
		yield()
	end
end

--- Generates the default terrain surface.
-- @param self PlaceDefault.
-- @param chunk TerrainChunk.
-- @param surface Surface data.
-- @param chk TerrainChunk data.
PlaceDefault.generate_terrain = function(self, chunk, surface, chk)
	local w = chunk.manager.chunk_size
	for z = 0,w-1 do
		for x = 0,w-1 do
			-- Calculate the corner heights.
			local a00,b00,c00 = surface:get(x, z)
			local a10,b10,c10 = surface:get(x + 1, z)
			local a01,b01,c01 = surface:get(x, z + 1)
			local a11,b11,c11 = surface:get(x + 1, z + 1)
			-- Generate the grass layer.
			if c00 > 0 or c10 > 0 or c01 > 0 or c11 > 0 then
				local y0,y1,y2,y3 = a00 + b00 + c00, a10 + b10 + c10, a01 + b01 + c01, a11 + b11 + c11
				chk:add_stick_corners(x, z, 0, 0, 0, 0, y0, y1, y2, y3, 2)
			end
			-- Generate the soil layer.
			if b00 > 0 or b10 > 0 or b01 > 0 or b11 > 0 then
				chk:add_stick_corners(x, z, 0, 0, 0, 0, a00 + b00, a10 + b10, a01 + b01, a11 + b11, 1)
			end
			-- Generate the rock layer.
			chk:add_stick_corners(x, z, 0, 0, 0, 0, a00, a10, a01, a11, 3)
			-- Generate ore veins.
			local ore = math.random() * 300
			if ore < 3 and ore > 0.1 then
				local y0,y1,y2,y3 = a00 + b00 + c00, a10 + b10 + c10, a01 + b01 + c01, a11 + b11 + c11
				local mat
				if math.random() < 0.75 then
					mat = Main.specs:find_random_by_category("TerrainMaterialSpec", "common ore")
				else
					mat = Main.specs:find_random_by_category("TerrainMaterialSpec", "rare ore")
				end
				chk:add_stick_corners(x, z, y0 - ore, y1 - ore, y2 - ore, y3 - ore, y0, y1, y2, y3, mat.id)
			end
		end
		yield()
	end
end

return PlaceDefault
