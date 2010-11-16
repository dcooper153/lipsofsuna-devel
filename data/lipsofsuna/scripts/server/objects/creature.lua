Creature = Class(Object)

Creature.setter = function(self, key, value)
	if key == "species" then
		local spec = type(value) == "string" and Species:find{name = value} or value
		Object.setter(self, key, spec)
		self.model = spec.model
		self.mass = spec.mass
		self.gravity = spec.gravity
		if spec.hair_color then
			self.hair_color = spec.hair_color
		end
		if spec.hair_style then
			self.hair_style = spec.hair_style
		end
		-- Create skills.
		self.skills = self.skills or Skills{owner = self}
		self.skills:clear()
		for k,v in pairs(spec.skills) do
			local prot = v.name == "health" and "public" or "private"
			self.skills:register{
				prot = prot,
				name = v.name,
				maximum = v.val,
				value = v.val,
				regen = Config.skillregen}
		end
		-- Create inventory.
		self.inventory = self.inventory or Inventory{owner = self, size = spec.inventory_size} -- FIXME: Resizing not supported.
		for k,v in pairs(spec.inventory_items) do
			self:add_item{object = Item:create{name = v}}
		end
		self:equip_best_items()
	else
		Object.setter(self, key, value)
	end
end

--- Creates a new creature.
-- @param clss Creature class.
-- @param args Arguments.
Creature.new = function(clss, args)
	local self = Object.new(clss, args)
	self.physics = args and args.physics or "kinematic"
	self.species = args and args.species or "aer"
	self.jumped = args and args.jumped or 0
	self.anim_timer = 0
	self.enemies = {}
	setmetatable(self.enemies, {__mode = "kv"})
	self:calculate_speed()
	self:calculate_combat_ratings()
	self:set_state{state = "wander"}
	self:set_movement(0)

	Thread(function()
		while true do
			local secs = coroutine.yield()
			if not self.realized then return end
			self:update(secs)
		end
	end)

	return self
end

--- Calculates the animation state based on the active controls.
-- @param self Object.
Creature.calculate_animation = function(self)
	local anim
	local back = self.movement < 0
	local front = self.movement > 0
	local left = self.strafing < 0
	local right = self.strafing > 0
	local run = self.running
	-- Select the animation.
	if back then anim = "walk-back"
	elseif front and right then anim = "run-right"
	elseif front and left then anim = "run-left"
	elseif front and run then anim = "run"
	elseif front then anim = "walk"
	elseif left then anim = "strafe-left"
	elseif right then anim = "strafe-right"
	else anim = "idle" end
	-- Playback the animation.
	self:animate{animation = anim, channel = Animation.CHANNEL_WALK, weight = 0.1, permanent = true}
end

--- Updates the combat ratings of the creature.
-- @param self Object.
Creature.calculate_combat_ratings = function(self)
	-- TODO
	self.melee_rating = 1
	self.ranged_rating = 0
end

--- Calculates how desirable it is to attack the given enemy.
-- @param self Object.
-- @param enemy Enemy object.
Creature.calculate_enemy_rating = function(self, enemy)
	-- TODO: Should take enemy weakness into account.
	-- TODO: Should probably take terrain into account by solving paths.
	return 1 / ((self.position - enemy.position).length + 1)
end

--- Calculates the movement speed of the creature.
-- @param self Object.
Creature.calculate_speed = function(self)
	-- Base speed.
	local s = self.running and self.species.speed_run or self.species.speed_walk
	-- Skill bonuses.
	local str = self.skills:get_value{skill = "strength"} or 0
	local agi = self.skills:get_value{skill = "agility"} or 0
	s = s * (1 + agi / 100 + str / 150)
	-- Update speed.
	if s ~= self.speed then
		self.speed = s
		self:calculate_animation()
	end
end

--- Checks if the given object is an enemy of the creature.
-- @param self Object.
-- @param object Object.
-- @return True if the object is an enemy.
Creature.check_enemy = function(self, object)
	if object == self then return end
	if object.dead then return end
	return self.species:check_enemy(object)
end

--- Causes the object to take damage.
-- @param self Object.
-- @param amount Amount of damage.
Creature.damaged = function(self, amount)
	if not self.realized then return end
	local skills = self.skills
	local health = skills:get_value{skill = "health"}
	if not health then return end
	if health - amount > 0 then
		skills:set_value{skill = "health", value = health - amount}
	else
		skills:set_value{skill = "health", value = 0}
		self:die()
	end
end

--- Causes the creature to die.
-- @param self Object.
Creature.die = function(self)
	if self.dead then return end
	-- Death dialog.
	Dialog:start{object = self, type = "die"}
	-- Disable controls.
	self.dead = true
	self.physics = "rigid"
	self.shape = "dead"
	self:set_movement(0)
	self:set_strafing(0)
	-- Disable skills.
	self.skills.enabled = false
	-- Playback animation.
	self:animate{animation = "death", weight = 10}
	self:animate{animation = "dead", channel = Animation.CHANNEL_WALK, weight = 0.1, permanent = true}
	-- Drop held items.
	local o = self:get_item{slot = "hand.L"}
	if o then
		o:detach()
		o.position = self.position
		o.realized = true
	end
	o = self:get_item{slot = "hand.R"}
	if o then
		o:detach()
		o.position = self.position
		o.realized = true
	end
	--Object.die(self)
end

--- Equips the item passed as a parameter.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Item to equip.</li></ul>
-- @return True if equipped successfully.
Creature.equip_item = function(self, args)
	local slot = args.object.itemspec.equipment_slot
	if not slot then return end
	if not self:unequip_item{slot = slot} then return end
	self.inventory:set_object{object = args.object, slot = slot}
	return true
end

--- Automatically equips the best set of items.
-- @param self Object.
-- @param args Arguments.
Creature.equip_best_items = function(self, args)
	-- Loop through all our equipment slots.
	for name in pairs(self.species.equipment_slots) do
		-- Find the best item to place to the slot.
		local best = self:get_item{slot = name}
		for index,item in pairs(self.inventory.slots) do
			if item.itemspec.equipment_slot == name then
				-- TODO: Actually check for armor class etc.
				if not best then best = item end
			end
		end
		-- Place the best item to the slot.
		if best then self:equip_item{object = best} end
	end
end

--- Makes the creature face a point.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>point: Position vector.</li>
--   <li>secs: Tick length.</li></ul>
-- @return The dot product of the current direction and the target direction.
Creature.face_point = function(self, args)
	local sdir = self.rotation * Vector(0, 0, -1)
	local edir = (args.point - self.position):normalize()
	-- Interpolate rotation towards target point.
	-- TODO: Should use args.secs here somehow.
	local quat = Quaternion{dir = Vector(edir.x, 0, edir.z), up = Vector(0, 1, 0)}
	self.rotation = self.rotation:nlerp(quat, 0.9)
	-- Check if facing the target point.
	return sdir:dot(edir)
end

--- Finds the best feat to use in combat.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>category: Feat category.</li></ul>
Creature.find_best_feat = function(self, args)
	local best_feat = nil
	local best_score = -1
	-- Loop through all known feats.
	for feat_name in pairs(self.species.feats) do
		local feat = Feat:find{name = feat_name}
		if feat then
			if feat.categories[args.category] and feat:usable{user = self} then
				-- Calculate feat score.
				-- TODO: Take more factors into account?
				local score = feat.inflict_damage
				-- Maintain the best feat.
				if score > best_score then
					best_feat = feat
					best_score = score
				end
			end
		end
	end
	return best_feat
end

Creature.jump = function(self)
	if self.ground then
		local t = Program.time
		if t - self.jumped > 1.0 then
			local v = self.velocity
			self.jumped = t
			Thread(function()
				self:effect{effect = "hop-000"}
				self:animate{animation = "jump"}
				local t = 0.0
				while t < 0.2 do t = t + coroutine.yield() end
				if self.ground then
					Object.jump(self, {impulse = Vector(v.x, 400, v.z)})
				end
			end)
		end
	end
end

--- Updates the enemy list of the creature.
-- @param self Object.
Creature.scan_enemies = function(self)
	-- Clear old enemies.
	for k,v in pairs(self.enemies) do
		self.enemies[k] = nil
	end
	-- Find new enemies.
	local objs = Object:find{point = self.position, radius = 10}
	for k,v in pairs(objs) do
		if self:check_enemy(v) then
			self.enemies[v] = v
			break
		end
	end
end

--- Sets the forward/backward movement state of the creature.
-- @param self Object.
-- @param value Movement rate.
Creature.set_movement = function(self, value)
	self.movement = math.min(1, math.max(-1, value))
	self:calculate_animation()
end

--- Sets the AI state of the creature.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>state: AI state.</li>
--   <li>target: Targeted object.</li></ul>
Creature.set_state = function(self, args)
	local s = args.state
	if (s == "wander" and not self.species.ai_enable_wander) or
	   (s == "combat" and not self.species.ai_enable_combat) then
		s = "wait"
	end
	self.state = s
	self.target = args.target
	self.state_timer = 0
	self.update_timer = 0
	self.action_timer = 0
	self.action_state = nil
end

--- Sets the strafing state of the creature.
-- @param self Object.
-- @param value Strafing rate.
Creature.set_strafing = function(self, value)
	self.strafing = value
	self:calculate_animation()
end

--- Unequips the item in the given equipment slot.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>slot: Slot name.</li></ul>
-- @return True if the slot is now empty.
Creature.unequip_item = function(self, args)
	local item = self.inventory:get_object{slot = args.slot}
	if not item then return true end
	if not self:add_item{object = item} then return end
	return true
end

--- Updates the state of the creature.
-- @param self Object.
-- @param secs Seconds since the last update.
Creature.update = function(self, secs)
	-- Update animations.
	self.anim_timer = self.anim_timer + secs
	if self.anim_timer > 0.05 then
		self:update_animations{secs = self.anim_timer}
		self.anim_timer = 0
	end
	-- Skip all controls if we are dead.
	if self.dead then
		return
	end
	-- Update feat cooldown.
	if self.cooldown then
		self.cooldown = self.cooldown - secs
		if self.cooldown <= 0 then
			self.cooldown = nil
		end
	end
	-- Skip the rest if AI is disabled.
	if not self.species.ai_enabled then
		self:refresh()
		return
	end
	-- Maintain timers.
	self.state_timer = self.state_timer + secs
	self.update_timer = self.update_timer + secs
	self.action_timer = self.action_timer - secs
	-- Let the current state manipulate the position and other attributes
	-- of the character and trigger state dependent actions such as attacking.
	local func = self.state_updaters[self.state]
	func(self, secs)
	-- Only consider state changes every couple of seconds.
	if self.update_timer < self.species.ai_update_delay then return end
	self:update_ai_state()
end

--- Updates the AI state of the creature.<br/>
-- You can force the AI to recheck its state immediately by calling the function.
-- This can be useful when, for example, the player has initiated a dialog and
-- you want to set the NPC into the chat mode immediately instead of waiting for
-- the next AI update cycle.
-- @param self Object.
Creature.update_ai_state = function(self)
	self.update_timer = 0
	-- Update our combat ratings so that we can correctly estimate our
	-- chances to stand against our enemies.
	self:calculate_combat_ratings()
	-- TODO: Flee if about to die.
	-- Find the best enemy to attack.
	self:scan_enemies()
	local best_enemy = nil
	local best_rating = -1
	if self.species.ai_enable_combat then
		for k,v in pairs(self.enemies) do
			local r = self:calculate_enemy_rating(v)
			if r > best_rating then
				best_enemy = v
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

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Creature.write = function(self)
	return "local self=Creature{" ..
		"angular=" .. serialize_value(self.angular) .. "," ..
		"dead=" .. serialize_value(self.dead) .. "," ..
		"id=" .. serialize_value(self.id) .. "," ..
		"physics=" .. serialize_value(self.physics) .. "," ..
		"position=" .. serialize_value(self.position) .. "," ..
		"rotation=" .. serialize_value(self.rotation) .. "," ..
		"species=" .. serialize_value(self.species.name) .. "}\n" ..
		serialize_skills(self.skills) .. "\n" ..
		"return self"
end

------------------------------------------------------------------------------

-- This table contains the functions called each tick to update the position
-- of the creature and to initiate state specific actions such as attacking.
-- No state changes can occur during these updates.
Creature.state_updaters =
{
	chat = function(self, secs)
		-- Turn towards the target.
		if self.dialog then
			self:set_movement(0)
			if self.dialog.user then
				self:face_point{point = self.dialog.user.position, secs = secs}
			end
		end
	end,
	combat = function(self, secs)
		-- Decide what combat action to perform next.
		if self.action_timer <= 0 then
			local actions = {}
			local spec = self.species
			local dist = (self.target.position - self.position).length
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
			print("SETACTION", self.action_state, self.action_timer, self.movement, self.strafing)
		end
		-- Turn towards the target.
		local face = self:face_point{point = self.target.position, secs = secs}
		if face < 0.5 then
			self:set_movement(0)
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
		local face = self:face_point{point = self.target, secs = secs}
		if face < 0.5 then
			self:set_movement(0)
			return
		end
		-- Move towards the target.
		local dist = (self.target - self.position).length
		if dist < 0.5 then
			self:set_movement(0)
		else
			self:set_movement(0.3)
		end
	end
}

-- This table contains the functions called every couple of seconds to switch
-- the state of the creature. Switching to the combat mode is handled separately
-- so it isn't present here but all other state changes are.
Creature.state_switchers =
{
	chat = function(self)
		if not self.dialog then
			self:set_state{state = "wander"}
		end
	end,
	combat = function(self)
		-- If we were in the combat state, enter the searching mode in hopes
		-- of finding any enemies that are hiding or trying to escape.
		-- TODO: Companion should follow master.
		self:set_state{state = "search"}
		self:set_movement(0)
		self:set_strafing(0)
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
		--if self.state_timer > self.species.ai_search_time then
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
		self.target = self.position + rot * Vector(0, 0, 5)
		-- Switch to waiting mode after wandering enough.
		if self.species.ai_wait_allowed then
			if self.state_timer > self.species.ai_wander_time then
				self:set_state{state = "wait"}
			end
		end
	end
}

-- This table contains the functions called each tick to update the position
-- of the creature when it's in combat.
Creature.combat_updaters =
{
	attack = function(self)
		-- Maintain distance to the target.
		local dist = (self.target.position - self.position).length
		if dist < 1 then
			self.movement = -0.25
		elseif dist > 3 then
			self.movement = 1
		end
		-- Attack when close enough.
		if dist < 2 then
			local f = self:find_best_feat{category = "melee"}
			if f then
				Feat:perform{name = f.name, user = self}
			end
		end
	end,
	defend = function(self)
		-- Maintain distance to the target.
		local dist = (self.target.position - self.position).length
		if dist < 2 then
			self.movement = -0.25
		elseif dist > 3 then
			self.movement = 0.25
		end
	end,
	normal = function(self)
		-- Maintain distance to the target.
		local dist = (self.target.position - self.position).length
		if dist >= 5 then
			self:set_movement(1)
		elseif dist >= 4 then
			self:set_movement(0.5)
		elseif dist <= 1 then
			self:set_movement(-0.5)
		elseif dist <= 2 then
			self:set_movement(-0.25)
		else
			self:set_movement(0)
			self.action_timer = math.max(self.action_timer, 1)
		end
	end,
	strafe = function(self)
	end,
	walk = function(self)
		-- Move toward the target until close.
		local dist = (self.target.position - self.position).length
		if dist < 1 and self.movement > 0 then
			self.action_timer = 0
		end
	end
}

-- This table contains the functions called every time the previous combat action
-- of the creature is completed.
Creature.combat_switchers =
{
	attack = function(self)
		local moves = { 1, 0.5, 0.25 }
		self.action_state = "attack"
		self.action_timer = math.random(2, 4)
		self:set_strafing(0)
		self:set_movement(moves[math.random(3)])
	end,
	defend = function(self)
		self.action_state = "defend"
		self.action_timer = math.random(2, 3)
		self:set_strafing(0)
		self:set_movement(0)
	end,
	normal = function(self)
		self.action_state = "normal"
		self.action_timer = math.random(1, 2)
		self:set_strafing(0)
	end,
	strafe = function(self)
		local moves = { 0.75, -0.75 }
		self.action_state = "strafe"
		self.action_timer = math.random(2, 5)
		self:set_strafing(moves[math.random(2)])
		self:set_movement(0)
	end,
	walk = function(self)
		local moves = { 1, 0.5, 0.25, -0.25, -0.5 }
		self.action_state = "walk"
		self.action_timer = math.random(2, 4)
		self:set_strafing(0)
		self:set_movement(moves[math.random(5)])
	end
}
