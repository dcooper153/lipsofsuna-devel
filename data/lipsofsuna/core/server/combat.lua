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
local Damage = require("arena/damage") --FIXME
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
	damage:add_item_or_unarmed_modifiers(weapon, attacker.skills)
	damage:add_knockback()
	damage:apply_attacker_physical_modifiers(attacker)
	damage:apply_attacker_charge(attacker:get_attack_charge())
	damage:apply_defender_armor(defender)
	damage:apply_defender_blocking(defender)
	damage:apply_defender_vulnerabilities(defender)
	-- Apply object damage.
	if defender then
		Main.combat_utils:apply_damage_to_actor(attacker, defender, damage, point)
	else
		Main.combat_utils:apply_damage_to_terrain(attacker, tile, damage, point)
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
	damage:apply_defender_armor(defender)
	damage:apply_defender_vulnerabilities(defender)
	-- Apply object damage.
	if defender then
		Main.combat_utils:apply_damage_to_actor(attacker, defender, damage, point)
	else
		Main.combat_utils:apply_damage_to_terrain(attacker, tile, damage, point)
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
	damage:add_item_modifiers(weapon, attacker.skills)
	damage:add_item_modifiers(projectile, attacker.skills)
	damage:add_knockback()
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
	-- Calculate the damage.
	local damage = Damage()
	damage:add_spell_modifiers(projectile.modifiers)
	damage:apply_defender_vulnerabilities(defender)
	-- Knockback the defender.
	if defender then
		defender.physics:impulse(Vector(0,0,-100):transform(projectile:get_rotation()))
	end
	-- Apply the damage.
	if defender then
		Main.combat_utils:apply_damage_to_actor(attacker, defender, damage, point)
	else
		Main.combat_utils:apply_damage_to_terrain(attacker, tile, damage, point)
	end
	return not absorb
end

return Combat
