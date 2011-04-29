Ai = Class()

--- Creates a new creature AI.
-- @param clss AI class.
-- @param object Controlled creature.
-- @return AI.
Ai.new = function(clss, object)
	local self = Class.new(clss)
	self.object = object
	self.enemies = setmetatable({}, {__mode = "kv"})
	self.update_timer = 0
	self:calculate_combat_ratings()
	self:set_state{state = "wander"}
	self.object:set_movement(0)
	return self
end

--- Chooses the combat action of the creature.
-- @param self AI.
Ai.choose_combat_action = function(self)
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
	local dist = diff.length
	local hint = spec.ai_distance_hint + self.target.spec.ai_distance_hint
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
	-- Calculate the melee attack probability.
	-- The creature needs to be able to attack and the cooldown should be over.
	-- It also needs to be close enough to the target, face towards it and
	-- know at least one melee attack feat.
	-- Offensive, magnitude is 0 or 4.
	local feat
	local p_attack = 0
	if spec.ai_enable_attack and not self.cooldown and dist < hint * 3 and (allow_forward or allow_forward_jump or not spec.ai_enable_walk) then
		local look = self.object.rotation * Vector(0,0,-1)
		look = Vector(look.x, 0, look.z):normalize()
		if dir:dot(look) > 0.8 then
			local w = self.object:get_item{slot = "hand.R"}
			feat = self:find_best_feat{category = "melee", target = self.target, weapon = w}
			if feat then p_attack = 4 end
		end
	end
	-- Calculate the forward walking probability.
	-- Walking forward if preferred if the creature is far away from the target.
	-- Offensive, magnitude is 0 or 1.
	local p_forward = 0
	if (allow_forward or allow_forward_jump) and dist > hint * 3 then
		p_forward = 1
	end
	-- Calculate the backstep probability.
	-- Backstepping is preferred if the creature is too close to the target.
	-- Defensive, magnitude is 0, 1 or 4.
	local p_backward = 0
	if (allow_backward or allow_backward_jump) and dist < 6 * hint then
		if dist < hint then
			p_backward = 4
		else
			p_backward = 1
		end
	end
	-- Calculate the blocking probability.
	-- Defensive, magnitude is 0 or 1.
	local p_block = 0
	if spec.ai_enable_block and dist < 3 * hint then
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
	local p_offense = p_attack + p_forward
	local p_defense = p_backward + p_strafe + p_block
	if p_offense > 0 then
		p_attack = p_attack / p_offense * spec.ai_offense_factor
		p_forward = p_forward / p_offense * spec.ai_offense_factor
	end
	if p_defense > 0 then
		p_backward = p_backward / p_defense * (1 - spec.ai_offense_factor)
		p_block = p_block / p_defense * (1 - spec.ai_offense_factor)
		p_strafe = p_strafe / p_defense * (1 - spec.ai_offense_factor)
	end
	-- Select the action based on the calculated probabilities.
	local choice = math.random()
	if choice < p_attack then
		if diff.y > 1 and spec.allow_jump then self.object:jump() end
		self.object:set_block(false)
		if dist < hint then
			self.object:set_movement(-1)
		elseif dist > 2 * hint then
			self.object:set_movement(1)
		else
			self.object:set_movement(0)
		end
		self.object:set_strafing(0)
		feat:perform{user = self.object}
		self.action_timer = 0.5
	elseif choice < p_attack + p_forward then
		if allow_forward_jump then self.object:jump() end
		self.object:set_block(false)
		self.object:set_movement(1)
		self.object:set_strafing(0)
		self.action_timer = math.random(1, 3)
	elseif choice < p_attack + p_forward + p_backward then
		self.object:set_block(false)
		self.object:set_movement(-0.5)
		self.object:set_strafing(0)
		self.action_timer = math.random(2, 4)
	elseif choice < p_attack + p_forward + p_backward + p_block then
		self.object:set_block(true)
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.action_timer = math.random(4, 8)
	elseif choice < p_attack + p_forward + p_backward + p_block + p_strafe then
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
	else
		self.object:set_movement(0)
		self.object:set_strafing(0)
		self.action_timer = math.random(1, 5)
	end
end

--- Analyzes the surrounding terrain and chooses a wander target.<br/>
-- TODO: Having goals or a schedule would be nice.
-- @param self AI.
Ai.choose_wander_target = function(self)
	-- Randomize the search order.
	local src = (self.object.position * Voxel.tile_scale + Vector(0,0.5,0)):floor()
	local dirs = {Vector(1,0,0), Vector(-1,0,0), Vector(0,0,1), Vector(0,0,-1)}
	for a=1,4 do
		local b = math.random(1,4)
		dirs[a],dirs[b] = dirs[b],dirs[a]
	end
	-- Try to find an open path.
	for k,v in ipairs(dirs) do
		local dst = src + v
		local t1 = Voxel:get_tile(dst)
		local t2 = Voxel:get_tile(dst + Vector(0,1,0))
		if t1 == 0 and t2 == 0 then
			self.target = (src + v * 10 + Vector(0.5,0.5,0.5)) * Voxel.tile_size
			return
		end
		local t3 = Voxel:get_tile(dst + Vector(0,2,0))
		if t2 == 0 and t3 == 0 then
			self.target = (src + v * 10 + Vector(0.5,0.5,0.5)) * Voxel.tile_size
			self.object:jump()
			return
		end
	end
	-- Fallback to a random direction.
	local rot = Quaternion{axis = Vector(0,1,0), angle = math.random() * 6.28}
	self.target = self.object.position + rot * Vector(0, 0, 10)
end

--- Updates the combat ratings of the creature.
-- @param self AI.
Ai.calculate_combat_ratings = function(self)
	-- TODO
	self.melee_rating = 1
	self.ranged_rating = 0
end

--- Calculates how desirable it is to attack the given enemy.
-- @param self Object.
-- @param enemy Enemy object.
Ai.calculate_enemy_rating = function(self, enemy)
	-- TODO: Should take enemy weakness into account.
	-- TODO: Should probably take terrain into account by solving paths.
	return 1 / ((self.object.position - enemy.position).length + 1)
end

--- Finds the best feat to use in combat.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>category: Feat category.</li>
--   <li>target: Object to be attacked.</li>
--   <li>weapon: Weapon to be used.</li></ul>
-- @return New feat.
Ai.find_best_feat = function(self, args)
	local best_feat = nil
	local best_score = -1
	local solve_effect_value = function(feat, effect)
		-- Solves the maximum value the effect can have without the feat
		-- becoming unusable. The solution is found by bisecting.
		local i = #feat.effects + 1
		local e = {effect.name, 0}
		local step = 50
		feat.effects[i] = e
		repeat
			e[2] = e[2] + step
			if not feat:usable{user = self.object} then
				e[2] = e[2] - step
			end
			step = step / 2
		until step < 1
		feat.effects[i] = nil
		return e[2]
	end
	local process_anim = function(anim)
		-- Check if the feat animation is usable.
		local feat = Feat{animation = anim.name}
		if not feat:usable{user = self.object} then return end
		-- Add usable feat effects.
		for name in pairs(self.object.spec.feat_effects) do
			local effect = Feateffectspec:find{name = name}
			if effect then
				local value = solve_effect_value(feat, effect)
				if value >= 1 then feat.effects[#feat.effects + 1] = {name, value} end
			end
		end
		-- Calculate the score.
		local info = feat:get_info{attacker = self.object, target = args.target, weapon = args.weapon}
		local score = -(info.influences.health or 0)
		if not score or score < 0 then return end
		score = score + 100 * math.random()
		-- Maintain the best feat.
		if score <= best_score then return end
		best_feat = feat:copy()
		best_score = score
	end
	-- Score each feat animation and choose the best one.
	for anim_name in pairs(self.object.spec.feat_anims) do
		local anim = Featanimspec:find{name = anim_name}
		if anim and anim.categories[args.category] then process_anim(anim) end
	end
	return best_feat
end

--- Updates the enemy list of the AI.
-- @param self AI.
Ai.scan_enemies = function(self)
	-- Clear old enemies.
	local old = self.enemies
	local time = Program.time
	self.enemies = {}
	-- Find new enemies.
	local objs = Object:find{point = self.object.position, radius = 10}
	for k,v in pairs(objs) do
		local enemy = old[v]
		if enemy and time - enemy[2] < 10 then
			-- If the enemy is still nearby and was last seen a very short time
			-- ago, we add it back to the list. Without this, the creature would
			-- give up the moment the target hides behind anything.
			self.enemies[v] = enemy
		elseif self.object:check_enemy(v) then
			-- If a new enemy was within the scan radius, a line of sight check
			-- is performed to cull enemies behind walls. If the LOS check
			-- succeeds, the enemy is considered found.
			if self.object:check_line_of_sight{object = v} then
				self.enemies[v] = {v, time}
			end
		end
	end
end

--- Sets the AI state of the AI.
-- @param self AI.
-- @param args Arguments.<ul>
--   <li>state: AI state.</li>
--   <li>target: Targeted object.</li></ul>
Ai.set_state = function(self, args)
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
Ai.update = function(self, secs)
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
Ai.update_state = function(self)
	self.ai_timer = 0
	-- Update our combat ratings so that we can correctly estimate our
	-- chances to stand against our enemies.
	self:calculate_combat_ratings()
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
Ai.state_updaters =
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
Ai.state_switchers =
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
