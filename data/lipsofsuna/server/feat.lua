local oldinfo = Feat.get_info

--- Adds the best possible effects to the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>category: Category from which to pick effects.</li>
--   <li>user: Object whose skills and inventory to use.</li></ul>
Feat.add_best_effects = function(self, args)
	-- Solves the maximum value the effect can have without the feat
	-- becoming unusable. The solution is found by bisecting.
	local solve_effect_value = function(feat, effect)
		local i = #feat.effects + 1
		local e = {effect.name, 0}
		local step = 50
		feat.effects[i] = e
		repeat
			e[2] = e[2] + step
			if not feat:usable{user = args.user} then
				e[2] = e[2] - step
			end
			step = step / 2
		until step < 1
		feat.effects[i] = nil
		return e[2]
	end
	-- Add usable feat effects.
	-- TODO: Reject effects if this will be used for animations that can have too many.
	local anim = Featanimspec:find{name = self.animation}
	for name in pairs(args.user.spec.feat_effects) do
		if anim.effects[name] then
			local effect = Feateffectspec:find{name = name}
			if effect and (not args.category or effect.categories[args.category]) then
				local value = solve_effect_value(self, effect)
				if value >= 1 then self.effects[#self.effects + 1] = {name, value} end
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
	-- Effects.
	local effects = {}
	local anim = Featanimspec:find{name = self.animation}
	if anim and anim.effect_impact then
		effects[anim.effect_impact] = true
	end
	for index,data in ipairs(self.effects) do
		local effect = Feateffectspec:find{name = data[1]}
		if effect and effect.effect then
			effects[effect.effect] = true
		end
	end
	for effect in pairs(effects) do
		if args.object then
			Effect:play{effect = effect, object = args.object}
		else
			Effect:play{effect = effect, point = args.point}
		end
	end
	-- Impulse.
	if anim.categories["melee"] or anim.categories["ranged"] or
	   anim.categories["self"] or anim.categories["touch"] then
		if args.object and args.owner then
			args.object:impulse{impulse = args.owner.rotation * Vector(0, 0, -100)}
		end
	end
	-- Cooldown.
	-- The base cooldown has already been applied but we add some extra
	-- if the object was blocking in order to penalize rampant swinging.
	if anim.categories["melee"] and args.owner and args.object and args.object.blocking then
		if Program.time - args.object.blocking > args.object.spec.blocking_delay then
			args.owner.cooldown = (args.owner.cooldown or 0) * 2
		end
	end
	-- Influences.
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
	-- Digging.
	if anim.categories["melee"] and args.tile then
		-- Break the tile.
		if (args.weapon and args.weapon.spec.categories["mattock"]) or math.random(1, 5) == 5 then
			Voxel:damage(args.owner, args.tile)
		end
		-- Damage the weapon.
		if args.weapon and args.weapon.spec.damage_mining then
			if not args.weapon:damaged{amount = 2 * args.weapon.spec.damage_mining * math.random(), type = "mining"} then
				args.owner:send{packet = Packet(packets.MESSAGE, "string",
					"The " .. args.weapon.spec.name .. " broke!")}
			end
		end
	end
	-- Building.
	if anim.categories["build"] and args.tile and args.weapon then
		if args.weapon.spec.construct_tile then
			local m = Material:find{name = args.weapon.spec.construct_tile}
			local need = args.weapon.spec.construct_tile_count or 1
			local have = args.weapon.count
			if m and need <= have then
				local t,p = Utils:find_build_point(args.point, args.owner)
				if t then
					local o = args.weapon:split(need)
					o:detach()
					Voxel:set_tile(p, m.id)
					if m.effect_build then
						Effect:play{effect = m.effect_build, point = p * Voxel.tile_size}
					end
				end
			end
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
	local anim = Featanimspec:find{name = self.animation}
	local slot = anim and (anim.slot or (anim.required_weapon and args.user.spec.weapon_slot))
	local weapon = slot and args.user.inventory:get_object_by_slot(slot)
	local info = anim and self:get_info{owner = args.user, weapon = weapon}
	-- Check for cooldown and requirements.
	if info and not args.stop then
		if args.user.cooldown then return end
		if not self:usable(args) then return end
		for k,v in pairs(info.required_reagents) do
			args.user.inventory:subtract_objects_by_name(k, v)
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
	-- Animate the feat.
	-- The move parameter is used by melee feats to tell which attack animation was used.
	Vision:event{type = "object-feat", object = args.user, anim = anim, move = move}
	return true
end

--- Plays the effects of the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>user: Object using the feat. (required)</li>
--   <li>stop: True if stopped performing, false if started.</li></ul>
Feat.play_effects = function(self, args)
	local anim = Featanimspec:find{name = self.animation}
	if not anim then return end
	if anim.effect then
		Effect:play{effect = anim.effect, object = args.user}
	end
	if anim.action ~= "melee" and anim.slot then
		local weapon = args.user.inventory:get_object_by_slot(anim.slot)
		if weapon and weapon.spec.effect_attack then
			Effect:play{effect = weapon.spec.effect_attack, object = args.user}
		end
	end
end

--- Unlocks a random feat to the player base.
-- @param self Feat class.
-- @param args Arguments.<ul>
--   <li>category: Category name or nil.</li></ul>
-- @return Feat or nil.
Feat.unlock = function(clss, args)
	print("Warning: unlocking feats isn't implemented, nor are there any locked feats.")
	return nil
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
	if spec.type ~= "species" then return end
	-- Check that the feat type exists.
	local anim = Featanimspec:find{name = self.animation}
	if not anim then
		return false, "No such feat exists."
	end
	-- Check that the actor supports the feat.
	if args.user.client then
		if not spec.feat_anims[self.animation] and
		   not Unlocks:get("spell type", self.animation) then
			return false, "The spell type has not been unlocked."
		end
		for k,v in pairs(self.effects) do
			if not spec.feat_effects[v[1]] and
			   not Unlocks:get("spell effect", v[1]) then
				return false, "The spell effect has not been unlocked."
			end
		end
	else
		if not spec.feat_anims[self.animation] then
			return false, "Not doable by your race."
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
	for k,v in pairs(info.required_reagents) do
		local count = inventory:count_objects_by_name(k)
		if count < v then
			return false, "Not enough reagents."
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

