require "server/objects/object"

Creature = Class(Object)
Creature.class_name = "Creature"
Creature.dict_id = setmetatable({}, {__mode = "kv"})

Creature:add_getters{
	armor_class = function(s)
		local value = 0
		for k,v in pairs(s.spec.equipment_slots) do
			local i = s.inventory:get_object_by_slot(v.name)
			value = value + (i and i.spec.armor_class or 0)
		end
		return value
	end,
	beheaded = function(s)
		return Bitwise:band(s.flags or 0, Protocol.object_flags.BEHEADED) ~= 0
	end}

Creature:add_setters{
	beheaded = function(s, v)
		s.flags = Bitwise:bor(s.flags or 0, Protocol.object_flags.BEHEADED)
		Vision:event{type = "object-beheaded", object = s}
		local hat = s.inventory:get_object_by_slot("head")
		if hat then
			local p = s.position
			hat:detach()
			hat.position = p + s.rotation * (s.dead and Vector(0,0.5,2) or Vector(0,2,0))
			hat.velocity = Vector(math.random(), math.random(), math.random())
			hat.realized = true
		end
	end,
	spec = function(s, v)
		local spec = type(v) == "string" and Species:find{name = v} or v
		if s.spec == spec then return end
		rawset(s, "__spec", spec)
		s.model = spec.model
		s.mass = spec.mass
		s.friction_liquid = spec.water_friction
		s.gravity = spec.gravity
		s.gravity_liquid = spec.water_gravity
		-- Set appearance.
		-- Only set once when spawned by the map generator or an admin.
		if s.random and spec.eye_style then
			if spec.eye_style == "random" and spec.eye_styles then
				local s = spec.eye_styles[math.random(1, #spec.eye_styles)]
				local rgb = Color:hsv_to_rgb{math.random(), 0.2 + 0.8 * math.random(), math.random()}
				rgb[1] = math.floor(255 * rgb[1] + 0.5)
				rgb[2] = math.floor(255 * rgb[1] + 0.5)
				rgb[3] = math.floor(255 * rgb[1] + 0.5)
				table.insert(rgb, 1, s[2])
				s.eye_style = rgb
			else
				s.eye_style = spec.eye_style
			end
		end
		if s.random and spec.hair_style then
			if spec.hair_style == "random" and spec.hair_styles then
				local h = spec.hair_styles[math.random(1, #spec.hair_styles)]
				local r = math.random(0, 255)
				local g = math.random(0, 255)
				local b = math.random(0, 255)
				s.hair_style = {h[2], r, g, b}
			else
				s.hair_style = spec.hair_style
			end
		end
		-- Assign skills.
		s.skills:clear()
		for k in pairs(spec.skills) do
			s.skills:add(k)
		end
		-- Initialize stats.
		s:update_skills()
		if s.random then
			s.stats:set_value("health", s.stats:get_maximum("health"))
			s.stats:set_value("willpower", s.stats:get_maximum("willpower"))
		end
		-- Create inventory.
		-- When the map generator or an admin command creates an object, the
		-- random field is set to indicate that items should be generated.
		-- The field isn't saved so items are only created once as expected.
		if s.inventory then
			s.inventory = Inventory{id = s.id, size = spec.inventory_size}
		else
			-- FIXME: May break for existing objects that have too many inventory items.
			s.inventory.size = spec.inventory_size
		end
		if s.random then
			for k,v in pairs(spec.inventory_items) do
				local itemspec = Itemspec:find{name = k}
				if itemspec then
					if itemspec.stacking then
						s.inventory:merge_object(Item{spec = itemspec, count = v})
					else
						for i = 1,v do s.inventory:merge_object(Item{spec = itemspec}) end
					end
				else
					print(string.format("WARNING: Creature '%s' uses an invalid inventory item name '%s'", s.spec.name, k))
				end
			end
			s.inventory:equip_best_objects()
		end
		-- Create random loot.
		-- The same about random objects applies as above.
		if s.random and spec.loot_categories then
			local num_cat = #spec.loot_categories
			local num_item
			if spec.loot_count then
				num_item = math.random(spec.loot_count[1], spec.loot_count[2])
			else
				num_item = math.random(0, s.inventory.size)
			end
			for i = 1,num_item do
				local cat = spec.loot_categories[math.random(1, num_cat)]
				local itemspec = Itemspec:random{category = cat}
				if itemspec then
					s.inventory:merge_object(Item{spec = itemspec})
				else
					print(string.format("WARNING: Creature '%s' uses an invalid inventory item category '%s'", s.spec.name, v))
				end
			end
		end
		-- Create the map marker.
		-- Usually the marker exists already but we support creating new markers on
		-- the fly in case that'll be needed in the future.
		if spec.marker then
			s.marker = Marker:find{name = spec.marker}
			if s.marker then
				s.marker.position = s.position
				s.marker.target = s.id
			else
				s.marker = Marker{name = spec.marker, position = s.position, target = s.id}
			end
		end
		-- Kill dead quest characters.
		if spec.dead then
			s.dead = true
		end
		-- Create the AI.
		if not spec.dead and spec.ai_enabled and Ai then
			ai_class = Ai.dict_name[spec.ai_type or "npc"]
			s.ai = ai_class and ai_class(s)
		end
	end}

--- Creates a new creature.
-- @param clss Creature class.
-- @param args Arguments.<ul>
--   <li>angular: Angular velocity.</li>
--   <li>beheaded: True to spawn without a head.</li>
--   <li>body_scale: Scale factor of the body.</li>
--   <li>body_style: Body style defined by an array of scalars.</li>
--   <li>dead: True for a dead creature.</li>
--   <li>eye_style: Eye style defined by an array of {style, red, green, blue}.</li>
--   <li>hair_style: Hair style defined by an array of {style, red, green, blue}.</li>
--   <li>id: Unique object ID or nil for a random free one.</li>
--   <li>jumped: Jump timer.</li>
--   <li>name: Name of the creature.</li>
--   <li>physics: Physics mode.</li>
--   <li>position: Position vector of the creature.</li>
--   <li>rotation: Rotation quaternion of the creature.</li>
--   <li>skills: Skill table of the character.</li>
--   <li>skin_style: Skin style defined by an array of {style, red, green, blue}.</li>
--   <li>spec: Species of the creature.</li>
--   <li>realized: True to add the object to the simulation.</li></ul>
Creature.new = function(clss, args)
	local self = Object.new(clss, {id = args.id})
	local copy = function(n, d)
		if args[n] ~= nil or d then
			self[n] = (args[n] ~= nil) and args[n] or d
		end
	end
	self.attributes = {}
	self.skills = Skills{id = self.id}
	self.stats = Stats{id = self.id}
	copy("angular")
	copy("beheaded")
	copy("body_scale")
	copy("body_style")
	copy("eye_style")
	copy("face_style")
	copy("hair_style")
	copy("home_point")
	copy("jumped", 0)
	copy("name")
	copy("physics", "kinematic")
	copy("random")
	copy("rotation")
	copy("position")
	copy("skin_style")
	copy("carried_weight", 0)
	copy("spec")
	copy("variables")
	clss.dict_id[self.id] = self
	self.update_timer = 0.1 * math.random()
	if args and args.dead then self:set_dead_state() end
	copy("realized")
	-- Initialize skills.
	if args.skills then
		self.skills:clear()
		for k,v in pairs(args.skills) do
			self.skills:add(k)
		end
	end
	self:update_skills()
	return self
end

--- Clones the object.
-- @param self Object.
-- @return New object.
Creature.clone = function(self)
	local variables = {}
	for k,v in pairs(self.variables) do variables[k] = v end
	return Creature{
		angular = self.angular,
		beheaded = self.beheaded,
		dead = self.dead,
		eye_style = self.eye_style,
		face_style = self.face_style,
		hair_style = self.hair_style,
		physics = self.physics,
		position = self.position,
		rotation = self.rotation,
		spec = self.spec,
		variables = variables}
end

--- Adds an object to the list of known enemies.<br/>
-- This function skips faction checks and adds the object directly to the
-- list. Hence, calling this temporarily makes the creature angry at the
-- passed object.
-- @param self Object.
-- @param object Object to add to the list of enemies.
Creature.add_enemy = function(self, object)
	if not self.ai then return end
	if object.god then return end
	self.ai:add_enemy(object)
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
	if back then self:animate("walk back")
	elseif front and right then self:animate("run right")
	elseif front and left then self:animate("run left")
	elseif front and run and not self.blocking then self:animate("run")
	elseif front then self:animate("walk")
	elseif left then self:animate("strafe left")
	elseif right then self:animate("strafe right")
	else self:animate("idle") end
end

--- Calculates the movement speed of the creature.
-- @param self Object.
Creature.calculate_speed = function(self)
	-- Base speed.
	local s = (self.running and not self.blocking) and self.spec.speed_run or self.spec.speed_walk
	-- Skill bonuses.
	s = s * self.attributes.speed
	-- Burdening penalty.
	if self:get_burdened() then
		s = math.max(1, s * 0.3)
	end
	-- Update speed.
	if s ~= self.speed then
		self.speed = s
		self:calculate_animation()
	end
end

--- Checks if the creature could climb over a low wall.
-- @param self Creature.
-- @return True if could climb.
Creature.can_climb_low = function(self)
	if self.movement < 0 then return end
	local ctr = self.position * Voxel.tile_scale + Vector(0,0.5,0)
	local dir = self.rotation * Vector(0,0,-1)
	local dst = (ctr + dir):floor()
	local f1 = Voxel:get_tile(dst)
	local f2 = Voxel:get_tile(dst + Vector(0,1,0))
	local f3 = Voxel:get_tile(dst + Vector(0,2,0))
	return f1 ~= 0 and f2 == 0 and f3 == 0
end

--- Checks if the creature could climb over a high wall.
-- @param self Creature.
-- @return True if could climb.
Creature.can_climb_high = function(self)
	if self.movement < 0 then return end
	local ctr = self.position * Voxel.tile_scale + Vector(0,0.5,0)
	local dir = self.rotation * Vector(0,0,-1)
	local dst = (ctr + dir):floor()
	local f1 = Voxel:get_tile(dst + Vector(0,1,0))
	local f2 = Voxel:get_tile(dst + Vector(0,2,0))
	local f3 = Voxel:get_tile(dst + Vector(0,3,0))
	return f1 ~= 0 and f2 == 0 and f3 == 0
end

--- Checks line of sight to the target point or object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>object: Object being looked for.</li>
--   <li>point: Point being looked for.</li></ul>
-- @return True if seen.
Creature.check_line_of_sight = function(self, args)
	-- TODO: Take stealth into account.
	local src
	local dst
	-- Get the vision ray.
	-- TODO: Take bounding box into account.
	if args.point then
		src = self.position + Vector(0,1,0)
		dst = args.point
	elseif args.object then
		src = self.position + Vector(0,1,0)
		dst = args.object.position + Vector(0,1,0)
	end
	-- Check for view cone.
	local ray = (src - dst):normalize()
	local look = self.rotation * Vector(0,0,-1)
	if math.acos(ray:dot(look)) > self.spec.view_cone then
		return
	end
	-- Check for ray cast success.
	-- TODO: Shoot multiple rays?
	if args.point then
		local ret = Physics:cast_ray{src = src, dst = dst, ignore = self}
		return not ret or (ret.point - dst).length < 0.5
	elseif args.object then
		local ret = Physics:cast_ray{src = src, dst = dst, ignore = self}
		if not ret or ret.object == args.object then return true end
	end
end

--- Checks if the given object is an enemy of the creature.
-- @param self Object.
-- @param object Object.
-- @return True if the object is an enemy.
Creature.check_enemy = function(self, object)
	if object == self then return end
	if object.dead then return end
	if object.god then return end
	return self.spec:check_enemy(object)
end

--- Causes the object to take damage.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>amount: Amount of damage.</li>
--   <li>point: Damage point.</li>
--   <li>type: Damage type.</li></ul>
Creature.damaged = function(self, args)
	-- Check for invulnerability.
	if not self.realized then return end
	if self.god then return end
	local health = self.stats:get_value("health")
	if not health then return end
	-- Reduce health.
	if args.amount < 0 then
		local max = self.stats:get_maximum("health")
		self.stats:set_value("health", math.min(health - args.amount, max))
	elseif health - args.amount > 0 then
		self.stats:set_value("health", health - args.amount)
	else
		-- Behead if the blow was strong.
		-- Players can't be beheaded with first hit.
		if not self.beheaded then
			if self.dead then
				if args.amount > 15 then self.beheaded = true end
			elseif not self.client then
				if args.amount > 30 then self.beheaded = true end
			end
		end
		-- Say the death message.
		-- This is omitted if the actor is beheaded or killed by a surprise.
		if not self.dead and not self.beheaded and self.ai then
			for k,v in pairs(self.ai.enemies) do
				local p = self.spec:get_personality()
				local s = p and p:get_phrase("death")
				if s then self:say(s) end
				break
			end
		end
		-- Kill if not killed already.
		self.stats:set_value("health", 0)
		self:die()
	end
	-- Play the damage effect.
	-- TODO: Should depend on the attack type.
	-- TODO: Should depend on the damage magnitude.
	if args.type == "physical" and self.spec.effect_physical_damage and args.amount > 0 then
		Effect:play{effect = self.spec.effect_physical_damage, object = not args.point and self, point = args.point}
	end
end

--- Gets the spell effects known by the object.
-- @param self Object.
-- @return Dictionary of booleans.
Creature.get_known_spell_effects = function(self)
	return self.spec.feat_effects
end

--- Gets the spell types known by the object.
-- @param self Object.
-- @return Dictionary of booleans.
Object.get_known_spell_types = function(self)
	return self.spec.feat_anims
end

Creature.get_weapon = function(self)
	return self.inventory:get_object_by_slot(self.spec.weapon_slot)
end

Creature.set_weapon = function(self, value)
	return self.inventory:equip_object(value, self.spec.weapon_slot)
end

Creature.set_dead_state = function(self, drop)
	-- Playback animation.
	-- This needs to be done first because setting the 'dead' member will
	-- prevent any future animation changes.
	self:animate("death")
	-- Disable controls.
	self.dead = true
	self:set_movement(0)
	self:set_strafing(0)
	self.auto_attack = nil
	self.jumping = nil
	self.climbing = nil
	self.shape = "dead"
	self.physics = "rigid"
	-- Disable stats.
	self.stats.enabled = false
	self.stats:set_value("health", 0)
	-- Drop held items.
	if drop then
		local o = self.inventory:get_object_by_slot("hand.L")
		if o then
			o:detach()
			o.position = self.position
			o.realized = true
		end
		o = self:get_weapon()
		if o then
			o:detach()
			o.position = self.position
			o.realized = true
		end
	end
	-- Emit a vision event.
	Vision:event{type = "object-dead", object = self, dead = true}
end

Creature.climb = function(self)
	if self.blocking then return end
	if self.climbing then return end
	if self:can_climb_high() then
		self.jumping = nil
		self.climbing = true
		self:animate("climb high")
		Coroutine(function()
			-- Rise.
			local t = 0
			local p = self.position
			local r = self.rotation
			repeat
				local d = coroutine.yield()
				t = t + d
				self.position = p + Vector(0,2*t,0)
				self.velocity = Vector()
			until t > 0.8 * Voxel.tile_size
			-- Slide.
			t = 0
			p = self.position
			repeat
				local d = coroutine.yield()
				t = t + d
				self.position = p + r * Vector(0,0.3,-0.8) * t
			until t > 1
			self.climbing = nil
		end)
	elseif self:can_climb_low() then
		self.jumping = nil
		self.climbing = true
		self:animate("climb low")
		Coroutine(function()
			-- Rise.
			local t = 0
			local p = self.position
			local r = self.rotation
			repeat
				local d = coroutine.yield()
				t = t + d
				self.position = p + Vector(0,4*t,0)
				self.velocity = Vector()
			until t > 0.2 * Voxel.tile_size
			-- Slide.
			t = 0
			p = self.position
			repeat
				local d = coroutine.yield()
				t = t + d
				self.position = p + r * Vector(0,0.3,-1) * 2 * t
			until t > 0.5
			self.climbing = nil
		end)
	end
end

--- Causes the creature to die.
-- @param self Object.
-- @return True if killed, false if saved by Sanctuary.
Creature.die = function(self)
	if self.dead then return end
	-- Sanctuary save.
	if self.modifiers and self.modifiers.sanctuary and not self.beheaded then
		self:remove_modifier("sanctuary")
		self:damaged{amount = -10, type = "physical"}
		self:teleport{marker = "sanctuary"}
		return
	end
	-- Death dialog.
	if self.spec.dialog then
		local dialog = Dialog{object = self, name = self.spec.dialog .. " death"}
		if dialog then
			self.dialog = dialog
			self.dialog:execute()
			self.dialog = nil
		end
	end
	-- Disable controls etc.
	self:set_dead_state(true)
	return true
end

--- Makes the creature face a point.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>point: Position vector.</li>
--   <li>secs: Tick length or nil for instant rotation.</li></ul>
-- @return The dot product of the current direction and the target direction.
Creature.face_point = function(self, args)
	local sdir = self.rotation * Vector(0, 0, -1)
	local edir = (args.point - self.position):normalize()
	local quat = Quaternion{dir = Vector(edir.x, 0, edir.z), up = Vector(0, 1, 0)}
	if args.secs then
		-- Interpolate rotation towards target point.
		-- TODO: Should use args.secs here somehow.
		self.rotation = self.rotation:nlerp(quat, 0.9)
		return sdir:dot(edir)
	else
		-- Instantly set the target rotation.
		self.rotation = quat
		return 1.0
	end
end

--- Gets the rotation quaternion towards the given point.
-- @param self Object.
-- @param point Position vector in world space.
-- @return Quaternion.
Creature.get_rotation_to_point = function(self, point)
	local dir = (point - self.position):normalize()
	dir.y = 0
	return Quaternion{dir = dir, up = Vector(0, 1, 0)}
end

--- Gets the attack ray of the creature.
-- @param self Object.
-- @param rel Destination shift vector or nil.
-- @return Ray start point and ray end point relative to the object.
Creature.get_attack_ray = function(self, rel)
	local ctr = self.spec.aim_ray_center
	local ray1 = Vector(0, 0, -self.spec.aim_ray_start)
	local ray2 = Vector(0, 0, -self.spec.aim_ray_end)
	if rel then ray2 = ray2 + rel * self.spec.aim_ray_end end
	if self.tilt then
		local rot = Quaternion{euler = self.tilt.euler}
		local src = self.position + self.rotation * (ctr + rot * ray1)
		local dst = self.position + self.rotation * (ctr + rot * ray2)
		return src, dst
	else
		local src = self.position + self.rotation * (ctr + ray1)
		local dst = self.position + self.rotation * (ctr + ray2)
		return src, dst
	end
end

--- Returns the burdening limit of the creature.
-- @param self creature.
-- @return Burdening limit in kilograms
Creature.get_burden_limit = function(self)
	-- TODO
	return 500
end

--- Returns true if the creature is burdened.
-- @param self Creature.
-- @return True if burdened.
Creature.get_burdened = function(self)
	return self.carried_weight > self:get_burden_limit()
end

--- Gets a modifier by name.
-- @param self Object.
-- @param name Modifer name.
Creature.get_modifier = function(self, name)
	if not self.modifiers then return end
	return self.modifiers[name]
end

--- Inflicts a modifier on the object.
-- @param self Object.
-- @param name Modifier name.
-- @param strength Modifier strength.
-- @param args Arguments passed to the modifier
Creature.inflict_modifier = function(self, name, strength, args)
	if not self.modifiers then self.modifiers = {} end
	if not self.modifiers[name] or self.modifiers[name].st < strength then
		self.modifiers[name] = {st=strength,a=args}
	end
end

Creature.jump = function(self)
	-- Check for preconditions.
	if self.blocking then return end
	if self.climbing then return end
	local t = Program.time
	if t - self.jumped < 0.5 then return end
	-- Jump or swim.
	if self.submerged and self.submerged > 0.4 then
		-- Swimming upwards.
		if self:get_burdened() then return end
		local v = self.velocity
		self.jumped = t - 0.3
		self.jumping = true
		if v.y < self.speed then
			Object.jump(self, {impulse = Vector(v.x, self.spec.swim_force * self.spec.mass, v.z)})
		end
	else
		-- Jumping.
		if not self.ground or self:get_burdened() then return end
		self.jumped = t
		self.jumping = true
		Effect:play{effect = "jump1", object = self}
		self:animate("jump")
		Coroutine(function(thread)
			Coroutine:sleep(self.spec.timing_jump * 0.02)
			if not self.realized then return end
			local v = self.velocity
			Object.jump(self, {impulse = Vector(v.x, self.spec.jump_force * self.spec.mass, v.z)})
		end)
	end
end

--- Loots the object.
-- @param self Object.
-- @param user Object doing the looting.
Creature.loot = function(self, user)
	return Object.loot(self, user)
end

--- Picks up an object.
-- @param self Object.
-- @param src_id ID of the picked up object.
-- @param dst_id ID of the inventory where to place the object.
-- @param dst_slot Name of the inventory slot where to place the object.
Creature.pick_up = function(self, src_id, dst_id, dst_slot)
	self:animate("pick up")
	Timer{delay = self.spec.timing_pickup * 0.02, func = function(timer)
		Actions:move_from_world_to_inv(self, src_id, dst_id, dst_slot)
		timer:disable()
	end}
end

--- Removes a modifier.
-- @param self Object.
-- @param name Modifier name.
Creature.remove_modifier = function(self, name)
	if self.modifiers and self.modifiers[name] then
		self.modifiers[name] = nil
		self:removed_modifier(name)
	end
end

--- Called when a modifier is removed.
-- @param self Object.
-- @param name Modifier name.
Creature.removed_modifier = function(self, name)
end

--- Enables or disables the blocking stance.
-- @param self Creature.
-- @param value True to block.
Creature.set_block = function(self, value)
	if self.jumping then return end
	if value and self.blocking then return end
	if not value and not self.blocking then return end
	if value then
		self.blocking = Program.time
		self:animate("block start")
	else
		self.blocking = nil
		self:animate("block stop")
	end
	self:calculate_speed()
	self:calculate_animation()
end

--- Sets the forward/backward movement state of the creature.
-- @param self Object.
-- @param value Movement rate.
Creature.set_movement = function(self, value)
	self.movement = math.min(1, math.max(-1, value))
	self:calculate_animation()
end

--- Sets the strafing state of the creature.
-- @param self Object.
-- @param value Strafing rate.
Creature.set_strafing = function(self, value)
	self.strafing = value
	self:calculate_animation()
end

--- Teleports the object.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>marker: Map marker name.</li>
--   <li>position: World position.</li>
--   <li>region: Region name.</li></ul>
-- @return True on success.
Creature.teleport = function(self, args)
	if Object.teleport(self, args) then
		-- Update skills.
		-- As long as actors always teleport between the overworld and the
		-- underworld, this ensures that view distance is update correctly.
		self:update_skills()
		return true
	end
end

--- Updates the state of the creature.
-- @param self Object.
-- @param secs Seconds since the last update.
Creature.update = function(self, secs)
	-- Update the state.
	self.update_timer = self.update_timer + secs
	if self.update_timer > 0.3 then
		local tick = self.update_timer
		if self.modifiers then Modifier:update(self, tick) end
		self.stats:update(tick)
		self:update_actions(tick)
		self:update_burdening(tick)
		self:update_environment(tick)
		self.update_timer = 0
	end
	-- Update the AI.
	if self.ai and not self.dead then
		self.ai:update(secs)
	end
end

Creature.update_actions = function(self, secs)
	-- Update flying.
	if self.flying and self.tilt then
		local v = self.velocity
		if math.abs(self.movement) > 0.5 then
			local e = self.tilt.euler[3]
			if e > 0 then
				self.velocity = Vector(v.x, math.max(v.y, 5*math.sin(e)), v.z)
			else
				self.velocity = Vector(v.x, math.min(v.y, 5*math.sin(e)), v.z)
			end
		else
			self.velocity = Vector()
		end
	end
	-- Update feat cooldown.
	if self.blocking then self.cooldown = self.spec.blocking_cooldown end
	if self.cooldown then
		self.cooldown = self.cooldown - secs
		if self.cooldown <= 0 then
			self.cooldown = nil
		end
	end
	-- Update auto-attack.
	if self.auto_attack and not self.cooldown and not self.attack_charge then
		self:attack_charge_start()
		self:attack_charge_end()
	end
	-- Check for falling damage.
	-- Don't update every frame.
	self.fall_timer = (self.fall_timer or 0) + secs
	if self.fall_timer > 0.3 then
		if self.velocity_prev then
			local limity = self.spec.falling_damage_speed
			local prevy = self.velocity_prev.y
			local diffy = self.velocity.y - prevy
			if prevy < -limity and diffy > limity then
				local damage = (diffy - limity) * self.spec.falling_damage_rate
				if damage > 2 then
					self:damaged{amount = damage, type = "falling"}
					if self.spec.effect_falling_damage then
						Effect:play{effect = self.spec.effect_falling_damage, object = self}
					end
				end
			end
		end
		self.velocity_prev = self.velocity
	end
	-- Play the landing animation after jumping.
	-- Initiate climbing when applicable.
	if self.jumping then
		-- Climbing.
		-- Player have an explicit climb command so auto-climb is only used by NPCs.
		if not self.client then
			self:climb()
		end
		-- Landing.
		self.jump_timer = (self.jump_timer or 0) + secs
		if self.jump_timer > 0.2 and Program.time - self.jumped > 0.8 and self.ground then
			if not self.submerged or self.submerged < 0.3 then
				self:animate("land ground")
				if self.spec.effect_landing then
					Effect:play{effect = self.spec.effect_landing, object = self}
				end
			else
				self:animate("land water")
			end
			self.jumping = nil
		end
	end
end
--- Updates the AI state of the creature.<br/>
-- You can force the AI to recheck its state immediately by calling the function.
-- This can be useful when, for example, the player has initiated a dialog and
-- you want to set the NPC into the chat mode immediately instead of waiting for
-- the next AI update cycle.
-- @param self Object.
Creature.update_ai_state = function(self)
	if self.ai then
		self.ai:update_state()
	end
end

Creature.update_burdening = function(self, secs)
	-- Don't update every frame.
	self.burden_timer = (self.burden_timer or 0) + secs
	if self.burden_timer < 1 then return end
	self.burden_timer = 0
	-- Update burdening.
	-- The burdening status may change when the contents of the inventory
	-- or any subinventory change or if the strength skill level changes.
	-- The burdening status decreases movement speed and disables jumping.
	local prev_limit = self.burden_limit or 0
	local curr_limit = math.floor(self:get_burden_limit())
	local prev_burden = self:get_burdened()
	local curr_weight = math.ceil(self.inventory:calculate_weight())
	if curr_weight ~= self.carried_weight or prev_limit ~= curr_limit then
		self.carried_weight = curr_weight
		self.burden_limit = curr_limit
		self:send{packet = Packet{packets.INVENTORY_WEIGHT, "uint16", curr_weight, "uint16", curr_limit}}
	end
	local curr_burden = self:get_burdened()
	if prev_burden ~= curr_burden then
		if curr_burden then
			self:send{packet = Packet{packets.MESSAGE, "string", "You're now burdened."}}
		else
			self:send{packet = Packet{packets.MESSAGE, "string", "You're no longer burdened."}}
		end
	end
	-- Update speed.
	-- Skill regeneration affects speed too so this needs to be recalculated
	-- every now and then regardless of burdening.
	self:calculate_speed()
end

--- Updates the environment of the object and tries to fix it if necessary.
-- @param self Object.
-- @param secs Seconds since the last update.
-- @return Boolean and environment statistics. The boolean is true if the object isn't permanently stuck.
Creature.update_environment = function(self, secs)
	-- Don't update every frame.
	if not self.realized then return true end
	self.env_timer = (self.env_timer or 0) + secs
	if self.env_timer < 1.2 then return true end
	local tick = self.env_timer
	self.env_timer = 0
	-- Count tiles affecting us.
	local src,dst = self:get_tile_range()
	local env = Voxel:check_range(src, dst)
	if not env then return true end
	-- Count liquid tiles.
	local liquid = env.liquid / env.total
	local magma = env.magma / env.total
	if liquid ~= (self.submerged or 0) then
		self.submerged = liquid > 0 and liquid or nil
	end
	if magma ~= (self.submerged_in_magma or 0) then
		self.submerged_in_magma = magma > 0 and magma or nil
	end
	-- Apply liquid damage.
	if not self.dead and self.submerged then
		local magma = self.submerged_in_magma or 0
		local water = self.submerged - magma
		if magma > 0 and self.damage_from_magma ~= 0 then
			self:damaged{amount = self.spec.damage_from_magma * magma * tick, type = "liquid"}
		end
		if water > 0 and self.damage_from_water ~= 0 then
			self:damaged{amount = self.spec.damage_from_water * water * tick, type = "liquid"}
		end
	end
	return true, env
end

--- Updates the skills and related attributes of the creature.
-- @param self Object.
Creature.update_skills = function(self)
	-- Calculate the attributes.
	local attr = self.skills:calculate_attributes()
	-- Assign the attributes to the creature.
	self.attributes = attr
	self.stats:set_maximum("health", attr.max_health)
	self.stats:set_maximum("willpower", attr.max_willpower)
	-- Update the movement speed.
	self:calculate_speed()
	-- Update the vision radius.
	if self.vision then
		if self.position.y > 1600 then
			attr.view_distance = attr.view_distance * 1.5
		end
		self.vision.radius = attr.view_distance
	end
end

--- Serializes the object to a string.
-- @param self Object.
-- @return Data string.
Creature.write = function(self)
	return string.format("local self=Creature%s\n%s%s%s%s", serialize{
		angular = self.angular,
		beheaded = self.beheaded or nil,
		dead = self.dead,
		eye_style = self.eye_style,
		face_style = self.face_style,
		hair_style = self.hair_style,
		id = self.id,
		physics = self.physics,
		position = self.position,
		rotation = self.rotation,
		spec = self.spec.name,
		variables = self.variables},
		Serialize:encode_skills(self.skills),
		Serialize:encode_stats(self.stats),
		Serialize:encode_inventory(self.inventory),
		"return self")
end

--- Reads the object from a database.
-- @param self Object.
-- @param db Database.
Creature.read_db = function(self, db)
	Serialize:load_object_inventory(self)
	Serialize:load_object_skills(self)
	Serialize:load_object_stats(self)
end

--- Writes the object to a database.
-- @param self Object.
-- @param db Database.
Creature.write_db = function(self, db)
	-- Write the object.
	local data = string.format("return Creature%s", serialize{
		angular = self.angular,
		beheaded = self.beheaded or nil,
		dead = self.dead,
		eye_style = self.eye_style,
		face_style = self.face_style,
		hair_style = self.hair_style,
		home_point = self.home_point,
		id = self.id,
		physics = self.physics,
		position = self.position,
		rotation = self.rotation,
		spec = self.spec.name,
		variables = self.variables})
	db:query([[REPLACE INTO object_data (id,type,data) VALUES (?,?,?);]], {self.id, "actor", data})
	-- Write the sector.
	if self.sector then
		db:query([[REPLACE INTO object_sectors (id,sector) VALUES (?,?);]], {self.id, self.sector})
	else
		db:query([[DELETE FROM object_sectors where id=?;]], {self.id})
	end
	-- Write the inventory contents.
	db:query([[DELETE FROM object_inventory WHERE parent=?;]], {self.id})
	for index,object in pairs(self.inventory.stored) do
		object:write_db(db, index)
	end
	-- Write skills.
	db:query([[DELETE FROM object_skills WHERE id=?;]], {self.id})
	for name,value in pairs(self.skills.skills) do
		db:query([[REPLACE INTO object_skills (id,name) VALUES (?,?);]], {self.id, name})
	end
	-- Write stats.
	db:query([[DELETE FROM object_stats WHERE id=?;]], {self.id})
	for name,args in pairs(self.stats.stats) do
		db:query([[REPLACE INTO object_stats (id,name,value) VALUES (?,?,?);]], {self.id, name, args.value})
	end
end
