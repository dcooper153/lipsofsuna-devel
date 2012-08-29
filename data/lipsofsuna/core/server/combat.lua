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
		defender:impulse{impulse = Vector(0,0,-100):transform(attacker:get_rotation())}
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
			Voxel:damage(args.owner, args.tile)
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
		defender:impulse{impulse = Vector(0,0,-100):transform(projectile:get_rotation())}
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
		defender:impulse{impulse = Vector(0,0,-100):transform(projectile:get_rotation())}
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

return Combat
