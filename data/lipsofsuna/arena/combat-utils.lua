--- Combat utils.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module arena.combat_utils
-- @alias CombatUtils

local Class = require("system/class")
local Modifier = require("core/server/modifier")
local ModifierSpec = require("core/specs/modifier")

--- Combat utils.
-- @type CombatUtils
local CombatUtils = Class("CombatUtils")

--- Creates new combat utils.
-- @param clss CombatUtils class.
-- @return CombatUtils.
CombatUtils.new = function(clss)
	local self = Class.new(clss)
	return self
end

--- Applies the damage to the actor.
-- @param self CombatUtils.
-- @param caster Actor.
-- @param target Actor.
-- @param damage Damage.
-- @param point Hit point in world space. Nil for target position.
-- @return True if all the modifiers were absorbed.
CombatUtils.apply_damage_to_actor = function(self, caster, target, damage, point)
	-- Play impact effects.
	if not point then
		point = target:get_position()
	end
	for name in pairs(damage:get_impact_effects()) do
		Main.vision:world_effect(point, name)
	end
	-- Knockback the defender.
	if damage.knockback then
		target.physics:impulse(Vector(0,0,-100):transform(caster:get_rotation()))
	end
	-- Stagger the attacker.
	if damage.blocking then
		if Program:get_time() - target.blocking > target.spec.blocking_delay then
			caster.cooldown = (caster.cooldown or 0) + 1
			caster:animate("stagger")
		end
	end
	-- Apply the damage.
	--
	-- For each modifier type in the damage, we create a new modifier and
	-- let it modifier the target object. Modifiers may either apply their
	-- effects immediately or request us to add them to the target object so
	-- that they can perform effect-over-time updates.
	local absorb = true
	for name,value in pairs(damage.modifiers) do
		local spec = ModifierSpec:find_by_name(name)
		if spec then
			local modifier = Modifier(spec, target, caster, point)
			local ret = modifier:start(value)
			if ret then
				absorb = false
				if ret == true then
					target:add_modifier(modifier)
				end
			end
		end
	end
	return not absorb
end

--- FIXME
CombatUtils.apply_damage_to_terrain = function(self, caster, tile, damage, point)
	-- Play impact effects.
	for name in pairs(damage:get_impact_effects()) do
		Main.vision:world_effect(point, name)
	end
	-- Break the tile.
	--[[
	if weapon.spec.categories["mattock"] then
		self:destroy_terrain_sphere(attacker, point, tile, 1.5)
		return
	end
	-- Damage the weapon.
	if weapon.spec.damage_mining then
		if not weapon:damaged{amount = 2 * weapon.spec.damage_mining * math.random(), type = "mining"} then
			attacker:send_message("The " .. weapon.spec.name .. " broke!")
		end
	end]]
end

--- Checks if the actor is wielding a ranged weapon.
-- @param self CombatUtils.
-- @param actor Actor.
-- @return Weapon and ammo count if wielding. Nil otherwise.
CombatUtils.count_ranged_ammo_of_actor = function(self, actor, ammo)
	-- Check for weapon.
	local weapon = actor:get_weapon()
	if not weapon then return end
	if not weapon.spec.ammo_type then return end
	-- Split the ammo.
	local ammo = actor.inventory:count_objects_by_name(weapon.spec.ammo_type, 1)
	if ammo < 1 then return weapon end
	return weapon,ammo
end

--- Splits a projectile for the currenly wielded ranged weapon.
-- @param self CombatUtils.
-- @param actor Actor.
-- @return Weapon and ammo items if succeeded. Weapon and nil if not enough ammo. Nil otherwise.
CombatUtils.split_ranged_ammo_of_actor = function(self, actor)
	-- Check for weapon.
	local weapon = actor:get_weapon()
	if not weapon then return end
	if not weapon.spec.ammo_type then return end
	-- Split the ammo.
	local ammo = actor.inventory:split_object_by_name(weapon.spec.ammo_type, 1)
	if not ammo then return weapon end
	return weapon,ammo
end

--- Subtracts the stats of the actor.
-- @param self CombatUtils.
-- @param actor Actor.
-- @param modifiers Modifiers.
-- @return True if subtracted successfully. False and stat name if not enough stats.
CombatUtils.subtract_modifier_stats_for_actor = function(self, actor, modifiers)
	-- Get the actor stats.
	local stats = actor.stats
	if not stats then return true end
	-- Get the require stats.
	local required = {}
	for k,v in pairs(modifiers) do
		local spec = ModifierSpec:find_by_name(k)
		if spec then
			spec:get_required_stats(required)
		end
	end
	-- Check that all requirements are met.
	for k,v in pairs(required) do
		local val = stats:get_value(k)
		if not val or val < v then
			return false,k
		end
	end
	-- Subtract the actor stats.
	for k,v in pairs(required) do
		stats:subtract(k, v)
	end
	return true
end

--- Calculates the armor class of the actor.
-- @param self CombatUtils.
-- @param actor Actor.
-- @return Number.
CombatUtils.get_actor_armor_class = function(self, actor)
	-- Get the inventory.
	local inv = actor.inventory
	if not inv then return 0 end
	-- Get the actor spec.
	local spec = actor:get_spec()
	if not spec then return 0 end
	-- Add the armor class from each equipped item.
	local value = 0
	for slot in pairs(spec.equipment_slots) do
		local item = inv:get_object_by_slot(slot)
		if item and item.spec and item.spec.armor_class then
			value = value + item.spec.armor_class
		end
	end
	return value
end

--- Gets the world space position of the weapon of the actor.
-- @param self CombatUtils.
-- @param actor Actor.
-- @param position Relative displacement vector. Nil for default displacement.
-- @return Vector in world space.
CombatUtils.get_actor_attack_point = function(self, actor, position)
	local ctr = actor.spec.aim_ray_center
	local tip
	if position then
		tip = Vector(0, 0, -actor.spec.aim_ray_start)
		tip:add(position)
	else
		tip = Vector(0, 0, -actor.spec.aim_ray_end)
	end
	if actor.tilt then tip = actor.tilt * tip end
	return actor:transform_local_to_global(ctr + tip)
end

--- Gets the combat action for the weapon of the actor or the actor itself.
-- @param self CombatUtils.
-- @param actor Actor.
-- @param hand Hand name.
-- @return Action spec if found. Nil otherwise.
CombatUtils.get_combat_action_for_actor = function(self, actor, hand)
	-- Check for weapon actions.
	local action = self:get_weapon_action_for_actor(actor, hand)
	if action then return action end
	-- Check for actor actions.
	local name = actor.spec.actions[hand]
	if not name then return end
	return Actionspec:find_by_name(name)
end

--- Gets the spell modifiers for the item.
-- @param self CombatUtils.
-- @param item item.
-- @return Dictionary of modifier names and values. Nil if none existed.
CombatUtils.get_spell_modifiers_for_item = function(self, item)
	-- Get the modifier names.
	local names = {}
	if item.spell_modifiers then
		for k,v in pairs(item.spell_modifiers) do
			names[k] = (names[k] or 0) + v
		end
	else
		for k,v in pairs(item.spec.modifiers) do
			names[k] = (names[k] or 0) + v
		end
	end
	-- Filter out invalid modifiers.
	local found
	local modifiers = {}
	for k,v in pairs(names) do
		local spec = ModifierSpec:find_by_name(k)
		if spec then
			modifiers[k] = v
			found = true
		end
	end
	return found and modifiers or nil
end

--- Gets the combat action for the weapon of the actor.
-- @param self CombatUtils.
-- @param actor Actor.
-- @param hand Hand name.
-- @return Action spec if found. Nil otherwise.
CombatUtils.get_weapon_action_for_actor = function(self, actor, hand)
	-- Get the weapon.
	local weapon = actor:get_weapon()
	if not weapon then return end
	-- Check for enchanted actions.
	if weapon.spell_action then
		local spec = Actionspec:find_by_name(weapon.spell_action)
		if spec then return spec end
	end
	-- Check for weapon actions.
	local name = weapon.spec.actions[hand]
	if not name then return end
	return Actionspec:find_by_name(name)
end

--- Calculates the armor class of the item.
-- @param self CombatUtils.
-- @param item Item.
-- @return Number.
CombatUtils.get_item_armor_class = function(self, item)
	if not item.spec then return 0 end
	if not item.spec.armor_class then return 0 end
	return item.spec.armor_class
end

--- Gets the weapon damage types of the item.
-- @param self CombatUtils.
-- @param actor Actor.
-- @param self Item.
-- @return Array of modifiers.
CombatUtils.get_item_attack_modifiers = function(self, actor, item)
	-- Get the item spec.
	local spec = item:get_spec()
	if not spec then return {} end
	if not spec.modifiers then return {} end
	-- Calculate the damage multiplier.
	local mult = 1
	if actor.skills then
		mult = actor.skills:calculate_damage_multiplier_for_item(item)
	end
	-- Calculate modifiers.
	local modifiers = {}
	for k,v in pairs(spec.modifiers) do
		modifiers[k] = mult * v
	end
	return modifiers
end

--- Calculates the usefulness rating of the item for the given actor.
-- @param self CombatUtils.
-- @param actor Actor.
-- @param self Item.
-- @return Number
CombatUtils.get_item_equip_value = function(self, actor, item)
	local score = 50 * self:get_item_armor_class(actor, item)
	for k,v in pairs(self:get_item_attack_modifiers(actor, item)) do
		if k ~= "hatchet" then
			score = score + v
		end
	end
	return score
end

--- Gets the name of the directional melee move for the actor.
-- @param self CombatUtils.
-- @param actor Actor.
-- @return String.
CombatUtils.get_melee_move_of_actor = function(self, actor)
	if actor:get_strafing() < -0.2 then
		return "left"
	elseif actor:get_strafing() > 0.2 then
		return "right"
	elseif actor:get_movement() < -0.2 then
		return "back"
	elseif actor:get_movement() > 0.2 then
		return "front"
	else
		return "stand"
	end
end

return CombatUtils
