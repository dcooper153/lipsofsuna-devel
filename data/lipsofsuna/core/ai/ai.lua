--- Artificial intelligence.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.ai.ai
-- @alias Ai

local Class = require("system/class")
local Action = require("core/actions/action")
local Physics = require("system/physics")

--- Artificial intelligence.
-- @type Ai
Ai = Class("Ai")
Ai.dict_name = {}

--- Creates a new actor AI.<br/>
--
-- The AI is inactive when created. It's only activated when the controlled
-- actor enters the vision radius of the player. This allows us to save lots
-- of computing time since player motion often triggers loading of sectors whose
-- actors are never seen.
--
-- @param clss AI class.
-- @param object Controlled actor.
-- @return AI.
Ai.new = function(clss, object)
	local self = Class.new(clss)
	self.object = object
	self.enemies = setmetatable({}, {__mode = "kv"})
	self:calculate_combat_ratings()
	self.object:set_movement(0)
	return self
end

--- Registers an AI subclass.
-- @param clss AI class.
-- @param ai Subclass.
Ai.register = function(clss, ai)
	clss.dict_name[ai.type] = ai
end

--- Adds an enemy to the list of enemies.
-- @param self AI.
-- @param object Enemy object.
Ai.add_enemy = function(self, object)
	local enemy = self.enemies[object]
	if enemy then
		enemy[2] = Program:get_time() + 30
	else
		self.enemies[object] = {object, Program:get_time() + 30}
	end
end

--- Updates the combat ratings of the actor.
-- @param self AI.
Ai.calculate_combat_ratings = function(self)
	self.melee_rating = 0
	self.ranged_rating = 0
	self.throw_rating = 0
	self.best_melee_weapon = nil
	self.best_ranged_weapon = nil
	self.best_throw_weapon = nil
	if self.object.spec.can_melee then self.melee_rating = 1 end
	for k,v in pairs(self.object.inventory.stored) do
		local score,melee,ranged,throw = self:calculate_weapon_ratings(v)
		if melee >= self.melee_rating then
			self.melee_rating = melee
			self.best_melee_weapon = v
		end
		if ranged >= self.ranged_rating then
			self.ranged_rating = ranged
			self.best_ranged_weapon = v
		end
		if throw >= self.throw_rating then
			self.throw_rating = throw
			self.best_throw_weapon = v
		end
	end
end

--- Calculates how desirable it is to attack the given enemy.
-- @param self Ai.
-- @param enemy Enemy object.
Ai.calculate_enemy_rating = function(self, enemy)
	-- TODO: Should take enemy weakness into account.
	-- TODO: Should probably take terrain into account by solving paths.
	return 1 / ((self.object:get_position() - enemy:get_position()).length + 1)
end

--- Calculates the tilt value for melee attacks.
-- @param self Ai.
-- @return Tilt quaternion.
Ai.calculate_melee_tilt = function(self)
	-- Calculate distance to the target.
	local diff = self.target:get_position() + self.target.physics:get_center_offset() - self.object:get_position() - self.object.spec.aim_ray_center
	local dist = Vector(diff.x, 0, diff.z).length
	-- Solve the tilt angle analytically.
	local angle = math.atan2(diff.y, dist)
	local limit = self.object.spec.tilt_limit
	angle = math.min(math.max(angle, -limit), limit)
	return Quaternion{euler = {0,0,angle}}
end

--- Calculates the tilt value for ranged spell attacks.
-- @param self Ai.
-- @return Tilt quaternion.
Ai.calculate_ranged_spell_tilt = function(self)
	return self:calculate_melee_tilt()
end

--- Calculates the tilt value for ranged attacks.
-- @param self Ai.
-- @return Tilt quaternion.
Ai.calculate_ranged_tilt = function(self)
	-- Get the ammo type.
	local weapon = self.object:get_weapon()
	if not weapon or not weapon.spec.ammo_type then return Quaternion() end
	local spec = Itemspec:find{name = weapon.spec.ammo_type}
	if not spec then return Quaternion() end
	-- Calculate distance to the target.
	local diff = self.target:get_position() + self.target.physics:get_center_offset() - self.object:get_position() - self.object.spec.aim_ray_center
	local dist = Vector(diff.x, 0, diff.z).length
	-- Solve the tilt angle with brute force.
	local speed = 20
	local solve = function(angle)
		local a = spec.gravity_projectile
		local v = Quaternion{euler = {0,0,angle}} * Vector(0, 0, -speed)
		local t_hit = dist / Vector(v.x,0,v.z).length
		local y_hit = v.y * t_hit + 0.5 * a.y * t_hit^2
		return y_hit
	end
	local best = 0
	local best_error = 1000
	for angle = 0,1.2,0.05 do
		local e = math.abs(solve(angle) - diff.y)
		if e < best_error then
			best = angle
			best_error = e
		end
	end
	return Quaternion{euler = {0,0,best}}
end

-- Calculates the combat ratings of a weapon.
-- @param self Ai.
-- @param weapon Item.
-- @return Best rating, melee rating, ranged rating, throw rating.
Ai.calculate_weapon_ratings = function(self, weapon)
	local spec = self.object.spec
	-- Calculate total damage.
	local score = 0
	local infl = Main.combat_utils:get_item_attack_modifiers(self.object, weapon)
	for k,v in pairs(infl) do
		if k ~= "hatchet" then
			score = score + v
		end
	end
	score = math.max(0, score)
	-- Melee rating.
	local a = 0
	if spec.can_melee and weapon.spec.categories["melee"] then
		a = score
	end
	-- Ranged rating.
	local b = 0
	if spec.can_ranged and weapon.spec.categories["ranged"] then
		local has_ammo
		if weapon.spec.ammo_type then
			has_ammo = self.object.inventory:get_object_by_name(weapon.spec.ammo_type)
		else
			has_ammo = true
		end
		if has_ammo then
			b = score
		end
	end
	-- Throw rating.
	local c = 0
	if spec.can_throw and weapon.spec.categories["throwable"] then
		c = score
	end
	return math.max(a,b,c), a, b, c
end

--- Checks line of sight to the target point or object.
-- @param self AI.
-- @param target Object being looked for.
-- @return True if seen.
Ai.check_line_of_sight = function(self, target)
	local object = self.object
	-- Get the vision ray.
	-- TODO: Take stealth into account.
	-- TODO: Take bounding box into account.
	local src = object:get_position() + Vector(0,1,0)
	local dst = target:get_position() + Vector(0,1,0)
	-- Check for view cone.
	local ray = (src - dst):normalize()
	local look = Vector(0,0,-1):transform(object:get_rotation())
	if math.acos(ray:dot(look)) > object.spec.view_cone then
		return
	end
	-- Check for ray cast success.
	-- TODO: Shoot multiple rays?
	local ret = Physics:cast_ray(src, dst, object)
	return ret and ret.object == target:get_id()
end

--- Finds the best action to use in combat.
-- @param self AI.
-- @param args Arguments.<ul>
--   <li>category: Feat category.</li>
--   <li>target: Object to be attacked.</li>
--   <li>weapon: Weapon to be used.</li></ul>
-- @return Action spec if found. Nil otherwise.
Ai.find_best_action = function(self, args)
	local beneficial = (self.object == args.target)
	local best_action = nil
	local best_score = -1
	local process_action = function(spec)
		-- Check that the category is correct.
		local b = (spec.categories["beneficial"] ~= nil)
		if beneficial ~= b then return end
		-- Check if the action is usable.
		local action = Action(spec, self.object)
		if not action:get_usable() then return end
		-- Make sure that the action can reach the target.
		local range = action:get_range()
		local dist = (self.object:get_position() - args.target:get_position()).length
		if dist > range then return end
		-- Calculate the score.
		return action:get_score()
	end
	-- Score each action and choose the best one.
	local actions = {}
	local total = 0
	local process_spec = function(spec)
		if not spec.actions then return end
		for k,name in pairs(spec.actions) do
			local aspec = Actionspec:find_by_name(name)
			if aspec and aspec.categories[args.category] then
				local score = process_action(aspec)
				if score then
					actions[aspec] = score
					total = total + score
				end
			end
		end
	end
	process_spec(self.object.spec)
	if args.weapon and args.weapon.spec.actions then
		process_spec(args.weapon.spec)
	end
	-- Choose a random action weighted by the score.
	local val = total * math.random()
	for action,score in pairs(actions) do
		val = val - score
		if val <= 0 then
			return action
		end
	end
end

--- Updates the enemy list of the AI.
-- @param self AI.
Ai.scan_enemies = function(self)
	-- Clear old enemies.
	local old = self.enemies
	local time = Program:get_time()
	self.enemies = {}
	-- Find new enemies.
	local objs = Main.objects:find_by_point(self.object:get_position(), 15)
	for k,v in pairs(objs) do
		if not v.dead then
			local enemy = old[v]
			if enemy and time - enemy[2] < 10 then
				-- If the enemy is still nearby and was last seen a very short time
				-- ago, we add it back to the list. Without this, the actor would
				-- give up the moment the target hides behind anything.
				self.enemies[v] = enemy
			elseif self.object:check_enemy(v) then
				-- If a new enemy was within the scan radius, a line of sight check
				-- is performed to cull enemies behind walls. If the LOS check
				-- succeeds, the enemy is considered found.
				if self:check_line_of_sight(v) then
					self.enemies[v] = {v, time}
				end
			end
		end
	end
end

--- Updates the AI.
-- @param self AI.
-- @param secs Seconds since the last update.
Ai.update = function(self, secs)
end

return Ai
