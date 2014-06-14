--- Damage calculator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module arena.damage
-- @alias Damage

local Class = require("system/class")
local ModifierSpec = require("core/specs/modifier")

--- Damage calculator.
-- @type Damage
local Damage = Class("Damage")

--- Creates a new damage instance.
-- @param clss Damage class.
-- @param modifiers Dictionary of modifiers, or nil.
-- @return Damage.
Damage.new = function(clss, modifiers)
	local self = Class.new(clss)
	self.modifiers = modifiers or {}
	return self
end

--- Adds modifiers for bare-handed combat.<br/>
--
-- Bare-handed combat has a static base damage that may be amplified by
-- the skills of the attacker.
--
-- @param self Damage.
-- @param skills Attacker's skills. Nil if not available.
Damage.add_unarmed_modifiers = function(self, skills)
	-- Calculate the damage multiplier.
	local mult = 1
	if skills then
		mult = skills:calculate_damage_multiplier_for_unarmed()
	end
	-- Add the multiplied modifiers.
	local bonuses = {["physical damage"] = 3}
	for k,v in pairs(bonuses) do
		self.modifiers[k] = (self.modifiers[k] or 0) + mult * v
	end
end

--- Adds modifiers for the given item.
-- @param self Damage.
-- @param weapon Item.
-- @param skills Attacker's skills. Nil if not available.
Damage.add_item_modifiers = function(self, weapon, skills)
	if not weapon then return end
	self:add_itemspec_modifiers(weapon.spec, skills)
end

--- Adds modifiers for the given item or for barehanded if the item is nil.
-- @param self Damage.
-- @param weapon Item.
-- @param skills Attacker's skills. Nil if not available.
Damage.add_item_or_unarmed_modifiers = function(self, weapon, skills)
	if weapon then
		self:add_itemspec_modifiers(weapon.spec, skills)
	else
		self:add_unarmed_modifiers(skills)
	end
end

--- Adds modifiers for the given itemspec.<br/>
--
-- Equipped items may provide additional modifiers. The magnitude of these
-- modifiers may depend on the skills of the player as some weapons may be
-- configured to grant bonuses for having certain skills.
--
-- @param self Damage.
-- @param spec Itemspec.
-- @param skills Attacker's skills. Nil if not available.
Damage.add_itemspec_modifiers = function(self, spec, skills)
	if not spec then return end
	if not spec.modifiers then return end
	-- Calculate the damage multiplier.
	local mult = 1
	if skills then
		mult = skills:calculate_damage_multiplier_for_itemspec(spec)
	end
	-- Add the multiplied modifiers.
	for k,v in pairs(spec.modifiers) do
		self.modifiers[k] = (self.modifiers[k] or 0) + mult * v
	end
end

--- Enables the knockback effect.
-- @param self Damage.
Damage.add_knockback = function(self)
	self.knockback = true
end

--- Adds modifiers for a spell.
-- @param self Damage.
-- @param modifiers Dictionary of modifiers and their values.
-- @param skills Attacker's skills. Nil if not available.
Damage.add_spell_modifiers = function(self, modifiers, skills)
	if not modifiers then return end
	-- Calculate the damage multiplier.
	local mult = 1
	if skills then
		mult = skills:calculate_damage_multiplier_for_spells()
	end
	-- Add the multiplied modifiers.
	for k,v in pairs(modifiers) do
		self.modifiers[k] = (self.modifiers[k] or 0) + mult * v
	end
end

--- Modifies the damage based on the charging of the attacker.<br/>
--
-- Holding the attack button down allows players to perform more powerful attacks.
-- The damage reaches its maximum after 2 seconds of charging, at which point it
-- it will be doubled.
--
-- @param self Damage.
-- @param charge Charge time in seconds. Nil for no charge.
Damage.apply_attacker_charge = function(self, charge)
	if not charge then return end
	local p = self.modifiers["physical damage"]
	if p and p > 0 then
		local f = 1 + math.min(1.5, charge)
		self.modifiers["physical damage"] = p * f
	end
end

--- Modifies the damage based on various bonus modifiers of the attacker.<br/>
--
-- This currently only includes the berserk spell.
--
-- @param self Damage.
-- @param object Attacker object.
Damage.apply_attacker_physical_modifiers = function(self, object)
	if not object then return end
	if object:get_modifier("berserk") then
		local p = self.modifiers["physical damage"]
		if p and p < 0 then
			local h = object.stats:get_value("health")
			local f = 3 - 2 * math.min(h, 25) / 25
			self.modifiers["physical damage"] = p * f
		end
	end
end

--- Modifies the damage based on the armor and blocking status of the target.<br/>
--
-- Only a limited number of modifier types is affected by this. The rest
-- are considered to bypass the armor check.<br/>
--
-- Negative damage is never blocked, effectively meaning that healing always
-- has full magnitude regardless of the armor class.
--
-- @param self Damage.
-- @param object Defender object.
Damage.apply_defender_armor = function(self, object)
	if not object then return end
	if object:get_spec().type ~= "actor" then return end
	local reduce = {["cold damage"] = true, ["fire damage"] = true, ["physical damage"] = true}
	-- Calculate the blocking bonus.
	local armor = Main.combat_utils:get_actor_armor_class(object)
	if object.blocking then
		local delay = object.spec.blocking_delay
		local elapsed = Program:get_time() - object.blocking
		local frac = math.min(1, elapsed / delay)
		armor = armor + frac * object.spec.blocking_armor
	end
	-- Reduce modifiers based on the blocking bonus.
	local mult = math.max(0.0, 1 - armor)
	for k,v in pairs(self.modifiers) do
		if v > 0 and reduce[k] then
			self.modifiers[k] = v * mult
		end
	end
end

--- Applies the effects of the defender blocking to the attacker.<br/>
--
-- The attacker may stagger and suffer an extended cooldown if the
-- defender was blocking.
--
-- @param self Damage.
-- @param object Defender object.
Damage.apply_defender_blocking = function(self, object)
	if not object then return end
	if object.blocking then
		self.blocking = true
	end
end

--- Modifies the damage based on the vulnerabilities of the defender.<br/>
--
-- modifiers to which the defender is immune are removed from the list
-- here, and the rest are multiplied my the resistence of the defender.
--
-- @param self Damage.
-- @param object Defender object.
Damage.apply_defender_vulnerabilities = function(self, object)
	-- Get the vulnerabilities.
	if not object then return end
	if not object.spec.vulnerabilities then return end
	local vuln = object.spec.vulnerabilities
	-- Multiply the modifiers.
	for name,value in pairs(self.modifiers) do
		self.modifiers[name] = value * (vuln[name] or 1)
	end
end

--- Gets the effects that should be played when a projectile is created for the damage.
-- @param self Damage.
-- @return Dictionary of effect names.
Damage.get_firing_effects = function(self)
	local res = {}
	for name,value in pairs(self.modifiers) do
		local effect = Main.specs:find_by_name("ModifierSpec", name)
		if effect and effect.effect_fire then
			res[effect.effect_fire] = true
		end
	end
	return res
end

--- Gets the effects that should be played when the damage lands.
-- @param self Damage.
-- @param target Target object is available. Nil otherwise.
-- @return Dictionary of effect names.
Damage.get_impact_effects = function(self, target)
	local res = {}
	for name,value in pairs(self.modifiers) do
		local modifier = Main.specs:find_by_name("ModifierSpec", name)
		if modifier then
			-- Try actor specific effects.
			local effect
			if target and target.spec.get_effect and modifier.effect_target then
				local spec = target.spec:get_effect(modifier.effect_target, target.effect_profile)
				if spec then
					effect = spec.name
				end
			end
			-- Try modifier specific effects.
			if not effect then
				effect = modifier.effect
			end
			-- Add the effect if found.
			if effect then
				res[effect] = true
			end
		end
	end
	return res
end

--- Gets the total damage to the health stat.
-- @param self Damage.
-- @return Number.
Damage.get_total_health_modifier = function(self)
	local mask = {["cold damage"] = 1, ["fire damage"] = 1, ["physical damage"] = 1, ["poison"] = 1}
	local health = 0
	for k,v in pairs(self.modifiers) do
		if mask[k] then
			health = health + v
		end
	end
	return health
end

--- Multiplies all the influences by a constant.
-- @param self Damage.
-- @param multiplier Multiplier.
Damage.multiply = function(self, multiplier)
	for name,value in pairs(self.modifiers) do
		self.modifiers[name] = value * multiplier
	end
end

return Damage
