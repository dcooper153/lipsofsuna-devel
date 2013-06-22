--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.damage
-- @alias Damage

local Class = require("system/class")

--- TODO:doc
-- @type Damage
local Damage = Class("Damage")

--- Creates a new damage instance.
-- @param clss Damage class.
-- @param influences Dictionary of influences, or nil.
-- @return Damage.
Damage.new = function(clss, influences)
	local self = Class.new(clss)
	self.influences = influences or {}
	return self
end

--- Adds influences for bare-handed combat.<br/>
--
-- Bare-handed combat has a static base damage that may be amplified by
-- the skills of the attacker.
--
-- @param self Damage.
-- @param skills Attacker's skills, or nil.
Damage.add_barehanded_influences = function(self, skills)
	-- Calculate the damage multiplier.
	local mult = 1
	if skills then
		mult = skills:calculate_damage_multiplier_for_unarmed()
	end
	-- Add the multiplied influences.
	local bonuses = {["physical damage"] = 3}
	for k,v in pairs(bonuses) do
		self.influences[k] = (self.influences[k] or 0) + mult * v
	end
end

--- Adds influences for the given item.
-- @param self Damage.
-- @param weapon Item.
-- @param skills Attacker's skills, or nil.
Damage.add_item_influences = function(self, weapon, skills)
	if not weapon then return end
	self:add_itemspec_influences(weapon.spec, skills)
end

--- Adds influences for the given itemspec.<br/>
--
-- Equipped items may provide additional influences. The magnitude of these
-- influences may depend on the skills of the player as some weapons may be
-- configured to grant bonuses for having certain skills.
--
-- @param self Damage.
-- @param spec Itemspec.
-- @param skills Attacker's skills, or nil.
Damage.add_itemspec_influences = function(self, spec, skills)
	if not spec then return end
	if not spec.influences then return end
	-- Calculate the damage multiplier.
	local mult = 1
	if skills then
		mult = skills:calculate_damage_multiplier_for_itemspec(spec)
	end
	-- Add the multiplied influences.
	for k,v in pairs(spec.influences) do
		self.influences[k] = (self.influences[k] or 0) + mult * v
	end
end

--- Modifies the damage based on the charging of the attacker.<br/>
--
-- Holding the attack button down allows players to perform more powerful attacks.
-- The damage reaches its maximum after 2 seconds of charging, at which point it
-- it will be doubled.
--
-- @param self Damage.
-- @param charge Charge time in seconds, or nil.
Damage.apply_attacker_charge = function(self, charge)
	local p = self.influences["physical damage"]
	if p and p > 0 then
		local f = 1 + math.min(1.5, charge)
		self.influences["physical damage"] = p * f
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
		local p = self.influences["physical damage"]
		if p and p < 0 then
			local h = object.stats:get_value("health")
			local f = 3 - 2 * math.min(h, 25) / 25
			self.influences["physical damage"] = p * f
		end
	end
end

--- Modifies the damage based on the armor and blocking status of the target.<br/>
--
-- Only a limited number of influence types is affected by this. The rest
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
	-- Reduce influences based on the blocking bonus.
	local mult = math.max(0.0, 1 - armor)
	for k,v in pairs(self.influences) do
		if reduce[k] then
			local prev = self.influences[k]
			if prev < 0 then
				self.influences[k] = prev * mult
			end
		end
	end
end

--- Modifies the damage based on the vulnerabilities of the defender.<br/>
--
-- Influences to which the defender is immune are removed from the list
-- here, and the rest are multiplied my the resistence of the defender.
--
-- @param self Damage.
-- @param object Defender object.
Damage.apply_defender_vulnerabilities = function(self, object)
	-- Get the vulnerabilities.
	if not object then return end
	if not object.spec.vulnerabilities then return end
	local vuln = object.spec.vulnerabilities
	-- Multiply the influences.
	for name,value in pairs(self.influences) do
		self.influences[name] = value * (vuln[name] or 1)
	end
end

--- Gets the effects that should be played when the damage lands.
-- @param self Damage.
-- @return Dictionary of effect names.
Damage.get_impact_effects = function(self)
	local res = {}
	for name,value in pairs(self.influences) do
		local effect = Feateffectspec:find{name = name}
		if effect and effect.effect then
			res[effect.effect] = true
		end
	end
	return res
end

--- Gets the total damage to the health stat.
-- @param self Damage.
-- @return Number.
Damage.get_total_health_influence = function(self)
	local mask = {["cold damage"] = 1, ["fire damage"] = 1, ["physical damage"] = 1, ["poison"] = 1}
	local health = 0
	for k,v in pairs(self.influences) do
		if mask[k] then
			health = health + v
		end
	end
	return health
end

return Damage


