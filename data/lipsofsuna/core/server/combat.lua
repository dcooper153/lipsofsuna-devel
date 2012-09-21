--- Damage calculation and applying helpers.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.combat
-- @alias Combat

local Class = require("system/class")
local Damage = require("core/server/damage")
local Item = require("core/objects/item")

--- Damage calculation and applying helpers.
-- @type Combat
local Combat = Class("Combat")

--- Calculates and applies the damage of a melee impact.
-- @param self Combat class.
-- @param attacker Actor who cast the spell.
-- @param weapon Item used for the attack, or nil.
-- @param point Impact point in world space.
-- @param defender Hit object, or nil.
-- @param tile Hit tile, or nil.
Combat.apply_melee_impact = function(self, attacker, weapon, point, defender, tile)
	-- Calculate the damage.
	local damage = Damage()
	if weapon then
		damage:add_item_influences(weapon, attacker.skills)
	else
		damage:add_barehanded_influences(attacker.skills)
	end
	damage:apply_attacker_physical_modifiers(attacker)
	damage:apply_attacker_charge(attacker:get_attack_charge())
	if defender then
		damage:apply_defender_armor(defender)
		damage:apply_defender_vulnerabilities(defender)
	end
	-- Play impact effects.
	for name in pairs(damage:get_impact_effects()) do
		Server:world_effect(point, name)
	end
	-- Apply object damage.
	if defender then
		-- Knockback the defender.
		defender.physics:impulse(Vector(0,0,-100):transform(attacker:get_rotation()))
		-- Stagger the attacker.
		if defender.blocking then
			if Program:get_time() - defender.blocking > defender.spec.blocking_delay then
				attacker.cooldown = (attacker.cooldown or 0) + 1
				attacker:animate("stagger")
			end
		end
		-- Damage the defender.
		local args = {owner = attacker, object = defender}
		for name,value in pairs(damage.influences) do
			local effect = Feateffectspec:find{name = name}
			if effect and effect.touch then
				args.value = value
				effect:touch(args)
			end
		end
	end
	-- Apply tile damage.
	if tile and weapon then
		-- Break the tile.
		if weapon.spec.categories["mattock"] then
			self:destroy_terrain_sphere(attacker, point, tile, 2)
			return
		end
		-- Damage the weapon.
		if weapon.spec.damage_mining then
			if not weapon:damaged{amount = 2 * weapon.spec.damage_mining * math.random(), type = "mining"} then
				attacker:send_message("The " .. weapon.spec.name .. " broke!")
			end
		end
	end
end

--- Applies the damage of a ranged impact.
-- @param self Combat class.
-- @param attacker Actor who cast the spell.
-- @param projectile Item used as the projectile.
-- @param damage Damage information.
-- @param point Impact point in world space.
-- @param defender Hit object, or nil.
-- @param tile Hit tile, or nil.
Combat.apply_ranged_impact = function(self, attacker, projectile, damage, point, defender, tile)
	-- Apply defender's status.
	if defender then
		damage:apply_defender_armor(defender)
		damage:apply_defender_vulnerabilities(defender)
	end
	-- Play impact effects.
	for name in pairs(damage:get_impact_effects()) do
		Server:world_effect(point, name)
	end
	-- Apply object damage.
	if defender then
		-- Knockback the defender.
		defender.physics:impulse(Vector(0,0,-100):transform(projectile:get_rotation()))
		-- Damage the defender.
		local args = {owner = attacker, object = defender}
		for name,value in pairs(damage.influences) do
			local effect = Feateffectspec:find{name = name}
			if effect and effect.touch then
				args.value = value
				effect:touch(args)
			end
		end
	end
end

--- Calculates the damage of a ranged impact.
-- @param self Combat class.
-- @param attacker Actor who cast the spell.
-- @param weapon Item used for the attack, or nil.
-- @param projectile Item used as the projectile.
-- @return Damage information.
Combat.calculate_ranged_damage = function(self, attacker, weapon, projectile)
	local damage = Damage()
	if weapon then
		damage:add_item_influences(weapon, attacker.skills)
	end
	if projectile then
		damage:add_item_influences(projectile, attacker.skills)
	end
	damage:apply_attacker_charge(attacker:get_attack_charge())
	return damage
end

--- Applies the effect of a ranged spell on impact.
-- @param self Combat class.
-- @param attacker Actor who cast the spell.
-- @param projectile Spell object used as the projectile.
-- @param effect Effectspec name.
-- @param point Impact point in world space.
-- @param defender Hit object, or nil.
-- @param tile Hit tile, or nil.
-- @return True if the effect is still alive.
Combat.apply_ranged_spell_impact = function(self, attacker, projectile, effect, point, defender, tile)
	-- Find the effect spec.
	local espec = Feateffectspec:find{name = effect}
	if not espec then return end
	-- Calculate the damage.
	local damage = Damage(espec.influences)
	damage:apply_defender_vulnerabilities(defender)
	-- Play impact effects.
	for name in pairs(damage:get_impact_effects()) do
		Server:world_effect(point, name)
	end
	-- Knockback the defender.
	if defender then
		defender.physics:impulse(Vector(0,0,-100):transform(projectile:get_rotation()))
	end
	-- Apply the damage.
	local absorb
	local args = {owner = attacker, object = defender, tile = tile}
	for name,value in pairs(damage.influences) do
		local effect = Feateffectspec:find{name = name}
		if effect and effect.ranged then
			args.value = value
			if not effect:ranged(args) then
				absorb = true
			elseif absorb == nil then
				absorb = false
			end
		end
	end
	return not absorb
end

--- Destroys terrain at the given impact point.
-- @param self Combat class.
-- @param attacker Actor who destroyed terrain.
-- @param point Impact point in world space.
-- @param tile Grid coordinates of the hit tile.
-- @param radius Radius of the destroyed sphere, in grid units.
Combat.destroy_terrain_sphere = function(self, attacker, point, tile, radius)
	-- FIXME: Old terrain.
	if tile.y ~= 0 then
		Voxel:damage(attacker, tile)
		return
	end
	-- Erase sticks.
	local materials = {}
	for x,z,y,y00,y10,y01,y11 in Game.terrain.terrain:get_sticks_in_sphere(point, radius) do
		local yavg = (y00 + y10 + y01 + y11) / 4
		Game.terrain.terrain:count_column_materials(x, z, y - yavg, yavg * 2, materials)
		Game.terrain.terrain:add_stick_corners(x, z,
			y - y00, y - y10, y - y01, y - y11,
			y + y00, y + y10, y + y01, y + y11, 0)
	end
	-- Smoothen the modified columns.
	Game.terrain.terrain:calculate_smooth_normals_in_sphere(point, radius)
	-- Play the collapse effect.
	self:__play_terrain_destruction_effect(point, materials)
	-- Create items.
	self:__create_terrain_mining_items(attacker, materials)
	-- Spawn random monsters.
	-- TODO: Should use material specs.
		--[[
		if not n and math.random() < 0.01 then
			local spec = Actorspec:random{category = "mining"}
			if spec then
				local offset = (point + Vector(0.5,0.1,0.5)) * Voxel.tile_size
				local object = Actor{random = true, spec = spec, position = offset, realized = true}
			end
		end
		--]]
	return true
end

--- Destroys terrain at the given impact point.
-- @param self Combat class.
-- @param attacker Actor who destroyed terrain.
-- @param point Impact point in world space.
-- @param tile Grid coordinates of the hit tile.
Combat.destroy_terrain_stick = function(self, attacker, point, tile, height)
	-- Erase sticks.
	local materials = Game.terrain.terrain:count_column_materials(tile.x, tile.z, point.y - height / 2, height)
	Game.terrain.terrain:add_stick(tile.x, tile.z, point.y - height / 2, height, 0)
	-- Smoothen the modified columns.
	for z = tile.z-1,tile.z+1 do
		for x = tile.x-1,tile.x+1 do
			Game.terrain.terrain:calculate_smooth_normals(x, z)
		end
	end
	-- Play the collapse effect.
	self:__play_terrain_destruction_effect(point, materials)
	-- Create items.
	self:__create_terrain_mining_items(attacker, materials)
	return true
end

--- Creates inventory items for mined materials.
-- @param self Combat class.
-- @param point Point in world units.
-- @param radius Radius in world units.
Combat.__create_terrain_mining_items = function(self, attacker, materials)
	for k,v in pairs(materials) do
		local mat = TerrainMaterialSpec:find_by_id(k)
		if mat and mat.mining_item and v >= 0.5 then
			local spec = Itemspec:find_by_name(mat.mining_item)
			local item = Item{spec = spec}
			item:set_count(math.floor(v + 0.5))
			attacker.inventory:merge_or_drop_object(item)
		end
	end
end

--- Plays the terrain destruction effect.
-- @param self Combat class.
-- @param point Impact point in world space.
-- @param materials Dictionary of destroyed materials.
Combat.__play_terrain_destruction_effect = function(self, point, materials)
	local effect = "collapse2"
	local best_k,best_v
	for k,v in pairs(materials) do
		if k ~= 0 and (not best_k or best_v < v) then
			best_k,best_v = k,v
		end
	end
	if best_k then
		local mat = TerrainMaterialSpec:find_by_id(best_k)
		if mat and mat.effect_collapse then
			effect = mat.effect_collapse
		end
	end
	Server:world_effect(point, effect)
end

return Combat
