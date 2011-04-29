Ai = Class()

--- Creates a new creature AI.
-- @param clss AI class.
-- @param object Controlled creature.
-- @return AI.
Ai.new = function(clss, object)
	local self = Class.new(clss)
	self.object = object
	self.enemies = setmetatable({}, {__mode = "kv"})
	self:calculate_combat_ratings()
	self:set_state{state = "wander"}
	self.object:set_movement(0)
	return self
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
end

--- Updates the AI.
-- @param self AI.
-- @param secs Seconds since the last update.
Ai.update = function(self, secs)
	-- Maintain timers.
	self.state_timer = self.state_timer + secs
	self.ai_timer = self.ai_timer + secs
	self.action_timer = self.action_timer - secs
	-- Let the current state manipulate the position and other attributes
	-- of the character and trigger state dependent actions such as attacking.
	local func = self.state_updaters[self.state]
	func(self, secs)
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
			local actions = {}
			local spec = self.object.spec
			local dist = (self.target.position - self.object.position).length
			for i=1,5 do
				if spec.ai_enable_attack then table.insert(actions, "attack") end
			end
			if spec.ai_enable_attack and dist < 3 then table.insert(actions, "attack") end
			if spec.ai_enable_defend and dist < 4 then table.insert(actions, "defend") end
			if spec.ai_enable_strafe and dist < 4 then table.insert(actions, "strafe") end
			if spec.ai_enable_walk and dist > 5 then table.insert(actions, "normal") end
			if spec.ai_enable_walk and dist < 6 then table.insert(actions, "walk") end
			local action = self.combat_switchers[actions[math.random(#actions)]]
			action(self)
		end
		-- Turn towards the target.
		local face = self.object:face_point{point = self.target.position, secs = secs}
		if face < 0.5 then
			self.object:set_movement(0)
			return
		end
		-- Perform the current combat action.
		local action = self.combat_updaters[self.action_state]
		action(self)
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
		-- Keep waiting if already waiting.
	end,
	wander = function(self)
		-- Select target point.
		-- TODO: Should check for walls.
		local rot = Quaternion{axis = Vector(0,1,0), angle = math.random() * 6.28}
		self.target = self.object.position + rot * Vector(0, 0, 10)
		-- Switch to waiting mode after wandering enough.
		if self.object.spec.ai_wait_allowed then
			if self.state_timer > self.object.spec.ai_wander_time then
				self:set_state{state = "wait"}
			end
		end
	end
}

-- This table contains the functions called each tick to update the position
-- of the creature when it's in combat.
Ai.combat_updaters =
{
	attack = function(self)
		-- Maintain distance to the target.
		local diff = self.target.position - self.object.position
		local dist = diff.length
		local hint = self.object.spec.ai_distance_hint + self.target.spec.ai_distance_hint
		if diff.y > 0.5 then self.object:jump() end
		if dist < hint then
			self.object:set_movement(-0.5)
		elseif dist > hint + 2 then
			self.object:set_movement(1)
		end
		-- Attack when close enough.
		if not self.cooldown and dist < hint + 1 then
			local f = self:find_best_feat{category = "melee", target = self.target,
				weapon = self.object:get_item{slot = "hand.R"}}
			if f then f:perform{user = self.object} end
		end
	end,
	defend = function(self)
		-- Maintain distance to the target.
		local dist = (self.target.position - self.object.position).length
		local hint = self.object.spec.ai_distance_hint + self.target.spec.ai_distance_hint
		if dist < hint + 1 then
			self.object:set_movement(-0.5)
		elseif dist > hint + 2 then
			self.object:set_movement(0.5)
		end
	end,
	normal = function(self)
		-- Maintain distance to the target.
		local diff = self.target.position - self.object.position
		local dist = diff.length
		local hint = self.object.spec.ai_distance_hint + self.target.spec.ai_distance_hint
		if diff.y > 0.5 then self.object:jump() end
		if dist >= hint + 4 then
			self.object:set_movement(1)
		elseif dist >= hint + 3 then
			self.object:set_movement(0.5)
		elseif dist <= hint + 1 then
			self.object:set_movement(-0.5)
		else
			self.object:set_movement(0)
			self.action_timer = math.max(self.action_timer, 1)
		end
	end,
	strafe = function(self)
	end,
	walk = function(self)
		-- Move toward the target until close.
		local dist = (self.target.position - self.object.position).length
		local hint = self.object.spec.ai_distance_hint + self.target.spec.ai_distance_hint
		if dist < hint and self.object.movement > 0 then
			self.action_timer = 0
		end
	end
}

-- This table contains the functions called every time the previous combat action
-- of the creature is completed.
Ai.combat_switchers =
{
	attack = function(self)
		self.action_state = "attack"
		self.action_timer = math.random(2, 4)
		self.object:set_strafing(0)
		self.object:set_movement(1)
	end,
	defend = function(self)
		self.action_state = "defend"
		self.action_timer = math.random(2, 3)
		self.object:set_strafing(0)
		self.object:set_movement(0)
	end,
	normal = function(self)
		self.action_state = "normal"
		self.action_timer = math.random(1, 2)
		self.object:set_strafing(0)
	end,
	strafe = function(self)
		local moves = { 0.75, -0.75 }
		self.action_state = "strafe"
		self.action_timer = math.random(2, 5)
		self.object:set_strafing(moves[math.random(2)])
		self.object:set_movement(0)
	end,
	walk = function(self)
		local moves = { 1, -0.5 }
		self.action_state = "walk"
		self.action_timer = math.random(2, 4)
		self.object:set_strafing(0)
		self.object:set_movement(moves[math.random(2)])
	end
}
