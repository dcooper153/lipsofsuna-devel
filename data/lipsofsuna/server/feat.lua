local oldinfo = Feat.get_info

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

