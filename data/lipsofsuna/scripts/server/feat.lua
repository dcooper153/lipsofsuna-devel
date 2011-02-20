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
	if anim.categories["melee"] or anim.categories["ranged"] or
	   anim.categories["self"] or anim.categories["touch"] then
		if args.target and args.attacker then
			args.target:impulse{impulse = args.attacker.rotation * Vector(0, 0, -100)}
		end
	end
	-- Influences.
	local info = self:get_info()
	info.influences.health = -self:calculate_damage(args)
	for k,v in pairs(info.influences) do
		if k == "health" then
			-- Increase or decrease health.
			if args.target then
				if info.influences.health < 0 then
					local armor = args.target.armor_class or 0
					local mult = math.max(0.2, 1 - armor)
					info.influences.health = info.influences.health * mult
				end
				args.target:damaged(-info.influences.health)
			end
		elseif k == "plague" then
			-- Summon plagued beasts.
			for i = 1,math.random(1, math.ceil(v + 0.01)) do
				local s = Species:random{category = "plague"}
				if s then
					local p = args.point + Vector(
						-1 + 2 * math.random(),
						-1 + 2 * math.random(),
						-1 + 2 * math.random())
					local o = Creature{spec = s, position = p, random = true, realized = true}
					o:inflict_modifier("plague", 10000)
				end
			end
		elseif k == "sanctuary" then
			-- Increase sanctuary duration.
			if args.target then
				args.target:inflict_modifier("sanctuary", v)
			end
		end
	end
	-- Digging.
	if anim.categories["melee"] and args.tile and args.weapon then
		if args.weapon.spec.categories["mattock"] then
			Voxel:damage(args.tile, args.damage, "mattock")
		end
	end
	-- Building.
	if anim.categories["build"] and args.tile and args.weapon then
		if args.weapon.spec.construct_tile then
			local m = Material:find{name = args.weapon.spec.construct_tile}
			local need = args.weapon.spec.construct_tile_count or 1
			local have = args.weapon.count
			if m and need <= have then
				local t,p = Voxel:find_tile{match = "empty", point = args.point}
				if t then
					local tile = Aabb{point = p * Config.tilewidth, size = Vector(1, 1, 1) * Config.tilewidth}
					local char = Aabb{point = args.attacker.position - Vector(0.5, 0, 0.5), size = Vector(1.5, 2, 1.5)}
					if not tile:intersects(char) then
						local o = args.weapon:split{count = need}
						o:detach()
						Voxel:set_tile{point = p, tile = m.id}
					end
				end
			end
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
	local spec1 = args.weapon and args.weapon.spec
	local spec2 = args.projectile and args.projectile.spec
	local info = self:get_info()
	local damage = -(info.influences.health or 0)
	if spec1 or spec2 then
		damage = damage + (spec1 and spec1.damage or 0) + (spec2 and spec2.damage or 0)
	else
		damage = damage + 3
	end
	-- Damage bonus from skills.
	-- The bonus damage depends on the type of weapon and ammunition used.
	-- The hardcoded bare-handed bonuses only apply to melee attacks.
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
	elseif info.animation.categories["melee"] then
		bonuses = {
			dexterity = 0.2,
			strength = 0.2,
			willpower = 0.1}
	else
		bonuses = {}
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
			if anim.categories["build"] then
				-- Build terrain or machines.
				-- While the attack animation is played, a sphere is swept along
				-- the path of the attack point. If a tile collides with the
				-- sphere, a new tile attached to it.
				Thread(function(self)
					Thread:sleep(anim.action_frames[1] * 0.05)
					local src,dst = args.user:get_attack_ray(args)
					local r = args.user:sweep_sphere{src = src, dst = dst, radius = 0.1}
					if not r or not r.tile then return end
					feat:apply{
						attacker = args.user,
						point = r.point,
						target = r.object,
						tile = r.tile,
						weapon = weapon}
				end)
			end
			if anim.categories["melee"] then
				-- Melee attack.
				-- While the attack animation is played, a sphere is swept along
				-- the path of the attack point. The first object or tile that
				-- collides with the sphere is damaged.
				Thread(function(self)
					Thread:sleep(anim.action_frames[1] * 0.05)
					local src,dst = args.user:get_attack_ray(args)
					local r = args.user:sweep_sphere{src = src, dst = dst, radius = 0.1}
					if not r then return end
					feat:apply{
						attacker = args.user,
						point = r.point,
						target = r.object,
						tile = r.tile,
						weapon = weapon}
				end)
			end
			if anim.categories["ranged"] then
				-- Ranged attack.
				-- A projectile is fired at the specific time into the attack
				-- animation. The collision callback of the projectile takes
				-- care of damaging the hit object or tile.
				Thread(function(self)
					Thread:sleep(anim.action_frames[1] * 0.05)
					for name,count in pairs(info.required_ammo) do
						local ammo = args.user:split_items{name = name, count = count}
						if ammo then
							ammo:fire{collision = true, feat = feat, owner = args.user, weapon = weapon}
						end
					end
					for index,data in ipairs(feat.effects) do
						local effect = Feateffectspec:find{name = data[1]}
						if effect and effect.projectile then
							local ammo = Object{model = effect.projectile, physics = "rigid"}
							ammo.gravity = Vector()
							ammo:fire{collision = true, feat = feat, owner = args.user, weapon = weapon}
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
					local src = args.user:get_attack_ray(args)
					local point = args.user.position + args.user.rotation * src
					local proj = weapon:fire{
						collision = not weapon.spec.destroy_timer,
						feat = feat,
						point = point,
						owner = args.user,
						speed = 10,
						timer = weapon.spec.destroy_timer}
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
	-- Check for support by the species.
	if not args.user then return end
	local spec = args.user.spec
	if spec.type ~= "species" then return end
	if not spec.feat_anims[self.animation] then return end
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
		if not item or item.count < v then return end
	end
	-- Check for ammo.
	for k,v in pairs(info.required_ammo) do
		if not inventory then return end
		local item = inventory:find_object{name = k}
		if not item or item.count < v then return end
	end
	-- Check for weapon.
	if info.required_weapon then
		if not inventory then return info.required_weapon == "melee" end
		local weapon = inventory:get_object{slot = "hand.R"}
		if not weapon then return info.required_weapon == "melee" end
		if not weapon.spec.categories[info.required_weapon] then return end
	end
	return true
end

