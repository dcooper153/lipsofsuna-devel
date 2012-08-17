Feat = Class()

--- Creates a new feat.
-- @param clss Feat class.
-- @param args Arguments.<ul>
--   <li>animation: Feat type name.</li>
--   <li>effects: List of effects and their magnitudes.</li></ul>
-- @return New feat.
Feat.new = function(clss, args)
	local self = Class.new(clss, args)
	self.effects = self.effects or {}
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
		args.object:impulse{impulse = args.owner.rotation * Vector(0, 0, -100)}
	end
end

Feat.apply_block_penalty = function(self, args)
	-- Increase the melee cooldown if the target is blocking.
	if args.owner and args.object and args.object.blocking then
		if Program.time - args.object.blocking > args.object.spec.blocking_delay then
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
	local damage = 0
	local reagents = {}
	local stats = {}
	local influences = {}
	local health_influences = {["cold damage"] = 1, ["fire damage"] = 1, ["physical damage"] = 1, poison = 1}
	-- Get the feat type.
	local anim = Feattypespec:find{name = self.animation}
	if not anim then return end
	local cooldown = anim.cooldown
	-- Influence contribution.
	for n,v in pairs(anim.influences) do
		influences[n] = (influences[n] or 0) + v
		if influences[n] == 0 then
			influences[n] = nil
		end
	end
	-- Effect contributions.
	for index,data in pairs(self.effects) do
		local effect = Feateffectspec:find{name = data[1]}
		if effect then
			-- Stat requirements.
			for stat,value in pairs(effect.required_stats) do
				local val = stats[stat] or 0
				stats[stat] = val + value
			end
			-- Reagent requirements.
			for reagent,value in pairs(effect.required_reagents) do
				local val = reagents[reagent] or 0
				reagents[reagent] = val + value
			end
			-- Cooldown contribution.
			cooldown = cooldown + effect.cooldown
			-- Influence contribution.
			for n,v in pairs(effect.influences) do
				influences[n] = (influences[n] or 0) + v
				if influences[n] == 0 then
					influences[n] = nil
				end
			end
		end
	end
	-- Ammo requirements.
	local ammo = anim.required_ammo
	if anim.required_ammo == "WEAPON" then
		ammo = args and args.weapon and args.weapon.spec.ammo_type
	end
	-- Stat requirements.
	for k,v in pairs(stats) do
		stats[k] = math.max(1, math.floor(v))
	end
	-- Reagent requirements.
	for k,v in pairs(reagents) do
		reagents[k] = math.max(1, math.floor(v))
	end
	-- Add weapon-specific influences.
	-- In addition to the base influences, weapons may grant bonuses for having
	-- points in certain skills. The skill bonuses are multiplicative since the
	-- system is easier to balance that way.
	if args and args.weapon and anim.bonuses_weapon and args.weapon.spec.influences then
		local mult = 1
		if args.owner.skills then
			mult = args.owner.skills:calculate_damage_multiplier_for_item(args.weapon)
		end
		for k,v in pairs(args.weapon.spec.influences) do
			influences[k] = (influences[k] or 0) + mult * v
		end
	end
	-- Add bare-handed-specific influences.
	-- Works like the weapon variant but uses hardcoded attributes and base damage.
	if args and not args.weapon and anim.bonuses_barehanded then
		local mult = 1
		if args.owner.skills then
			mult = args.owner.skills:calculate_damage_multiplier_for_unarmed()
		end
		local bonuses = {["physical damage"] = 3}
		for k,v in pairs(bonuses) do
			influences[k] = (influences[k] or 0) + mult * v
		end
	end
	-- Add projectile-specific influences.
	-- Works like the weapon variant but uses the projectile as the item.
	if anim.bonuses_projectile then
		local projspec = args and args.projectile and args.projectile.spec
		if not projspec and ammo then
			projspec = Itemspec:find{name = ammo}
		end
		if projspec and projspec.influences then
			local mult = 1
			if args.owner.skills and args.weapon then
				mult = args.owner.skills:calculate_damage_multiplier_for_item(args.weapon)
			end
			for k,v in pairs(projspec.influences) do
				influences[k] = (influences[k] or 0) + mult * v
			end
		end
	end
	-- Add berserk bonus.
	-- The bonus increases physical damage if the health of the owner is
	-- lower than 25 points. If the health is 1 point, the damage is tripled.
	if args and args.owner:get_modifier("berserk") then
		local p = influences["physical"]
		if anim.categories["melee"] and p and p < 0 then
			local h = args.owner.stats:get_value("health")
			local f = 3 - 2 * math.min(h, 25) / 25
			influences["physical"] = p * f
		end
	end
	-- Add charge bonus.
	-- Holding the attack button allows a power attack of a kind. The damage
	-- is doubled if charged to the maximum of 2 seconds.
	if args and args.charge then
		local p = influences["physical damage"]
		if (anim.categories["melee"] or anim.categories["ranged"]) and p and p > 0 then
			local f = 1 + math.min(1.5, args.charge / 2)
			influences["physical damage"] = p * f
		end
	end
	-- Apply object armor and blocking.
	-- Only a limited number of influence types is affected by this.
	-- Positive influences that would increase stats are never blocked.
	if args and args.object then
		local reduce = {["cold damage"] = true, ["fire damage"] = true, ["physical damage"] = true}
		local armor = args.object.armor_class or 0
		if args.object.blocking then
			local delay = args.object.spec.blocking_delay
			local elapsed = Program.time - args.object.blocking
			local frac = math.min(1, elapsed / delay)
			armor = armor + frac * args.object.spec.blocking_armor
		end
		local mult = math.max(0.0, 1 - armor)
		for k,v in pairs(influences) do
			if reduce[k] then
				local prev = influences[k]
				if prev < 0 then
					influences[k] = prev * mult
				end
			end
		end
	end
	-- Apply object vulnerabilities.
	-- Individual influences are multiplied by the vulnerability coefficients
	-- of the object and summed together to the total health influence.
	local vuln = args and args.object and args.object.spec.vulnerabilities
	if not vuln then vuln = health_influences end
	local health = influences.health or 0
	for k,v in pairs(influences) do
		local mult = vuln[k] or health_influences[k]
		if mult then
			local val = v * mult
			influences[k] = val 
			health = health + val
		end
	end
	-- Set the total health influence.
	-- This is the actual value added to the health of the object. The
	-- individual influence components remain in the table may be used
	-- by special feats but they aren't used for regular health changes.
	if health ~= 0 then
		influences.health = health
	end
	-- Return the final results.
	return {
		animation = anim,
		cooldown = cooldown,
		influences = influences,
		required_ammo = ammo and {[ammo] = 1} or {},
		required_reagents = reagents,
		required_stats = stats,
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
	if args.user.attack_charge then
		args.charge = Program.time - args.user.attack_charge
	end
	-- Call the feat function.
	local move
	if not info or anim.toggle or not args.stop then
		args.weapon = weapon
		if anim then
			local a = Actionspec:find{name = anim.action}
			if a then move = a.func(self, info, args) end
		end
		if self.func then self:func(args) end
	end
	-- Emit a vision event.
	Vision:event{type = "object-feat", object = args.user, anim = anim, move = move}
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
	return string.format("return Feat%s", serialize{
		animation = self.animation,
		effects = self.effects})
end
