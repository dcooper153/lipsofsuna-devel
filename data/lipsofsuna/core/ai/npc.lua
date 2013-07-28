--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.ai.npc
-- @alias NpcAi

local Class = require("system/class")
local Ai = require(Mod.path .. "ai")

--- TODO:doc
-- @type NpcAi
local NpcAi = Class("NpcAi", Ai)
NpcAi.type = "npc"

--- Creates a new actor AI.<br/>
-- The AI is inactive when created. It's only activated when the controlled
-- actor enters the vision radius of the player. This allows us to save lots
-- of computing time since player motion often triggers loading of sectors whose
-- actors are never seen.
-- @param clss AI class.
-- @param object Controlled actor.
-- @return AI.
NpcAi.new = function(clss, object)
	local self = Ai.new(clss, object)
	self.update_timer = 0
	self:set_state{state = "none"}
	-- Initialize states.
	self.enabled_states = {}
	for k,v in pairs(object.spec.ai_enabled_states) do
		self.enabled_states[k] = Aistatespec:find{name = k}
	end
	-- Initialize combat actions.
	self.combat_actions = {}
	for k,v in pairs(object.spec.ai_combat_actions) do
		self.combat_actions[k] = Aiactionspec:find{name = k}
	end
	return self
end

--- Chooses the combat action of the actor.
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
	local diff = self.target:get_position() - self.object:get_position()
	local size1 = self.object.physics:get_bounding_box().size
	local size2 = self.target.physics:get_bounding_box().size
	size1.y = 0
	size2.y = 0
	local dist = diff.length - 0.5 * (size1 + size2).length
	local hint = 0.7 * spec.aim_ray_end
	-- Calculate the tile offset.
	local ctr = self.object:get_position():copy():add_xyz(0,0.5,0)
	local dir = self.target:get_position() - self.object:get_position()
	dir = Vector(dir.x,0,dir.z):normalize()
	-- Check if we could walk forward.
	if spec.ai_enable_walk then
		local dst = (ctr + dir):floor()
		-- FIXME: Stick terrain
		local f1 = 0--Voxel:get_tile(dst)
		local f2 = 0--Voxel:get_tile(dst + Vector(0,1,0))
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
		-- FIXME: Stick terrain
		local b1 = 0--Voxel:get_tile(dstb)
		local b2 = 0--Voxel:get_tile(dstb + Vector(0,1,0))
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
		-- FIXME: Stick terrain
		local l1 = 0--Voxel:get_tile(dstl)
		local l2 = 0--Voxel:get_tile(dstl + Vector(0,1))
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
		-- FIXME: Stick terrain
		local r1 = 0--Voxel:get_tile(dstr)
		local r2 = 0--Voxel:get_tile(dstr + Vector(0,1))
		if r2 == 0 then
			if r1 == 0 then
				allow_strafe_right = true
			elseif spec.ai_enable_jump then
				local r3 = Voxel:get_tile(dstr + Vector(0,2))
				allow_strafe_right_jump = (r3 == 0)
			end
		end
	end
	-- Initialize the action arguments.
	local look = self.object:get_rotation() * Vector(0,0,-1)
	local args = {
		aim = dir:dot(Vector(look.x, 0, look.z):normalize()),
		allow_forward = allow_forward,
		allow_forward_jump = allow_forward_jump,
		allow_backward = allow_backward,
		allow_backward_jump = allow_backward_jump,
		allow_strafe_left = allow_strafe_left,
		allow_strafe_left_jump = allow_strafe_left_jump,
		allow_strafe_right = allow_strafe_right,
		allow_strafe_right_jump = allow_strafe_right_jump,
		attack = spec.ai_enable_attack and not self.cooldown and (allow_forward or allow_forward_jump or not spec.ai_enable_walk),
		dist = dist,
		diff = diff,
		hint = hint,
		look = look,
		spec = spec,
		weapon = self.object:get_weapon()}
	-- Calculate the likelihoods of the actions.
	local actions = {}
	local total = 0
	for k,v in pairs(self.combat_actions) do
		local l = v.calculate(self, args)
		if l and l > 0 then
			if v.categories["defensive"] then
				l = l * (1 - spec.ai_offense_factor)
			elseif v.categories["offensive"] then
				l = l * spec.ai_offense_factor
			else
				l = 0
			end
			if l > 0 then
				actions[k] = l
				total = total + l
			end
		end
	end
	-- Normalize the likelihoods to acquire probabilities.
	if total == 0 then return end
	for k,v in pairs(actions) do
		actions[k] = v / total
	end
	-- Perform a random action based on the probability distribution.
	local accum = 0
	local choice = math.random()
	for k,v in pairs(actions) do
		accum = accum + v
		if choice <= accum then
			local action = self.combat_actions[k]
			action.perform(self, args)
			return true
		end
	end
end

--- Analyzes the surrounding terrain and chooses a wander target.<br/>
-- TODO: Having goals or a schedule would be nice.
-- @param self AI.
NpcAi.choose_wander_target = function(self)
	-- Randomize the search order.
	local src = self.object:get_position():copy()
	local dirs = {Vector(1,0,0), Vector(-1,0,0), Vector(0,0,1), Vector(0,0,-1)}
	for a=1,4 do
		local b = math.random(1,4)
		dirs[a],dirs[b] = dirs[b],dirs[a]
	end
	-- Try to find an open path.
	-- FIXME: Stick terrain.
	--[[
	for k,v in ipairs(dirs) do
		local dst = src + v * Voxel.tile_size
		if self:avoid_wander_obstacles(dst) then
			self.target = (src + v * 10 + Vector(0.5,0.5,0.5)) * Voxel.tile_size
			return
		end
	end]]
	-- Fallback to a random direction.
	local rot = Quaternion{axis = Vector(0,1,0), angle = math.random() * 6.28}
	self.target = self.object:get_position() + rot * Vector(0, 0, 10)
end

--- Tries to avoid obstacles on the path to the given wander target.
-- @param self AI.
-- @param target Point vector in world space.
-- @return True if avoided successfully.
NpcAi.avoid_wander_obstacles = function(self, target)
	do return true end --FIXME
	local src = self.object:get_position():add_xyz(0,0.5,0):floor()
	local dst = (target * Voxel.tile_scale):add_xyz(0,0.5,0):floor()
	local p = Vector(
		math.min(math.max(dst.x, src.x - 1), src.x + 1), src.y,
		math.min(math.max(dst.z, src.z - 1), src.z + 1), src.y)
	-- FIXME: Stick terrain
	local t1 = 0--Voxel:get_tile(p)
	p.y = p.y + 1
	-- FIXME: Stick terrain
	local t2 = 0--Voxel:get_tile(p)
	if t1 == 0 and t2 == 0 then
		return true
	end
	p.y = p.y + 1
	-- FIXME: Stick terrain
	local t3 = 0--Voxel:get_tile(p)
	if t2 == 0 and t3 == 0 then
		self.object:action("jump")
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
	if not self.state_timer or s ~= self.state then
		self.state_timer = 0
	end
	local prev = self.state
	if s == prev then return end
	-- Leave the previous state.
	if prev then
		local state = Aistatespec:find_by_name(prev)
		if state and state.leave then
			state.leave(self)
		end
	end
	-- Reset the controls.
	self.state = s
	self.target = args.target
	self.ai_timer = math.random()
	self.action_timer = 0
	self.action_state = nil
	self.object:set_movement(0)
	self.object:set_strafing(0)
	self.object:set_block(false)
	self.object.running = (self.state == "combat")
	self.object:calculate_speed()
	-- Enter the new state.
	local state = Aistatespec:find_by_name(s)
	if state and state.enter then
		state.enter(self, prev)
	end
end

--- Updates the AI.
-- @param self AI.
-- @param secs Seconds since the last update.
NpcAi.update = function(self, secs)
	-- Disable for dead actors.
	if self.object.dead and self.state ~= "none" then
		self:set_state{state = "none"}
	end
	-- Early exit for inactive AI.
	-- There are often lots of actors in the active map area but most of them
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
	local state = Aistatespec:find{name = self.state}
	if state then state.update(self, tick) end
	-- Only consider state changes every couple of seconds.
	if self.ai_timer < self.object.spec.ai_update_delay then return end
	self:update_state()
end

--- Updates the state of the AI.
-- @param self AI.
NpcAi.update_state = function(self)
	self.ai_timer = 0
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
	self.best_enemy = best_enemy
	-- Choose the next state.
	local best_score
	local best_state
	for k,v in pairs(self.enabled_states) do
		local score = v.calculate(self)
		if score and score > 0 then
			if not best_score or score > best_score then
				best_state = k
				best_score = score
			end
		end
	end
	-- Enter the next state.
	if best_state then
		self:set_state{state = best_state}
	end
end

return NpcAi


