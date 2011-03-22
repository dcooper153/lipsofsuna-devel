local oldinfo = Feat.get_info

--- Applies the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>attacker: Attacking creature.</li>
--   <li>point: Hit point in world space.</li>
--   <li>projectile: Projectile.</li>
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
		if args.target then
			Effect:play{effect = effect, object = args.target}
		else
			Effect:play{effect = effect, point = args.point}
		end
	end
	-- Impulse.
	if anim.categories["melee"] or anim.categories["ranged"] or
	   anim.categories["self"] or anim.categories["touch"] then
		if args.target and args.attacker then
			args.target:impulse{impulse = args.attacker.rotation * Vector(0, 0, -100)}
		end
	end
	-- Cooldown.
	-- The base cooldown has already been applied but we add some extra
	-- if the target was blocking in order to penalize rampant swinging.
	if anim.categories["melee"] and args.attacker and args.target and args.target.blocking then
		if Program.time - args.target.blocking > args.target.spec.blocking_delay then
			args.attacker.cooldown = args.attacker.cooldown * 2
		end
	end
	-- Influences.
	local info = self:get_info(args)
	for k,v in pairs(info.influences) do
		if k == "health" then
			-- Increase or decrease health.
			if args.target then
				-- Randomize the amount.
				local val = info.influences.health
				if val < 0 then
					val = math.min(-1, val + val * 0.5 * math.random())
				else
					val = math.max(1, val + val * 0.5 * math.random())
				end
				-- Apply unless friendly fire.
				if val > 0 or not args.attacker.client or not args.target.client then
					args.target:damaged(-val)
				end
				-- Anger hurt creatures.
				if info.influences.health < 0 then
					args.target:add_enemy(args.attacker)
				end
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
		elseif k == "wilderness" then
			-- Calculate spell radius.
			local s = math.min(v, 100) / 100
			local r = 4 * s
			if args.attacker.spec.categories["devora"] then
				r = math.ceil(r * 1.5)
			end
			r = 2 + math.max(r,1)
			-- Create grass from soil.
			local soil = Material:find{name = "soil1"}
			local grass = Material:find{name = "grass1"}
			local org = (args.point * Voxel.tile_scale):floor()
			local pos = Vector()
			local hits = {}
			for x=org.x-r,org.x+r do
				pos.x = x
				for z=org.z-r,org.z+r do
					pos.z = z
					local blocked = true
					for y=org.y+r+1,org.y-r,-1 do
						pos.y = y
						local tile = Voxel:get_tile(pos)
						if not blocked then
							if tile == soil.id then
								Voxel:set_tile(pos, grass.id)
							end
							if tile == soil.id or tile == grass.id then
								local vec = Vector(x,y,z)
								table.insert(hits, vec)
							end
						end
						blocked = (tile ~= 0)
					end
				end
			end
			-- Create wilderness obstacles.
			if #hits then
				for i=1,math.ceil(s/25) do
					local k = math.random(1,#hits)
					local p = hits[k]
					if p then
						hits[k] = nil
						local obstspec = Obstaclespec:random{category = "wilderness"}
						if obstspec then
							local pos = (p + Vector(0.5,0.5,0.5)) * Voxel.tile_size
							if Utils:check_room(pos, obstspec.model) then
								local o = Obstacle{position = pos, spec = obstspec,
									random = true, realized = true}
							end
						end
					end
				end
			end
		end
	end
	-- Digging.
	if anim.categories["melee"] and args.tile then
		-- Break the tile.
		if (args.weapon and args.weapon.spec.categories["mattock"]) or math.random(1, 5) == 5 then
			Voxel:damage(args.attacker, args.tile)
		end
		-- Damage the weapon.
		if args.weapon and args.weapon.spec.damage_mining then
			if not args.weapon:damaged(2 * args.weapon.spec.damage_mining * math.random()) then
				args.attacker:send{packet = Packet(packets.MESSAGE, "string",
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
				local t,p = Voxel:find_tile{match = "empty", point = args.point}
				if t then
					local tile = Aabb{point = p * Config.tilewidth, size = Vector(1, 1, 1) * Config.tilewidth}
					local char = Aabb{point = args.attacker.position - Vector(0.5, 0, 0.5), size = Vector(1.5, 2, 1.5)}
					if not tile:intersects(char) then
						local o = args.weapon:split{count = need}
						o:detach()
						Voxel:set_tile(p, m.id)
					end
				end
			end
		end
	end
end

--- Gets the skill and reagent requirements of the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>attacker: Attacking creature.</li>
--   <li>point: Hit point in world space.</li>
--   <li>projectile: Fired object or nil.</li>
--   <li>target: Attacked creature or nil.</li>
--   <li>weapon: Used weapon or nil.</li></ul>
-- @return Feat info table.
Feat.get_info = function(self, args)
	-- Get the feat specific requirements and influences.
	local info = oldinfo(self)
	local health_influences = {cold = 1, fire = 1, physical = 1, poison = 1}
	-- Add weapon specific influences.
	-- In addition to the base influences, weapons may grant bonuses for
	-- having points in certain skills. The skill bonuses are multiplicative
	-- since the system is easier to balance that way.
	if args and args.weapon then
		if args.weapon.spec.influences_base then
			local mult = 1
			local bonuses = args.weapon.spec.influences_bonus
			if bonuses and args.attacker.skills then
				for k,v in pairs(bonuses) do
					local s = args.attacker.skills:get_value{skill = k}
					if s then mult = mult * (1 + v * s) end
				end
			end
			for k,v in pairs(args.weapon.spec.influences_base) do
				local prev = info.influences[k]
				info.influences[k] = (prev or 0) + mult * v
			end
		end
	-- Add bare-handed specific influences.
	-- Works like the weapon variant but uses hardcoded influences.
	-- Bare-handed influence bonuses only apply to melee feats.
	elseif args and info.animation.categories["melee"] then
		local mult = 1
		local bonuses = {dexterity = 0.02, strength = 0.01, willpower = 0.02}
		if args.attacker.skills then
			for k,v in pairs(bonuses) do
				local s = args.attacker.skills:get_value{skill = k}
				if s then mult = mult * (1 + v * s) end
			end
		end
		local influences = {physical = -3}
		for k,v in pairs(influences) do
			local prev = info.influences[k]
			info.influences[k] = (prev or 0) + mult * v
		end
	end
	-- Add projectile specific influences.
	-- Works like the weapon variant but uses the projectile as the item.
	if args and args.projectile and args.projectile.spec.influences_base then
		local mult = 1
		local bonuses = args.projectile.spec.influences_bonus
		if bonuses then
			for k,v in pairs(bonuses) do
				local s = skills:get_value{skill = k}
				if s then mult = mult * (1 + v * s) end
			end
		end
		for k,v in pairs(args.projectile.spec.influences_base) do
			local prev = info.influences[k]
			info.influences[k] = (prev or 0) + mult * v
		end
	end
	-- Apply target armor and blocking.
	-- Only a limited number of influence types is affects by this.
	-- Positive influences that would increase stats are never blocked.
	if args and args.target then
		local reduce = {cold = true, fire = true, physical = true}
		local armor = args.target.armor_class or 0
		if args.target.blocking then
			local delay = args.target.spec.blocking_delay
			local elapsed = Program.time - args.target.blocking
			local frac = math.min(1, elapsed / delay)
			armor = armor + frac * args.target.spec.blocking_armor
		end
		local mult = math.max(0.0, 1 - armor)
		for k,v in pairs(info.influences) do
			if reduce[k] then
				local prev = info.influences[k]
				if prev < 0 then
					info.influences[k] = prev * mult
				end
			end
		end
	end
	-- Apply target vulnerabilities.
	-- Individual influences are multiplied by the vulnerability coefficients
	-- of the target and summed together to the total health influence.
	local vuln = args and args.target and args.target.spec.vulnerabilities
	if not vuln then vuln = health_influences end
	local health = info.influences.health or 0
	for k,v in pairs(info.influences) do
		local mult = vuln[k] or health_influences[k]
		if mult then
			local val = v * mult
			info.influences[k] = val 
			health = health + val
		end
	end
	-- Set the total health influence.
	-- This is the actual value added to the health of the target. The
	-- individual influence components remain in the table may be used
	-- by special feats but they aren't used for regular health changes.
	if health ~= 0 then
		info.influences.health = health
	end
	return info
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
		Vision:event{type = "object-feat", object = args.user, anim = anim}
	end
	-- Call the feat function.
	if not info or anim.toggle or not args.stop then
		if anim then
			local slot = anim.slot
			local weapon = args.user:get_item{slot = slot}
			if anim.categories["build"] then
				-- Build terrain or machines.
				-- While the attack animation is played, an attack ray is cast.
				-- If a tile collides with the ray, a new tile is attached to it.
				Thread(function(self)
					Thread:sleep(args.user.spec.timing_build * 0.05)
					local src,dst = args.user:get_attack_ray(args)
					local r = Physics:cast_ray{src = src, dst = dst}
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
				-- While the attack animation is played, an attack ray is cast.
				-- The first object or tile that collides with the ray is damaged.
				Thread(function(self)
					Thread:sleep(args.user.spec.timing_attack_blunt * 0.05)
					local src,dst = args.user:get_attack_ray(args)
					local r = Physics:cast_ray{src = src, dst = dst}
					if not r then return end
					feat:apply{
						attacker = args.user,
						point = r.point,
						target = r.object,
						tile = r.tile,
						weapon = weapon}
				end)
			end
			if anim.categories["explode"] then
				-- Self-destruction.
				-- The creature explodes after the animation has played.
				Thread(function(self)
					Thread:sleep(args.user.spec.timing_attack_explode * 0.05)
					args.user:die()
					Utils:explosion(args.user.position)
				end)
			end
			if anim.categories["ranged"] then
				-- Ranged attack.
				-- A projectile is fired at the specific time into the attack
				-- animation. The collision callback of the projectile takes
				-- care of damaging the hit object or tile.
				Thread(function(self)
					if anim.categories["spell"] then
						Thread:sleep(args.user.spec.timing_spell_ranged * 0.05)
					else
						Thread:sleep(args.user.spec.timing_attack_ranged * 0.05)
					end
					for name,count in pairs(info.required_ammo) do
						local ammo = args.user:split_items{name = name, count = count}
						if ammo then
							ammo:fire{collision = true, feat = feat, owner = args.user, weapon = weapon}
							return
						end
					end
					for index,data in ipairs(feat.effects) do
						local effect = Feateffectspec:find{name = data[1]}
						if effect and effect.projectile then
							local ammo = Object{model = effect.projectile, physics = "rigid"}
							ammo.gravity = Vector()
							ammo:fire{collision = true, feat = feat, owner = args.user, weapon = weapon}
							return
						end
					end
				end)
			end
			if anim.categories["self"] then
				-- Target self.
				-- At the specific time into the attack animation, the effects of the
				-- feat are applied to the attacker herself.
				Thread(function(self)
					Thread:sleep(args.user.spec.timing_spell_self * 0.05)
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
					Thread:sleep(args.user.spec.timing_attack_throw * 0.05)
					local proj = weapon:fire{
						collision = not weapon.spec.destroy_timer,
						feat = feat,
						owner = args.user,
						speed = 10,
						timer = weapon.spec.destroy_timer}
				end)
			end
			if anim.categories["touch"] then
				-- Touch spell.
				-- While the attack animation is played, an attack ray is cast.
				-- The first object or tile that collides with the ray is damaged.
				Thread(function(self)
					Thread:sleep(args.user.spec.timing_spell_touch * 0.05)
					local src,dst = args.user:get_attack_ray(args)
					local r = Physics:cast_ray{src = src, dst = dst}
					if not r then return end
					feat:apply{
						attacker = args.user,
						point = r.point,
						target = r.object,
						tile = r.tile,
						weapon = weapon}
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

