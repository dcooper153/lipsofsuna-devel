local oldinfo = Feat.get_info

--- Adds the best possible effects to the feat.
-- @param self Feat.
-- @param args Arguments.<ul>
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
			if effect then
				local value = solve_effect_value(self, effect)
				if value >= 1 then self.effects[#self.effects + 1] = {name, value} end
			end
		end
	end
end

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
		if k == "berserk" then
			-- Increase berserk duration.
			if args.target then
				args.target:inflict_modifier("berserk", v)
			end
		elseif k == "dig" then
			-- Dig terrain.
			if args.tile then
				Voxel:damage(args.attacker, args.tile)
			end
		elseif k == "firewall" then
			-- Firewall.
			local p = Utils:find_empty_ground(args.point)
			if p then
				local makewall = function(ctr, dir)
					for i=-2,2 do
						local w = ctr + dir * i * Voxel.tile_size
						local t = (w * Voxel.tile_scale + Vector(0,0.5,0)):floor()
						if Voxel:get_tile(t) == 0 and Voxel:get_tile(t - Vector(0,1)) ~= 0 then
							Spell{effect = "firewall", position = w, power = v}
						end
					end
				end
				local d = (p - args.attacker.position):abs()
				makewall(p, (d.x < d.z) and Vector(1) or Vector(0,0,1))
			end
		elseif k == "health" then
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
					args.target:damaged{amount = -val, point = args.point, type = "physical"}
				end
				-- Anger hurt creatures.
				if info.influences.health < 0 then
					args.target:add_enemy(args.attacker)
				end
			end
		elseif k == "light" then
			-- Increase light duration.
			if args.target then
				args.target:inflict_modifier("light", v)
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
			if not args.weapon:damaged{amount = 2 * args.weapon.spec.damage_mining * math.random(), type = "mining"} then
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
					local tile = Aabb{point = p * Voxel.tile_size, size = Vector(1, 1, 1) * Voxel.tile_size}
					local char = Aabb{point = args.attacker.position - Vector(0.5, 0, 0.5), size = Vector(1.5, 2, 1.5)}
					if not tile:intersects(char) then
						local o = args.weapon:split{count = need}
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
end

--- Performs a feat
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>user: Object using the feat. (required)</li>
--   <li>stop: True if stopped performing, false if started.</li></ul>
-- @return True if performed successfully.
Feat.perform = function(self, args)
	local anim = Featanimspec:find{name = self.animation}
	local slot = anim and anim.slot
	local weapon = slot and args.user:get_item{slot = slot}
	local info = anim and self:get_info{attacker = args.user, weapon = weapon}
	-- Check for cooldown and requirements.
	if info and not args.stop then
		if args.user.cooldown then return end
		if not self:usable(args) then return end
		for k,v in pairs(info.required_reagents) do
			args.user:subtract_items{name = k, count = v}
		end
		local w = info.required_skills["willpower"]
		if w then args.user.skills:subtract{skill = "willpower", value = w} end
		if info.cooldown > 0 then
			args.user.cooldown = info.cooldown
		end
	end
	-- Calculate the charge time.
	local charge
	if args.user.attack_charge then
		charge = Program.time - args.user.attack_charge
	end
	-- Animate the feat.
	Vision:event{type = "object-feat", object = args.user, anim = anim}
	local play_effects = function(self)
		if anim.effect then
			Effect:play{effect = anim.effect, object = args.user}
		end
		if weapon and weapon.spec.effect_attack then
			Effect:play{effect = weapon.spec.effect_attack, object = args.user}
		end
	end
	-- Call the feat function.
	if not info or anim.toggle or not args.stop then
		if anim then
			if anim.categories["build"] then
				-- Build terrain or machines.
				-- While the attack animation is played, an attack ray is cast.
				-- If a tile collides with the ray, a new tile is attached to it.
				Thread(function(t)
					play_effects()
					Thread:sleep(args.user.spec.timing_build * 0.02)
					local src,dst = args.user:get_attack_ray(args)
					local r = Physics:cast_ray{src = src, dst = dst}
					if not r or not r.tile then return end
					self:apply{
						attacker = args.user,
						charge = charge,
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
				Thread(function(t)
					play_effects()
					Thread:sleep(args.user.spec.timing_attack_blunt * 0.02)
					local src,dst = args.user:get_attack_ray(args)
					local r = Physics:cast_ray{src = src, dst = dst}
					if not r then return end
					self:apply{
						attacker = args.user,
						charge = charge,
						point = r.point,
						target = r.object,
						tile = r.tile,
						weapon = weapon}
				end)
			end
			if anim.categories["explode"] then
				-- Self-destruction.
				-- The creature explodes after the animation has played.
				Thread(function(t)
					play_effects()
					Thread:sleep(args.user.spec.timing_attack_explode * 0.02)
					args.user:die()
					Utils:explosion(args.user.position)
				end)
			end
			if anim.categories["ranged"] then
				-- Ranged attack.
				-- A projectile is fired at the specific time into the attack
				-- animation. The collision callback of the projectile takes
				-- care of damaging the hit object or tile.
				Thread(function(t)
					if anim.categories["spell"] then
						Thread:sleep(args.user.spec.timing_spell_ranged * 0.02)
					elseif weapon and weapon.spec.animation_attack == "attack bow" then
						Thread:sleep(args.user.spec.timing_attack_bow * 0.02)
					elseif weapon and weapon.spec.animation_attack == "attack crossbow" then
						Thread:sleep(args.user.spec.timing_attack_crossbow * 0.02)
					elseif weapon and weapon.spec.animation_attack == "attack musket" then
						Thread:sleep(args.user.spec.timing_attack_musket * 0.02)
					elseif weapon and weapon.spec.animation_attack == "attack revolver" then
						Thread:sleep(args.user.spec.timing_attack_revolver * 0.02)
					end
					play_effects()
					for name,count in pairs(info.required_ammo) do
						local ammo = args.user:split_items{name = name, count = count}
						if ammo then
							ammo:fire{charge = charge, collision = true, feat = self, owner = args.user, speedline = true, weapon = weapon}
							return
						end
					end
					for index,data in ipairs(self.effects) do
						local effect = Feateffectspec:find{name = data[1]}
						if effect and effect.projectile then
							local ammo = Object{model = effect.projectile, physics = "rigid"}
							ammo.gravity = Vector()
							if effect.name == "dig" then
								ammo:fire{charge = charge, collision = true, feat = self, owner = args.user, timer = 10}
								ammo.orig_rotation = ammo.rotation
								ammo.orig_velocity = ammo.velocity
								ammo.effect = "dig"
								ammo.power = 1 + 0.1 * data[2]
							else
								ammo:fire{charge = charge, collision = true, feat = self, owner = args.user, weapon = weapon}
							end
							return
						end
					end
				end)
			end
			if anim.categories["self"] then
				-- Target self.
				-- At the specific time into the attack animation, the effects of the
				-- feat are applied to the attacker herself.
				Thread(function(t)
					play_effects()
					Thread:sleep(args.user.spec.timing_spell_self * 0.02)
					self:apply{
						attacker = args.user,
						charge = charge,
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
				local charge = 1 + 2 * math.min(1, (charge or 0) / 2)
				Thread(function(t)
					play_effects()
					Thread:sleep(args.user.spec.timing_attack_throw * 0.02)
					local proj = weapon:fire{
						charge = charge,
						collision = not weapon.spec.destroy_timer,
						feat = self,
						owner = args.user,
						speed = 10 * charge,
						timer = weapon.spec.destroy_timer}
				end)
			end
			if anim.categories["touch"] then
				-- Touch spell.
				-- While the attack animation is played, an attack ray is cast.
				-- The first object or tile that collides with the ray is damaged.
				Thread(function(t)
					play_effects()
					Thread:sleep(args.user.spec.timing_spell_touch * 0.02)
					local src,dst = args.user:get_attack_ray(args)
					local r = Physics:cast_ray{src = src, dst = dst}
					if not r then return end
					self:apply{
						attacker = args.user,
						charge = charge,
						point = r.point,
						target = r.object,
						tile = r.tile,
						weapon = weapon}
				end)
			end
		end
		if self.func then self:func(args) end
	end
	return true
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
	local anim = Featanimspec:find{name = self.animation}
	if not anim then return end
	local weapon = args.user:get_item{slot = anim.slot}
	local info = self:get_info{attacker = args.user, weapon = weapon}
	-- Check for skills.
	local skills = args.skills or args.user.skills
	for k,v in pairs(info.required_skills) do
		if not skills then return end
		local val = skills:get_value{skill = k}
		if not val or val < v then return end
	end
	-- Check for reagents.
	local inventory = args.inventory or args.user.inventory
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

