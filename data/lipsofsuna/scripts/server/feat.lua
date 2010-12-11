--- Applies the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>attacker: Attacking creature.</li>
--   <li>point: Hit point in world space.</li>
--   <li>target: Attacked creature.</li>
--   <li>tile: Attacked tile or nil.</li>
--   <li>weapon: Used weapon.</li></ul>
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
		Effect:play{effect = effect, point = args.point}
	end
	-- Impulse.
	if args.target and args.attacker then
		args.target:impulse{impulse = args.attacker.rotation * Vector(0, 0, -100)}
	end
	-- Damage target.
	if args.target then
		local damage = self:calculate_damage(args)
		args.target:damaged(damage)
	end
	if args.tile then
		if args.weapon and args.weapon.itemspec.name == "mattock" then
			Voxel:damage(args.tile, args.damage, "mattock")
		end
	end
end

--- Calculates the damage of the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>attacker: Attacking creature.</li>
--   <li>point: Hit point in world space.</li>
--   <li>projectile: Fired object or nil.</li>
--   <li>target: Attacked creature or nil.</li>
--   <li>weapon: Used weapon or nil.</li></ul>
-- @return Damage.
Feat.calculate_damage = function(self, args)
	-- Base damage.
	-- The base damage depends on the feat and the type of weapon and ammunition used.
	local spec1 = args.weapon and args.weapon.itemspec
	local spec2 = args.projectile and args.projectile.itemspec
	local info = self:get_info()
	local damage = info.inflict_damage
	if spec1 or spec2 then
		damage = damage + (spec1 and spec1.damage or 0) + (spec2 and spec2.damage or 0)
	else
		damage = damage + 3
	end
	-- Damage bonus from skills.
	-- The bonus damage depends on the type of weapon and ammunition used.
	local bonuses
	local skills = args.attacker and args.attacker.skills
	if not skills then return damage end
	if spec1 or spec2 then
		spec1 = spec1 or {}
		spec2 = spec2 or {}
		bonuses = {
			dexterity = (spec1.damage_bonus_dexterity or 0) + (spec2.damage_bonus_dexterity or 0),
			health = (spec1.damage_bonus_health or 0) + (spec2.damage_bonus_health or 0),
			intelligence = (spec1.damage_bonus_intelligence or 0) + (spec2.damage_bonus_intelligence or 0),
			perception = (spec1.damage_bonus_percention or 0) + (spec2.damage_bonus_percention or 0),
			strength = (spec1.damage_bonus_strength or 0) + (spec2.damage_bonus_strength or 0),
			willpower = (spec1.damage_bonus_willpower or 0) + (spec2.damage_bonus_willpower or 0)}
	else
		bonuses = {
			dexterity = 0.2,
			strength = 0.2,
			willpower = 0.1}
	end
	for k,v in pairs(bonuses) do
		if skills:has_skill{skill = k} then
			damage = damage + v * skills:get_value{skill = k}
		end
	end
	return damage
end

--- Performs a feat
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>user: Object using the feat. (required)</li>
--   <li>stop: True if stopped performing, false if started.</li></ul>
Feat.perform = function(self, args)
	local feat = self
	local anim = Featanimspec:find{name = feat.animation}
	local info = anim and feat:get_info()
	-- Check for cooldown and requirements.
	if info and not args.stop then
		if args.user.cooldown then return end
		if not feat:usable(args) then return end
		for k,v in pairs(info.required_reagents) do
			args.user:subtract_items{name = k, count = v}
		end
		local w = info.required_skills["willpower"]
		if w then Skills:subtract{owner = args.user, skill = "willpower", value = w} end
		if info.cooldown > 0 then
			args.user.cooldown = info.cooldown
		end
	end
	-- Playback effects.
	if info and not args.stop then
		if anim.effect then
			Effect:play{effect = anim.effect, object = args.user}
		end
		if anim.animation then
			args.user:animate{animation = anim.animation, weight = 100.0}
		end
	end
	-- Call the feat function.
	if not info or anim.toggle or not args.stop then
		if anim then
			local slot = anim.slot
			local weapon = args.user:get_item{slot = slot}
			if anim.categories["melee"] then
				-- Melee attack.
				-- While the attack animation is played, a sphere is swept along
				-- the path of the attack point. The first object or tile that
				-- collides with the sphere is damaged.
				Attack:sweep{user = args.user, slot = slot,
					start = anim.action_frames[1] * 0.05,
					stop = anim.action_frames[2] * 0.05,
					func = function(f, r)
					feat:apply{
						attacker = args.user,
						point = r.point,
						target = r.object,
						tile = r.tile,
						weapon = weapon}
				end}
			end
			if anim.categories["ranged"] then
				-- Ranged attack.
				-- A projectile is fired at the specific time into the attack
				-- animation. The collision callback of the projectile takes
				-- care of damaging the hit object or tile.
				Thread(function(self)
					Thread:sleep(anim.action_frames[1] * 0.05)
					local point = Attack:find_blade_point{object = args.user, slot = slot} + Vector(0, 0.3, -1.5)
					for name,count in pairs(info.required_ammo) do
						local ammo = args.user:split_items{name = name, count = count}
						if ammo then
							ammo:fire{collision = true, feat = feat, point = point,
								owner = args.user, weapon = weapon}
						end
					end
					for index,data in ipairs(feat.effects) do
						local effect = Feateffectspec:find{name = data[1]}
						if effect and effect.projectile then
							local ammo = Object{model = effect.projectile, physics = "rigid"}
							ammo.gravity = Vector()
							ammo:fire{collision = true, feat = feat, point = point,
								owner = args.user, weapon = weapon}
						end
					end
				end)
			end
			if anim.categories["self"] then
				-- Target self.
				-- At the specific time into the attack animation, the effects of the
				-- feat are applied to the attacker herself.
				Thread(function(self)
					Thread:sleep(anim.action_frames[1] * 0.05)
					feat:apply{
						attacker = args.user,
						point = args.user.position,
						target = args.user,
						weapon = weapon}
				end)
			end
			if anim.categories["throw"] then
				-- Throw attack.
				-- The weapon is fired at the specific time into the attack
				-- animation. The collision callback of the projectile takes
				-- care of damaging the hit object or tile.
				Thread(function(self)
					Thread:sleep(anim.action_frames[1] * 0.05)
					local point = Attack:find_blade_point{object = args.user, slot = slot} + Vector(0, 0.3, -1.5)
					local proj = weapon:fire{
						collision = not weapon.itemspec.destroy_timer,
						feat = feat,
						point = point + Vector(0.2, 0.3, -1.5),
						owner = args.user,
						speed = 10,
						timer = weapon.itemspec.destroy_timer}
				end)
			end
		end
		if feat.func then feat:func(args) end
	end
end

--- Unlocks a random feat to the player base.
-- @param self Feat class.
-- @param args Arguments.<ul>
--   <li>category: Category name or nil.</li></ul>
-- @return Feat or nil.
Feat.unlock = function(clss, args)
	local choices = {}
	-- Find the unlocked feats.
	if args and args.category then
		local cat = clss.dict_cat[args.category]
		if not cat then return end
		for k,v in pairs(cat) do
			if v.locked then table.insert(choices, v) end
		end
	else
		for k,v in pairs(clss.dict_id) do
			if v.locked then table.insert(choices, v) end
		end
	end
	-- Choose a random feat and unlock it.
	if #choices == 0 then return end
	local feat = choices[math.random(1,#choices)]
	feat.locked = false
	-- Inform clients.
	local packet = Packet(packets.FEAT_UNLOCK, "string", feat.name)
	for k,v in pairs(Player.clients) do
		v:send{packet = packet}
	end
	return feat
end

--- Checks if the feat can be used with the given skills.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>skills: Skills of the user.</li>
--   <li>inventory: Inventory of the user.</li>
--   <li>user: Object whose skills and inventory to use.</li></ul>
-- @return True if usable.
Feat.usable = function(self, args)
	-- Check for support by the species.
	local spec = args.species or args.user and args.user.species
	if spec and not spec.feats[self.animation] then return end
	-- Get feat information.
	local info = self:get_info()
	-- Check for skills.
	local skills = args.skills or Skills:find{owner = args.user}
	for k,v in pairs(info.required_skills) do
		if not skills then return end
		local val = skills:get_value{skill = k}
		if not val or val < v then return end
	end
	-- Check for reagents.
	local inventory = args.inventory or Inventory:find{owner = args.user}
	for k,v in pairs(info.required_reagents) do
		if not inventory then return end
		local item = inventory:find_object{name = k}
		if not item or item:get_count() < v then return end
	end
	-- Check for ammo.
	for k,v in pairs(info.required_ammo) do
		if not inventory then return end
		local item = inventory:find_object{name = k}
		if not item or item:get_count() < v then return end
	end
	-- Check for weapon.
	if info.required_weapon then
		if not inventory then return info.required_weapon == "melee" end
		local weapon = inventory:get_object{slot = "hand.R"}
		if not weapon then return info.required_weapon == "melee" end
		if not weapon.itemspec.categories[info.required_weapon] then return end
	end
	return true
end

