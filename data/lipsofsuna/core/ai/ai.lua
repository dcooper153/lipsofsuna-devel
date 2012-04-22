Ai = Class()
Ai.dict_name = {}

--- Creates a new creature AI.<br/>
-- The AI is inactive when created. It's only activated when the controlled
-- creature enters the vision radius of the player. This allows us to save lots
-- of computing time since player motion often triggers loading of sectors whose
-- creatures are never seen.
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

--- Updates the combat ratings of the creature.
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
	return 1 / ((self.object.position - enemy.position).length + 1)
end

--- Calculates the tilt value for melee attacks.
-- @param self Ai.
-- @return Tilt angle in radians.
Ai.calculate_melee_tilt = function(self)
	-- Calculate distance to the target.
	local diff = self.target.position + self.target.center_offset_physics - self.object.position - self.object.spec.aim_ray_center
	local dist = Vector(diff.x, 0, diff.z).length
	-- Solve the tilt angle analytically.
	local angle = math.atan2(diff.y, dist)
	local limit = self.object.spec.tilt_limit
	angle = math.min(math.max(angle, -limit), limit)
	return Quaternion{euler = {0,0,angle}}
end

--- Calculates the tilt value for ranged attacks.
-- @param self Ai.
-- @return Tilt angle in radians.
Ai.calculate_ranged_tilt = function(self)
	-- Get the ammo type.
	local weapon = self.object:get_weapon()
	if not weapon or not weapon.spec.ammo_type then return Quaternion() end
	local spec = Itemspec:find{name = weapon.spec.ammo_type}
	if not spec then return Quaternion() end
	-- Calculate distance to the target.
	local diff = self.target.position + self.target.center_offset_physics - self.object.position - self.object.spec.aim_ray_center
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
	for k,v in pairs(weapon:get_weapon_influences(self)) do
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

--- Finds the best feat to use in combat.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>category: Feat category.</li>
--   <li>target: Object to be attacked.</li>
--   <li>weapon: Weapon to be used.</li></ul>
-- @return New feat.
Ai.find_best_feat = function(self, args)
	local effect = (self.object == args.target and "beneficial" or "harmful")
	local best_feat = nil
	local best_score = -1
	local process_anim = function(anim)
		-- Check if the feat animation is usable.
		local feat = Feat{animation = anim.name}
		if not feat:usable{user = self.object} then return end
		-- Add best feat effects.
		feat:add_best_effects{category = effect, user = self.object}
		-- Calculate the score.
		-- TODO: Support influences other than health.
		local info = feat:get_info{owner = self.object, object = args.target, weapon = args.weapon}
		local score = (info.influences.health or 0)
		if args.target ~= self then score = score end
		if score < 1 then return end
		score = score + 100 * math.random()
		-- Maintain the best feat.
		if score <= best_score then return end
		best_feat = feat
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

--- Updates the AI.
-- @param self AI.
-- @param secs Seconds since the last update.
Ai.update = function(self, secs)
end
