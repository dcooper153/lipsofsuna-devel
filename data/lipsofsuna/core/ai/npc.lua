require(Mod.path .. "ai")

NpcAi = Class(Ai)
Ai.dict_name["npc"] = NpcAi

--- Creates a new creature AI.<br/>
-- The AI is inactive when created. It's only activated when the controlled
-- creature enters the vision radius of the player. This allows us to save lots
-- of computing time since player motion often triggers loading of sectors whose
-- creatures are never seen.
-- @param clss AI class.
-- @param object Controlled creature.
-- @return AI.
NpcAi.new = function(clss, object)
	local self = Ai.new(clss, object)
	self.update_timer = 0
	self:set_state{state = "none"}
	return self
end

--- Chooses the combat action of the creature.
-- @param self AI.
NpcAi.choose_combat_action = function(self)
	local allow_forward
	local allow_forward_jump
	local allow_backward
	local allow_backward_jump
	local allow_strafe_left
	local allow_strafe_left_jump
	local allow_strafe_right
	local allow_strafe_right_jump
	-- Calculate the distance to the target.
	local spec = self.object.spec
	local diff = self.target.position - self.object.position
	local size1 = self.object.bounding_box_physics.size
	local size2 = self.target.bounding_box_physics.size
	size1.y = 0
	size2.y = 0
	local dist = diff.length - 0.5 * (size1 + size2).length
	local hint = 0.7 * spec.aim_ray_end
	-- Calculate the tile offset.
	local ctr = self.object.position * Voxel.tile_scale + Vector(0,0.5,0)
	local dir = self.target.position - self.object.position
	dir = Vector(dir.x,0,dir.z):normalize()
	-- Check if we could walk forward.
	if spec.ai_enable_walk then
		local dst = (ctr + dir):floor()
		local f1 = Voxel:get_tile(dst)
		local f2 = Voxel:get_tile(dst + Vector(0,1,0))
		if f1 == 0 and f2 == 0 then
			allow_forward = true
		elseif f2 == 0 and spec.ai_enable_jump then
			local f3 = Voxel:get_tile(dst + Vector(0,2,0))
			if f3 == 0 then
				allow_forward_jump = true
			end
		end
	end
	-- Check if we could walk backward.
	if spec.ai_enable_backstep and math.random() > spec.ai_offense_factor then
		local dstb = (ctr - dir):floor()
		local b1 = Voxel:get_tile(dstb)
		local b2 = Voxel:get_tile(dstb + Vector(0,1,0))
		if b1 == 0 and b2 == 0 then
			allow_backward = true
		elseif b2 == 0 and spec.ai_enable_jump then
			local b3 = Voxel:get_tile(dstb + Vector(0,2,0))
			if b3 == 0 then
				allow_backward_jump = true
			end
		end
	end
	-- Check if we could strafe left.
	if spec.ai_enable_strafe and math.random() > spec.ai_offense_factor then
		local dirl = Quaternion{axis = Vector(0,1), angle = 0.5 * math.pi} * dir
		local dstl = (ctr + dirl):floor()
		local l1 = Voxel:get_tile(dstl)
		local l2 = Voxel:get_tile(dstl + Vector(0,1))
		if l2 == 0 then
			if l1 == 0 then
				allow_strafe_left = true
			elseif spec.ai_enable_jump then
				local l3 = Voxel:get_tile(dstl + Vector(0,2))
				allow_strafe_left_jump = (l3 == 0)
			end
		end
	end
	-- Check if we could strafe right.
	if spec.ai_enable_strafe and math.random() > spec.ai_offense_factor then
		local dirr = Quaternion{axis = Vector(0,1), angle = -0.5 * math.pi} * dir
		local dstr = (ctr + dirr):floor()
		local r1 = Voxel:get_tile(dstr)
		local r2 = Voxel:get_tile(dstr + Vector(0,1))
		if r2 == 0 then
			if r1 == 0 then
				allow_strafe_right = true
			elseif spec.ai_enable_jump then
				local r3 = Voxel:get_tile(dstr + Vector(0,2))
				allow_strafe_right_jump = (r3 == 0)
			end
		end
	end
	-- Prepare attack calculation data.
	-- For any type of attack to be possible, the AI must have attacks enabled
	-- and the cooldown should be over. The weapon and the aim correctness are
	-- also used by all the checks so they're determined here.
	local feat
	local attack = spec.ai_enable_attack and not self.cooldown and (allow_forward or allow_forward_jump or not spec.ai_enable_walk)
	local weapon = self.object:get_weapon()
	local look = self.object.rotation * Vector(0,0,-1)
	local aim = dir:dot(Vector(look.x, 0, look.z):normalize())
	-- Calculate the melee attack probability.
	-- The creature must be bare-handed or wield a melee weapon.
	-- Offensive, magnitude is 0 or 4.
	local p_melee = 0
	if attack and spec.can_melee and (not weapon or weapon.spec.categories["melee"]) and aim > 0.8 then
		feat = self:find_best_feat{category = "melee", target = self.target, weapon = weapon}
		if feat then p_melee = 4 end
	end
	-- Calculate ranged attack probability.
	-- The creature must wield a ranged weapon.
	-- Offensive, magnitude is 0 or 4.
	local p_ranged = 0
	if attack and spec.can_ranged and weapon and weapon.spec.categories["ranged"] and aim > 0.8 then
		feat = self:find_best_feat{category = "ranged", target = self.target, weapon = weapon}
		if feat then p_ranged = 4 end
	end
	-- Calculate ranged spell probability.
	-- The creature must be able to cast a useful ranged spell.
	-- Offensive, magnitude is 0, 2 or 4.
	local feat_spell_ranged
	local p_spell_ranged = 0
	if spec.can_cast_ranged and aim > 0.8 then
		feat_spell_ranged = self:find_best_feat{category = "ranged spell", target = self.target}
		if feat_spell_ranged then
			p_spell_ranged = (dist > hint) and 4 or 2
			for k,v in pairs(feat_spell_ranged.effects) do
				v[2] = v[2] * spec.ai_offense_factor
			end
		end
	end
	-- Calculate cast on self spell probability.
	-- The creature must be able to cast a useful spell on self.
	-- Offensive, magnitude is 0 or 4.
	local feat_spell_self
	local p_spell_self = 0
	if spec.can_cast_self and dist > 2 * hint then
		feat_spell_self = self:find_best_feat{category = "spell on self", target = self.object}
		if feat_spell_self then
			p_spell_self = 4
			for k,v in pairs(feat_spell_self.effects) do
				v[2] = v[2] * (1 - spec.ai_offense_factor)
			end
		end
	end
	-- Calculate touch spell probability.
	-- The creature must be able to cast a useful touch spell.
	-- Offensive, magnitude is 0 or 4.
	local feat_spell_touch
	local p_spell_touch = 0
	if spec.can_cast_touch and dist < hint and aim > 0.8 then
		feat_spell_touch = self:find_best_feat{category = "spell on touch", target = self.target}
		if feat_spell_touch then
			p_spell_touch = 4
			for k,v in pairs(feat_spell_touch.effects) do
				v[2] = v[2] * spec.ai_offense_factor
			end
		end
	end
	-- Calculate throw attack probability.
	-- The creature must wield a throwable weapon.
	-- Offensive, magnitude is 0 or 4.
	local p_throw = 0
	if attack and spec.can_throw and weapon and weapon.spec.categories["throwable"] and aim > 0.8 then
		feat = self:find_best_feat{category = "throw", target = self.target, weapon = weapon}
		if feat then
			p_ranged = 4
			for k,v in pairs(feat.effects) do
				feat.effects[k] = v * spec.ai_offense_factor
			end
		end
	end
	-- Calculate the forward walking probability.
	-- Walking forward if preferred if the creature is far away from the target.
	-- Offensive, magnitude is 0 or 1.
	local p_forward = 0
	if (allow_forward or allow_forward_jump) and dist > hint then
		p_forward = 1
	end
	-- Calculate the weapon switch probability.
	-- The weapon is switched if it's much worse than the best possible weapon.
	-- Offensive, magnitude is 0 or 3.
	local p_weapon = 0
	if spec.ai_enable_weapon_switch then
		local curr,melee,ranged,throw = 1,1,0,0
		if weapon then curr,melee,ranged,throw = self:calculate_weapon_ratings(weapon) end
		local best = math.max(self.melee_rating, self.ranged_rating, self.throw_rating)
		if (curr + 5 < best) or (curr == 0 and best > 0) then
			p_weapon = 3
		end
	end
	-- Calculate the backstep probability.
	-- Backstepping is preferred if the creature is too close to the target.
	-- Defensive, magnitude is 0, 1 or 4.
	local p_backward = 0
	if (allow_backward or allow_backward_jump) and dist < 2 * hint then
		if dist < 0.3 * hint then
			p_backward = 4
		else
			p_backward = 1
		end
	end
	-- Calculate the blocking probability.
	-- Defensive, magnitude is 0 or 1.
	local p_block = 0
	if spec.ai_enable_block and dist < hint then
		p_block = 1
	end
	-- Calculate the strafe probability.
	-- The defense factor determines the probability.
	-- Defensive, magnitude is 0 or 1.
	local p_strafe = 0
	if allow_strafe_left or allow_strafe_left_jump or allow_strafe_right or allow_strafe_right_jump then
		p_strafe = 1
	end
	-- Normalize the probabilities and scale them so that we get the right offense probability.
	local p_offense = p_melee + p_ranged + p_throw + p_forward + p_weapon + p_spell_ranged + p_spell_touch
	local p_defense = p_backward + p_strafe + p_block + p_spell_self
	if p_offense > 0 then
		p_melee = p_melee / p_offense * spec.ai_offense_factor
		p_forward = p_forward / p_offense * spec.ai_offense_factor
		p_weapon = p_weapon / p_offense * spec.ai_offense_factor
		p_spell_ranged = p_spell_ranged / p_offense * spec.ai_offense_factor
		p_spell_touch = p_spell_touch / p_offense * spec.ai_offense_factor
	end
	if p_defense > 0 then
		p_backward = p_backward / p_defense * (1 - spec.ai_offense_factor)
		p_block = p_block / p_defense * (1 - spec.ai_offense_factor)
		p_strafe = p_strafe / p_defense * (1 - spec.ai_offense_factor)
		p_spell_self = p_spell_self / p_offense * spec.ai_offense_factor
	end
	-- Select the action based on the calculated probabilities.
	local choice = math.random()
	if choice < p_melee then
		-- Melee.
		if diff.y > 1 and spec.allow_jump then self.object:jump() end
		self.object:set_block(false)
		if spec.ai_enable_backstep and dist < 0.3 * hint then
			self.object:set_movement(-1)
		elseif spec.ai_enable_walk and dist > 0.6 * hint then
			self.object:set_movement(1)
		else
			self.object:set_movement(0)
		end
		self.object.tilt = self:calculate_melee_tilt()
		self.object:set_strafing(0)
		feat:perform{user = self.object}
		self.action_timer = 0.5
	elseif choice < p_melee + p_ranged then
		-- Ranged.
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = self:calculate_ranged_tilt()
		feat:perform{user = self.object}
		self.action_timer = 1
	elseif choice < p_melee + p_ranged + p_throw then
		-- Throw.
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = self:calculate_ranged_tilt()
		feat:perform{user = self.object}
		self.action_timer = 1
	elseif choice < p_melee + p_ranged + p_throw + p_forward then
		-- Move forward.
		if allow_forward_jump then self.object:jump() end
		self.object:set_block(false)
		self.object:set_movement(1)
		self.object:set_strafing(0)
		self.action_timer = math.random(1, 3)
	elseif choice < p_melee + p_ranged + p_throw + p_forward + p_weapon then
		-- Weapon switch.
		local slot = self.object.spec.weapon_slot
		if self.melee_rating > self.ranged_rating and self.melee_rating > self.throw_rating then
			if self.best_melee_weapon then
				self.object.inventory:equip_object(self.best_melee_weapon, slot)
			else
				self.object.inventory:unequip_slot(slot)
			end
		elseif self.ranged_rating > self.throw_rating then
			self.object.inventory:equip_object(self.best_ranged_weapon, slot)
		else
			self.object.inventory:equip_object(self.best_throw_weapon, slot)
		end
		self.action_timer = 1
	elseif choice < p_melee + p_ranged + p_throw + p_forward + p_weapon + p_backward then
		-- Move backward.
		self.object:set_block(false)
		self.object:set_movement(-0.5)
		self.object:set_strafing(0)
		self.action_timer = math.random(2, 4)
	elseif choice < p_melee + p_ranged + p_throw + p_forward + p_weapon + p_backward + p_block then
		-- Block.
		self.object:set_block(true)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = self:calculate_melee_tilt()
		self.action_timer = math.random(4, 8)
	elseif choice < p_melee + p_ranged + p_throw + p_forward + p_weapon + p_backward + p_block + p_strafe then
		-- Strafe.
		local strafe_left = function()
			if allow_strafe_left or allow_strafe_left_jump then
				if allow_strafe_left_jump then self.object:jump() end
				self.object:set_block(false)
				self.object:set_strafing(-1)
				self.object:set_movement(0)
				self.action_timer = math.random(4, 8)
				return true
			end
		end
		local strafe_right = function()
			if allow_strafe_right or allow_strafe_right_jump then
				if allow_strafe_right_jump then self.object:jump() end
				self.object:set_block(false)
				self.object:set_strafing(1)
				self.object:set_movement(0)
				self.action_timer = math.random(4, 8)
				return true
			end
		end
		if math.random() > 0.5 then
			local r = strafe_left() or strafe_right()
		else
			local r = strafe_right() or strafe_left()
		end
	elseif choice < p_melee + p_ranged + p_throw + p_forward + p_weapon + p_backward + p_block + p_strafe + p_spell_ranged then
		-- Cast ranged.
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = self:calculate_ranged_tilt()
		feat_spell_ranged:perform{user = self.object}
		self.action_timer = 1
	elseif choice < p_melee + p_ranged + p_throw + p_forward + p_weapon + p_backward + p_block + p_strafe + p_spell_ranged + p_spell_self then
		-- Cast self.
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = Quaternion()
		feat_spell_self:perform{user = self.object}
		self.action_timer = 1
	elseif choice < p_melee + p_ranged + p_throw + p_forward + p_weapon + p_backward + p_block + p_strafe + p_spell_ranged + p_spell_self + p_spell_touch then
		-- Cast touch.
		self.object:set_block(false)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.object.tilt = self:calculate_melee_tilt()
		feat_spell_touch:perform{user = self.object}
		self.action_timer = 1
	else
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.action_timer = math.random(1, 5)
	end
end

--- Analyzes the surrounding terrain and chooses a wander target.<br/>
-- TODO: Having goals or a schedule would be nice.
-- @param self AI.
NpcAi.choose_wander_target = function(self)
	-- Randomize the search order.
	local src = (self.object.position * Voxel.tile_scale + Vector(0,0.5,0)):floor()
	local dirs = {Vector(1,0,0), Vector(-1,0,0), Vector(0,0,1), Vector(0,0,-1)}
	for a=1,4 do
		local b = math.random(1,4)
		dirs[a],dirs[b] = dirs[b],dirs[a]
	end
	-- Try to find an open path.
	for k,v in ipairs(dirs) do
		local dst = src + v * Voxel.tile_size
		if self:avoid_wander_obstacles(dst) then
			self.target = (src + v * 10 + Vector(0.5,0.5,0.5)) * Voxel.tile_size
			return
		end
	end
	-- Fallback to a random direction.
	local rot = Quaternion{axis = Vector(0,1,0), angle = math.random() * 6.28}
	self.target = self.object.position + rot * Vector(0, 0, 10)
end

--- Tries to avoid obstacles on the path to the given wander target.
-- @param self AI.
-- @param target Point vector in world space.
-- @return True if avoided successfully.
NpcAi.avoid_wander_obstacles = function(self, target)
	local src = (self.object.position * Voxel.tile_scale + Vector(0,0.5,0)):floor()
	local dst = (target * Voxel.tile_scale + Vector(0,0.5,0)):floor()
	local p = Vector(
		math.min(math.max(dst.x, src.x - 1), src.x + 1), src.y,
		math.min(math.max(dst.z, src.z - 1), src.z + 1), src.y)
	local t1 = Voxel:get_tile(p)
	p.y = p.y + 1
	local t2 = Voxel:get_tile(p)
	if t1 == 0 and t2 == 0 then
		return true
	end
	p.y = p.y + 1
	local t3 = Voxel:get_tile(p)
	if t2 == 0 and t3 == 0 then
		self.object:jump()
		return true
	end
end

--- Used to wake up the AI when a player is nearby.
-- @param self AI.
NpcAi.refresh = function(self)
	if self.state == "none" then self:set_state{state = "wander"} end
end

--- Sets the AI state of the AI.
-- @param self AI.
-- @param args Arguments.<ul>
--   <li>state: AI state.</li>
--   <li>target: Targeted object.</li></ul>
NpcAi.set_state = function(self, args)
	local s = args.state
	if (s == "wander" and not self.object.spec.ai_enable_wander) or
	   (s == "combat" and not self.object.spec.ai_enable_combat) then
		s = "wait"
	end
	self.state = s
	self.target = args.target
	self.state_timer = 0
	self.ai_timer = math.random()
	self.action_timer = 0
	self.action_state = nil
	self.object:set_block(false)
	self.object.running = (self.state == "combat")
	self.object:calculate_speed()
end

--- Updates the AI.
-- @param self AI.
-- @param secs Seconds since the last update.
NpcAi.update = function(self, secs)
	-- Early exit for inactive AI.
	-- There are often lots of creatures in the active map area but most of them
	-- have their AI disabled due no player being nearby. Since this function
	-- would otherwise do at least a bit of work for each tick, it pays to exit early.
	if self.state == "none" then return end
	-- Avoid excessive updates.
	-- Combat needs a high update rate so that we can react to the movements of
	-- the enemy. The peaceful mode needs to be able to avoid getting stuck.
	self.update_timer = self.update_timer + secs
	if self.state == "combat" then
		if self.update_timer < 0.03 then return end
	else
		if self.update_timer < 0.07 then return end
	end
	local tick = self.update_timer
	self.update_timer = 0
	-- Maintain timers.
	self.state_timer = self.state_timer + tick
	self.ai_timer = self.ai_timer + tick
	self.action_timer = self.action_timer - tick
	-- Let the current state manipulate the position and other attributes
	-- of the character and trigger state dependent actions such as attacking.
	local func = self.state_updaters[self.state]
	func(self, tick)
	-- Only consider state changes every couple of seconds.
	if self.ai_timer < self.object.spec.ai_update_delay then return end
	self:update_state()
end

--- Updates the state of the AI.
-- @param self AI.
NpcAi.update_state = function(self)
	self.ai_timer = 0
	-- TODO: Flee if about to die.
	-- Find the best enemy to attack.
	self:scan_enemies()
	local best_enemy = nil
	local best_rating = -1
	if self.object.spec.ai_enable_combat then
		for k,v in pairs(self.enemies) do
			local r = self:calculate_enemy_rating(v[1])
			if r > best_rating then
				best_enemy = v[1]
				best_rating = r
			end
		end
	end
	-- Enter combat mode if an enemy was found.
	if best_enemy then
		self:calculate_combat_ratings()
		self:set_state{state = "combat", target = best_enemy}
		return
	end
	-- Switch to chat mode if a dialog is active.
	if self.dialog then
		self:set_state{state = "chat"}
		return
	end
	-- Handle any other state changes.
	local func = self.state_switchers[self.state]
	func(self)
end

------------------------------------------------------------------------------

-- This table contains the functions called each tick to update the position
-- of the creature and to initiate state specific actions such as attacking.
-- No state changes can occur during these updates.
NpcAi.state_updaters =
{
	chat = function(self, secs)
		-- Turn towards the target.
		if self.object.dialog then
			self.object:set_movement(0)
			if self.object.dialog.user then
				self.object:face_point{point = self.object.dialog.user.position, secs = secs}
			end
		end
	end,
	combat = function(self, secs)
		-- Decide what combat action to perform next.
		if self.action_timer <= 0 then
			self:choose_combat_action()
		end
		-- Turn towards the target.
		local face = self.object:face_point{point = self.target.position, secs = secs}
		if face < 0.5 then
			self.object:set_movement(0)
			return
		end
	end,
	follow = function(self, secs)
		-- TODO
	end,
	flee = function(self, secs)
		-- TODO
	end,
	hide = function(self, secs)
		-- TODO
	end,
	none = function(self, secs)
	end,
	search = function(self, secs)
		-- TODO
	end,
	wait = function(self, secs)
		-- TODO
	end,
	wander = function(self, secs)
		if not self.target then return end
		-- Turn towards the target.
		local face = self.object:face_point{point = self.target, secs = secs}
		if face < 0.5 then
			self.object:set_movement(0)
			return
		end
		-- Move towards the target.
		local dist = (self.target - self.object.position).length
		if dist < 0.5 then
			self.object:set_movement(0)
		else
			self.object:set_movement(0.8)
		end
	end
}

-- This table contains the functions called every couple of seconds to switch
-- the state of the creature. Switching to the combat mode is handled separately
-- so it isn't present here but all other state changes are.
NpcAi.state_switchers =
{
	chat = function(self)
		if not self.object.dialog then
			self:set_state{state = "wander"}
		end
	end,
	combat = function(self)
		-- If we were in the combat state, enter the searching mode in hopes
		-- of finding any enemies that are hiding or trying to escape.
		-- TODO: Companion should follow master.
		self:set_state{state = "search"}
		self.object:set_movement(0)
		self.object:set_strafing(0)
		return
	end,
	follow = function(self)
		-- TODO
	end,
	flee = function(self)
		-- If we were fleeing, keep fleeing a bit more and then try to hide
		-- so that the enemies have harder time to find us.
		if self.state_timer > 5 then
			self:set_state{state = "hide"}
		end
	end,
	hide = function()
		-- If we were hiding, keep hiding for a good while and hope that the
		-- enemies got away. Then begin wandering.
		if self.state_timer > 30 then
			self:set_state{state = "wander"}
		end
	end,
	none = function(self)
	end,
	search = function(self)
		-- If we have been searching for a while without finding anything,
		-- conclude that there are no enemies and enter the wandering mode.
		--if self.state_timer > self.object.spec.ai_search_time then
			self:set_state{state = "wander"}
		--end
	end,
	wait = function(self)
		-- Start wandering after resting a while.
		if self.state_timer > 10 then
			self:set_state{state = "wander"}
		end
	end,
	wander = function(self)
		-- Select target point.
		self:choose_wander_target()
		-- Switch to waiting mode after wandering enough.
		if self.object.spec.ai_wait_allowed then
			if self.state_timer > self.object.spec.ai_wander_time then
				self.object:set_movement(0)
				self:set_state{state = "wait"}
			end
		end
	end
}
