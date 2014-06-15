--- Building utils.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module building.building_utils
-- @alias BuildingUtils

local Class = require("system/class")
local Item = require("core/objects/item")

--- Building utils.
-- @type BuildingUtils
local BuildingUtils = Class("BuildingUtils")

--- Creates new building utils.
-- @param clss BuildingUtils class.
-- @return BuildingUtils.
BuildingUtils.new = function(clss)
	local self = Class.new(clss)
	return self
end

--- Destroys terrain at the given impact point.
-- @param self BuildingUtils.
-- @param attacker Actor who destroyed terrain.
-- @param point Impact point in world space.
-- @param radius Radius of the destroyed sphere, in grid units.
BuildingUtils.destroy_terrain_sphere = function(self, attacker, point, radius)
	-- Erase sticks.
	local materials = {}
	for x,z,y,y00,y10,y01,y11 in Main.terrain.terrain:get_sticks_in_sphere(point, radius) do
		local yavg = (y00 + y10 + y01 + y11) / 4
		Main.terrain.terrain:count_column_materials(x, z, y - yavg, yavg * 2, materials)
		Main.terrain.terrain:add_stick_corners(x, z,
			y - y00, y - y10, y - y01, y - y11,
			y + y00, y + y10, y + y01, y + y11, 0)
	end
	-- Smoothen the modified columns.
	Main.terrain.terrain:calculate_smooth_normals_in_sphere(point, radius)
	-- Play the collapse effect.
	self:__play_terrain_destruction_effect(point, materials)
	-- Create items.
	if attacker then
		self:__create_terrain_mining_items(attacker, materials)
	end
	-- Spawn random monsters.
	-- TODO: Should use material specs.
		--[[
		if not n and math.random() < 0.01 then
			local offset = (point + Vector(0.5,0.1,0.5)) * Voxel.tile_size
			local object = Main.objects:create_object_by_spec_category("Actor", "mining")
			if object then
				object:set_position(offset)
				object:randomize()
				object:set_visible(true)
			end
		end
		--]]
	return true
end

--- Destroys terrain at the given impact point.
-- @param self BuildingUtils.
-- @param attacker Actor who destroyed terrain.
-- @param point Impact point in world space.
-- @param tile Grid coordinates of the hit tile.
BuildingUtils.destroy_terrain_stick = function(self, attacker, point, tile, height)
	-- Erase sticks.
	local materials = Main.terrain.terrain:count_column_materials(tile.x, tile.z, point.y - height / 2, height)
	Main.terrain.terrain:add_stick(tile.x, tile.z, point.y - height / 2, height, 0)
	-- Smoothen the modified columns.
	for z = tile.z-1,tile.z+1 do
		for x = tile.x-1,tile.x+1 do
			Main.terrain.terrain:calculate_smooth_normals(x, z)
		end
	end
	-- Play the collapse effect.
	self:__play_terrain_destruction_effect(point, materials)
	-- Create items.
	self:__create_terrain_mining_items(attacker, materials)
	return true
end

--- Creates inventory items for mined materials.
-- @param self BuildingUtils.
-- @param point Point in world units.
-- @param radius Radius in world units.
BuildingUtils.__create_terrain_mining_items = function(self, attacker, materials)
	for k,v in pairs(materials) do
		local mat = Main.specs:find_by_id("TerrainMaterialSpec", k)
		if mat and mat.mining_item and v >= 0.1 then
			local spec = Main.specs:find_by_name("ItemSpec", mat.mining_item)
			local item = Item(Main.objects)
			item:set_spec(spec)
			item:set_count(math.floor(v / 10) * 10 + 0.1)
			attacker.inventory:merge_or_drop_object(item)
		end
	end
end

--- Plays the terrain destruction effect.
-- @param self BuildingUtils.
-- @param point Impact point in world space.
-- @param materials Dictionary of destroyed materials.
BuildingUtils.__play_terrain_destruction_effect = function(self, point, materials)
	local effect = "collapse2"
	local best_k,best_v
	for k,v in pairs(materials) do
		if k ~= 0 and (not best_k or best_v < v) then
			best_k,best_v = k,v
		end
	end
	if best_k then
		local mat = Main.specs:find_by_id("TerrainMaterialSpec", best_k)
		if mat and mat.effect_collapse then
			effect = mat.effect_collapse
		end
	end
	Main.vision:world_effect(point, effect)
end

return BuildingUtils
