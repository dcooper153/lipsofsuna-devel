--- Generic combat action.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module arena.feat
-- @alias Feat

local Class = require("system/class")
local Damage = require("core/server/damage")
local Serialize = require("system/serialize")

--- Generic combat action.
-- @type Feat
local Feat = Class("Feat")

--- Creates a new feat.
-- @param clss Feat class.
-- @param animation Feat type name.
-- @param effects List of effects and their magnitudes.
-- @return New feat.
Feat.new = function(clss, animation, effects)
	local self = Class.new(clss)
	self.animation = animation
	self.effects = effects or {}
	return self
end

--- Creates a copy of the feat.
-- @param feat Feat.
-- @return Feat.
Feat.copy = function(feat)
	local self = Class.new(Feat)
	self.animation = feat.animation
	self.effects = {}
	for k,v in pairs(feat.effects) do self.effects[k] = v end
	return self
end

--- Adds the best possible effects to the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>category: Category from which to pick effects.</li>
--   <li>user: Object whose skills and inventory to use.</li></ul>
Feat.add_best_effects = function(self, args)
	-- Add usable feat effects.
	-- TODO: Reject effects if this will be used for animations that can have too many.
	local anim = Feattypespec:find{name = self.animation}
	for name in pairs(args.user:get_known_spell_effects()) do
		if anim.effects[name] then
			local effect = Feateffectspec:find{name = name}
			if effect and (not args.category or effect.categories[args.category]) then
				self.effects[#self.effects + 1] = {name, 1}
			end
		end
	end
end

--- Applies the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>object: Attacked object, or nil.</li>
--   <li>owner: Attacking actor.</li>
--   <li>point: Hit point in world space.</li>
--   <li>projectile: Projectile object, or nil.</li>
--   <li>tile: Attacked tile, or nil.</li>
--   <li>weapon: Used weapon object, or nil.</li></ul>
Feat.apply = function(self, args)
	self:play_effects_impact(args)
	self:apply_touch(args)
end

Feat.apply_impulse = function(self, args)
	if args.object and args.owner then
		args.object.physics:impulse(Vector(0,0,-100):transform(args.owner:get_rotation()))
	end
end

Feat.apply_block_penalty = function(self, args)
	-- Increase the melee cooldown if the target is blocking.
	if args.owner and args.object and args.object.blocking then
		if Program:get_time() - args.object.blocking > args.object.spec.blocking_delay then
			args.owner.cooldown = (args.owner.cooldown or 0) * 2
			args.owner:animate("stagger")
		end
	end
end

Feat.apply_digging = function(self, args)
	if not args.tile then return end
	-- Break the tile.
	if args.weapon and args.weapon.spec.categories["mattock"] then
		Voxel:damage(args.owner, args.tile)
	end
	-- Damage the weapon.
	if args.weapon and args.weapon.spec.damage_mining then
		if not args.weapon:damaged{amount = 2 * args.weapon.spec.damage_mining * math.random(), type = "mining"} then
			args.owner:send_message("The " .. args.weapon.spec.name .. " broke!")
		end
	end
end

Feat.apply_ranged = function(self, args)
	local ret = true
	local info = self:get_info(args)
	for k,v in pairs(info.influences) do
		local i = Feateffectspec:find{name = k}
		if i and i.ranged then
			args.feat = self
			args.info = info
			args.value = v
			if not i:ranged(args) then ret = nil end
		end
	end
	return ret
end

Feat.apply_touch = function(self, args)
	local info = self:get_info(args)
	for k,v in pairs(info.influences) do
		local i = Feateffectspec:find{name = k}
		if i and i.touch then
			args.feat = self
			args.info = info
			args.value = v
			i:touch(args)
		end
	end
end

--- Gets the icon of the feat.
-- @param self Feat.
-- @return Iconspec or nil.
Feat.get_icon = function(self)
	local icon = nil
	for i = 1,3 do
		local effect = self.effects[i]
		if effect then
			spec = Feateffectspec:find{name = effect[1]}
			icon = spec and Iconspec:find{name = spec.icon}
			if icon then break end
		end
	end
	return icon
end

--- Gets the skill and reagent requirements of the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>charge: Charge time of the attack.</li>
--   <li>object: Attacked object, or nil.</li>
--   <li>owner: Attacking actor.</li>
--   <li>point: Hit point in world space.</li>
--   <li>projectile: Fired object.</li>
--   <li>weapon: Used weapon.</li></ul>
-- @return Feat info table.
Feat.get_info = function(self, args)
	-- Get the feat type.
	local anim = Feattypespec:find{name = self.animation}
	if not anim then return end
	-- Get the feat information.
	local info = anim:get_casting_info()
	for index,data in pairs(self.effects) do
		local effect = Feateffectspec:find{name = data[1]}
		if effect then effect:get_casting_info(info) end
	end
	-- Get the ammo requirements.
	local ammo = anim.required_ammo
	if anim.required_ammo == "WEAPON" then
		ammo = args and args.weapon and args.weapon.spec.ammo_type
	end
	-- Set the base damage.
	local damage = Damage(influences)
	-- Add weapon-specific influences.
	if args and anim.bonuses_weapon then
		damage:add_item_influences(args.weapon, args.owner.skills)
	end
	-- Add bare-handed-specific influences.
	if args and not args.weapon and anim.bonuses_barehanded then
		damage:add_barehanded_influences(args.owner.skills)
	end
	-- Add projectile-specific influences.
	if anim.bonuses_projectile then
		if args and args.projectile then
			damage:add_item_influences(args.projectile, args.owner.skills)
		elseif ammo then
			damage:add_itemspec_influences(Itemspec:find{name = ammo}, args.owner.skills)
		end
	end
	-- Apply attacker modifiers.
	if args then
		if anim.categories["melee"] then
			damage:apply_attacker_physical_modifiers(args.owner)
		end
		if args.charge and (anim.categories["melee"] or anim.categories["ranged"]) then
			damage:apply_attacker_charge(args.charge)
		end
	end
	-- Apply defender modifiers.
	if args and args.object then
		damage:apply_defender_armor(args.object)
		damage:apply_defender_vulnerabilities(args.object)
	end
	-- Set the total health influence.
	local influences = damage.influences
	local health = damage:get_total_health_influence()
	if health ~= 0 then
		damage.influences.health = health
	end
	-- Return the final results.
	return {
		animation = anim,
		cooldown = info.cooldown or 0,
		influences = damage.influences,
		required_ammo = ammo and {[ammo] = 1} or {},
		required_reagents = info.reagents or {},
		required_stats = info.stats or {},
		required_weapon = anim and anim.required_weapon}
end

--- Creates a new feat from a data string.
-- @param clss Feat class.
-- @param args Arguments.<ul>
--  <li>data: Data string.</li></ul>
-- @return Feat or nil.
Feat.load = function(clss, args)
	if not args.data then return end
	local handler = function(err) print(debug.traceback("ERROR: " .. err)) end
	-- Compile the string.
	local func,err1 = loadstring(args.data)
	if not func then return handler(err1) end
	-- Execute the code.
	local err2,ret = xpcall(func, handler)
	return ret
end

--- Performs a feat
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>user: Object using the feat. (required)</li>
--   <li>stop: True if stopped performing, false if started.</li></ul>
-- @return True if performed successfully.
Feat.perform = function(self, args)
	local anim = Feattypespec:find{name = self.animation}
	local slot = anim and (anim.slot or (anim.required_weapon and args.user.spec.weapon_slot))
	local weapon = slot and args.user.inventory:get_object_by_slot(slot)
	local info = anim and self:get_info{owner = args.user, weapon = weapon}
	-- Check for cooldown and requirements.
	if info and not args.stop then
		if args.user.cooldown then return end
		if not self:usable(args) then return end
		if not args.user.spec.reagentless_spells then
			for k,v in pairs(info.required_reagents) do
				args.user.inventory:subtract_objects_by_name(k, v)
			end
		end
		local w = info.required_stats["willpower"]
		if w then args.user.stats:subtract("willpower", w) end
		if info.cooldown > 0 then
			args.user.cooldown = info.cooldown
		end
	end
	-- Calculate the charge time.
	args.charge = args.user:get_attack_charge()
	-- Call the feat function.
	local move
	if not info or anim.toggle or not args.stop then
		args.weapon = weapon
		if anim then
			local a = Actionspec:find_by_name(anim.action)
			if a then move = a.func(self, info, args) end
		end
		if self.func then self:func(args) end
	end
	-- Emit a vision event.
	Server:object_event(args.user, "object-feat", {anim = anim, move = move})
	return true
end

--- Plays the effects of the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>user: Object using the feat. (required)</li>
--   <li>stop: True if stopped performing, false if started.</li></ul>
Feat.play_effects = function(self, args)
	local anim = Feattypespec:find{name = self.animation}
	if not anim then return end
	Server:object_effect(args.user, anim.effect)
	if anim.action ~= "melee" and anim.slot then
		local weapon = args.user.inventory:get_object_by_slot(anim.slot)
		if weapon and weapon.spec.effect_attack then
			Server:object_effect(args.user, weapon.spec.effect_attack)
		end
	end
end

--- Plays the impact effects of the feat.
-- @param self Feat.
-- @param args Arguments.
Feat.play_effects_impact = function(self, args)
	local effects = {}
	-- Add the effect from the spell type.
	local anim = Feattypespec:find{name = self.animation}
	if anim and anim.effect_impact then
		effects[anim.effect_impact] = true
	end
	-- Add effects from spell effects.
	for index,data in ipairs(self.effects) do
		local effect = Feateffectspec:find{name = data[1]}
		if effect and effect.effect then
			effects[effect.effect] = true
		end
	end
	-- Play each effect.
	for effect in pairs(effects) do
		Server:world_effect(args.point, effect)
	end
end

--- Checks if the feat can be used with the given skills.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>skills: Skills of the user.</li>
--   <li>inventory: Inventory of the user.</li>
--   <li>user: Object whose skills and inventory to use.</li></ul>
-- @return True if usable.
Feat.usable = function(self, args)
	-- Check that the user is valid.
	if not args.user then return end
	local spec = args.user.spec
	if spec.type ~= "actor" then return end
	-- Check that the feat type exists.
	local anim = Feattypespec:find{name = self.animation}
	if not anim then
		return false, "No such feat exists."
	end
	-- Check that the actor supports the feat.
	local known_types = args.user:get_known_spell_types()
	if not known_types[self.animation] then
		return false, "The spell type has not been unlocked."
	end
	local known_effects = args.user:get_known_spell_effects()
	for k,v in pairs(self.effects) do
		if not known_effects[v[1]] then
			return false, "The spell effect has not been unlocked."
		end
	end
	-- Calculate requirements.
	local weapon = args.user.inventory:get_object_by_slot(anim.slot)
	local info = self:get_info{owner = args.user, weapon = weapon}
	-- Check for stats.
	local stats = args.stats or args.user.stats
	for k,v in pairs(info.required_stats) do
		if not stats then return end
		local val = stats:get_value(k)
		if not val or val < v then
			return false, "Not enough willpower."
		end
	end
	-- Check for reagents.
	local inventory = args.inventory or args.user.inventory
	if not args.user.spec.reagentless_spells then
		for k,v in pairs(info.required_reagents) do
			local count = inventory:count_objects_by_name(k)
			if count < v then
				return false, "Not enough reagents."
			end
		end
	end
	-- Check for ammo.
	for k,v in pairs(info.required_ammo) do
		local count = inventory:count_objects_by_name(k)
		if count < v then
			return false, "Not enough ammo."
		end
	end
	-- Check for the weapon.
	if info.required_weapon then
		local weapon = inventory:get_object_by_slot(args.user.spec.weapon_slot)
		if not weapon then
			if info.required_weapon ~= "melee" then
				return false, "No weapon equipped."
			end
		elseif not weapon.spec.categories[info.required_weapon] then
			return false, "Incorrect weapon equipped."
		end
	end
	return true
end

--- Saves the feat to a data string.
-- @param self Feat.
-- @return String.
Feat.write = function(self)
	return string.format("return Feat%s", Serialize:write{
		animation = self.animation,
		effects = self.effects})
end

return Feat
