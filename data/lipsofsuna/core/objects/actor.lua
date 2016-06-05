--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.actor
-- @alias Actor

local Class = require("system/class")
local Coroutine = require("system/coroutine")
local Modifier = require("core/combat/modifier")
local ObjectSerializer = require("core/objects/object-serializer")
local Physics = require("system/physics")
local Quaternion = require("system/math/quaternion")
local SimulationObject = require("core/objects/simulation")
local Skills = require("core/skills/skills")
local Stats = require("core/server/stats")
local Time = require("system/time")
local Timer = require("system/timer")
local Vector = require("system/math/vector")

--- TODO:doc
-- @type Actor
local Actor = Class("Actor", SimulationObject)
Actor.serializer = ObjectSerializer(
{
	"base",
	"fields",
	"inventory",
	"sector",
	"skills",
	"stats",
	"companion"
},
{
	{
		name = "angular",
		type = "vector",
		get = function(self) return self.physics:get_angular() end,
		set = function(self, v) return self.physics:set_angular(v) end
	},
	{
		name = "animation_profile",
		type = "string",
		get = function(self) return self.animation_profile end,
		set = function(self, v) self.animation_profile = v end
	},
	{
		name = "beheaded",
		type = "boolean",
		get = function(self) return self:get_beheaded() end,
		set = function(self, v) self:set_beheaded(v) end
	},
	{
		name = "body_scale",
		type = "number"
	},
	{
		name = "body_sliders",
		type = "number list"
	},
	{
		name = "brow_style",
		type = "string"
	},
	{
		name = "companion",
		type = "number",
		get = function(self) return self.companion and self.companion:get_id() end,
		set = function(self, v) self.companion = v end
	},
	{
		name = "dead",
		type = "boolean",
		set = function(self, v) if v then self:set_dead_state() end end
	},
	{
		name = "eye_color",
		type = "number list"
	},
	{
		name = "eye_style",
		type = "string"
	},
	{
		name = "face_sliders",
		type = "number list"
	},
	{
		name = "face_style",
		type = "string"
	},
	{
		name = "hair_color",
		type = "number list"
	},
	{
		name = "hair_style",
		type = "string"
	},
	{
		name = "head_style",
		type = "string"
	},
	{
		name = "home_point",
		type = "vector"
	},
	{
		name = "important",
		type = "boolean",
		get = function(self) return self:get_important() end,
		set = function(self, v) self:set_important(v) end
	},
	{
		name = "mouth_style",
		type = "string"
	},
	{
		name = "position",
		type = "vector",
		get = function(self) return self:get_position() end,
		set = function(self, v) self:set_position(v) end
	},
	{
		name = "rotation",
		type = "quaternion",
		get = function(self) return self:get_rotation() end,
		set = function(self, v) self:set_rotation(v) end
	},
	{
		name = "skin_color",
		type = "number list"
	},
	{
		name = "skin_style",
		type = "string"
	}
})

local __quat1 = Quaternion()
local __quat2 = Quaternion()
local __vec1 = Vector()
local __vec2 = Vector()
local __vec3 = Vector()
local __vec4 = Vector()

--- Creates a new actor.
-- @param clss Actor class.
-- @param manager Object manager.
-- @param id Unique object ID. Nil for a random free one.
-- @return Actor.
Actor.new = function(clss, manager, id)
	local self = SimulationObject.new(clss, manager, id)
	self.attributes = {}
	self.carried_weight = 0
	self.skills = Skills(self:get_id())
	self.stats = Stats(self:get_id())
	self.update_timer = 0.1 * math.random()
	self.physics:set_physics("kinematic")
	return self
end

--- Clones the object.
-- @param self Actor.
-- @return New object.
Actor.clone = function(self)
	-- TODO: Copy dialog variables?
	local o = self.manager:create_object("Actor")
	o:set_spec(self:get_spec())
	o:set_beheaded(self:get_beheaded())
	o:set_dead(self.dead)
	o:set_position(self:get_position())
	o:set_rotation(self:get_rotation())
	o.brow_style = self.brow_style
	o.eye_style = self.eye_style
	o.face_sliders = self.face_sliders
	o.face_style = self.face_style
	o.hair_style = self.hair_style
	o.head_style = self.head_style
	o.mouth_style = self.mouth_style
	o.physics:set_angular(self.physics:get_angular())
	o.physics:set_physics(self.physics:get_physics())
	return o
end

--- Adds an object to the list of known enemies.<br/>
-- This function skips faction checks and adds the object directly to the
-- list. Hence, calling this temporarily makes the actor angry at the
-- passed object.
-- @param self Actor.
-- @param object Object to add to the list of enemies.
Actor.add_enemy = function(self, object)
	if not self.ai then return end
	if object.god then return end
	if object == self.owner then return end
	self.ai:add_enemy(object)
end

--- Adds a modifier to the object.
-- @param self Actor.
-- @param name Modifier name.
-- @param strength Strength.
-- @param caster Caster object. Nil for self.
-- @param point Impact point. Nil for default.
-- @return Modifier if effect-over-time. Nil otherwise.
Actor.add_modifier = function(self, name, strength, caster, point)
	if not self.modifiers then self.modifiers = {} end
	local old = self.modifiers[name]
	if old then
		-- TODO: What to do with caster and point?
		local ret = old:restart(strength)
		if not ret then
			self.modifiers[name] = nil
		end
		self:update_skills()
		if not ret then return end
		return old
	else
		local spec = Main.specs:find_by_name("ModifierSpec", name)
		if not spec then return end
		local m = Modifier(spec, self, caster or self, point or self:get_position())
		if not m:start(strength) then return end
		self.modifiers[name] = m
		self:update_skills()
		return m
	end
end

--- Calculates the animation state based on the active controls.
-- @param self Actor.
Actor.calculate_animation = function(self)
	if not self:has_server_data() then return end
	local m = self:get_movement()
	local s = self:get_strafing()
	local anim
	local back = m < 0
	local front = m > 0
	local left = s < 0
	local right = s > 0
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

--- Calculates the movement speed of the actor.
-- @param self Actor.
Actor.calculate_speed = function(self)
	-- Base speed.
	if not self:get_spec() then return end
	local s = (self.running and not self.blocking) and self.spec.speed_run or self.spec.speed_walk
	-- Skill bonuses.
	s = s * self.attributes.speed
	-- Burdening penalty.
	if self:get_burdened() then
		s = math.max(1, s * 0.3)
	end
	-- Update speed.
	if s ~= self.physics:get_speed() then
		self.physics:set_speed(s)
		self:calculate_animation()
	end
end

--- Checks if the actor could climb over terrain.
-- @param self Actor.
-- @return Height above the actor at which the new surface is, or nil if not possible.
Actor.can_climb = function(self)
	if self:get_movement() < 0 then return end
	local ctr = self:get_position():copy()
	local dir = self:get_rotation() * Vector(0,0,-1)
	local dst = (ctr + dir)
	local yempty
	--check higher positions first.
	yempty = Main.terrain.terrain:find_nearest_empty_stick(dst.x, dst.z, dst.y + 2, 2) - dst.y
	if yempty >= 3 then
		--Position is too high, try again lower.
		yempty = Main.terrain.terrain:find_nearest_empty_stick(dst.x, dst.z, dst.y + 1, 2) - dst.y
	end
	if yempty > 0 and yempty < 3 then
		return yempty
	end
	return nil
end

--- Checks if the given object is an enemy of the actor.
-- @param self Actor.
-- @param object Object.
-- @return True if the object is an enemy.
Actor.check_enemy = function(self, object)
	-- Don't attack uninteresting targets.
	if object == self then return end
	if object.dead then return end
	if object.god then return end
	-- Enemy check for companions.
	if self.owner then
		if object == self.owner then return end
		if self.owner:check_enemy(object) then return true end
	end
	-- Default enemy check.
	return self.spec:check_enemy(object)
end

Actor.climb = function(self)
	if self.blocking then return end
	if self.climbing then return end
	local climbheight = self:can_climb()
	if climbheight then
		self.jumping = nil
		self.climbing = true
		local timerise, timeslide
		local heightrise, heightslide
		if climbheight > 1.5 then
			self:animate("climb high")
			timerise = 0.8
			timeslide = 1.5
			heightrise = 0.8
		else
			self:animate("climb low")
			timerise = 0.2
			timeslide = 0.5
			heightrise = 0.8
		end
		heightslide = 1 - heightrise
		Coroutine(function()
			-- Rise.
			local t = 0
			local startpos = self:get_position():copy()
			local p = startpos:copy()
			local r = self:get_rotation():copy()
			local z = Vector()
			repeat
				local d = coroutine.yield()
				t = t + d
				self:set_position(p + Vector(0, climbheight * heightrise * t / timerise,0))
				self:set_velocity(z)
			until t > timerise
			-- Slide.
			t = t - timerise
			p = p + Vector(0, climbheight * heightrise,0)
			repeat
				local d = coroutine.yield()
				t = t + d
				self:set_position(p + Vector(0, climbheight * heightslide * t / timeslide, 0) + r * Vector(0,0,-1) * (t / timeslide))
				self:set_velocity(z)
			until t >= timeslide
			self.climbing = nil
			self:set_position(startpos + Vector(0, climbheight,0) + r * Vector(0,0,-1))
		end)
	end
end

--- Causes the object to take damage.
-- @param self Actor.
-- @param args Arguments.<ul>
--   <li>amount: Amount of damage.</li>
--   <li>point: Damage point.</li>
--   <li>type: Damage type.</li></ul>
Actor.damaged = function(self, args)
	-- Check for invulnerability.
	if not self:get_visible() then return end
	if self.god then return end
	local health = self.stats:get_value("health")
	if not health then return end
	local was_already_dead = self.dead
	-- Reduce health.
	if args.amount < 0 then
		local max = self.stats:get_maximum("health")
		self.stats:set_value("health", math.min(health - args.amount, max))
	elseif health - args.amount > 0 then
		self.stats:set_value("health", health - args.amount)
	else
		-- Behead if the blow was strong.
		-- Players can't be beheaded with first hit.
		if not self:get_beheaded() then
			if self.dead then
				if args.amount > 15 then self:set_beheaded(true) end
			elseif not self.client then
				if args.amount > 30 then self:set_beheaded(true) end
			end
		end
		-- Say the death message.
		-- This is omitted if the actor is beheaded or killed by a surprise.
		if not self.dead and not self:get_beheaded() and self.ai then
			for k,v in pairs(self.ai.enemies) do
				local p = self.spec:get_personality()
				local s = p and p:get_phrase("death")
				if s then self:action("say", s) end
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
	if args.amount > 0 then
		local ename
		if was_already_dead then
			ename = "damage dead"
		elseif self.dead then
			ename = "damage death"
		else
			ename = "damage"
		end
		local effect = self.spec:get_effect(ename)
		if effect then
			if args.point then
				Main.vision:world_effect(args.point, effect.name)
			else
				Main.vision:object_effect(self, effect.name)
			end
		end
	end
	-- Play the flinch animation.
	if args.type == "physical" and args.amount > 0 then
		if not self.blocking then
			self:animate("flinch")
		end
	end
end

--- Causes the actor to die.
-- @param self Actor.
-- @return True if killed, false if saved by Sanctuary.
Actor.die = function(self)
	if self.dead then return end
	-- Sanctuary save.
	if self.modifiers and self.modifiers.sanctuary and not self:get_beheaded() then
		self:remove_modifier("sanctuary")
		self:damaged{amount = -10, type = "physical"}
		self:teleport{marker = "sanctuary"}
		return
	end
	-- Death dialog.
	if Main.dialogs then
		Main.dialogs:cancel(self)
		if self.spec.dialog then
			Main.dialogs:execute(self, self, "death")
			Main.dialogs:cancel(self)
		end
	end
	-- Disable controls etc.
	self:set_dead_state(true)
	-- Unsummon summoned actors.
	if self.summon_timer then
		self:unsummon()
	end
	-- Spawn death drop items.
	for k,v in pairs(self.spec.inventory_items_death) do
		local o = self.manager:create_object_by_spec("Item", k)
		if o then
			o:set_count(v)
			self.inventory:merge_or_drop_object(o)
		end
	end
	return true
end

--- Makes the actor face a point.
-- @param self Actor.
-- @param args Arguments.<ul>
--   <li>point: Position vector.</li>
--   <li>secs: Tick length or nil for instant rotation.</li></ul>
-- @return The dot product of the current direction and the target direction.
Actor.face_point = function(self, args)
	local sdir = __vec1:set_xyz(0, 0, -1):transform(self:get_rotation())
	local edir = __vec2:set(args.point):subtract(self:get_position()):normalize()
	local quat = __quat1:set_dir(__vec3:set_xyz(edir.x, 0, edir.z), __vec4:set_xyz(0, 1, 0))
	if args.secs then
		-- Interpolate rotation towards target point.
		-- TODO: Should use args.secs here somehow.
		self:set_rotation(__quat2:set(self:get_rotation()):nlerp(quat, 0.9))
		return sdir:dot(edir)
	else
		-- Instantly set the target rotation.
		self:set_rotation(quat)
		return 1.0
	end
end

--- Called when the inventory of the actor is updated.
-- @param self Actor.
-- @param args Inventory event arguments.
Actor.handle_inventory_event = function(self, args)
	if not args then return end
	if not self:get_spec() then return end
	if not self:has_client_data() then return end
	local node = self.spec:get_node_by_equipment_slot(args.slot)
	-- Update achored equipment or the actor model.
	if args.inventory.id == self.inventory.id then
		if args.type == "inventory-equipped" then
			self.render:handle_inventory_equip(args.slot, args.object)
			local effect = args.object.spec:get_effect("equip")
			if effect then
				if not Operators.play:is_startup_period() then
					Main.effect_manager:play_object(effect.name, self, node)
				end
			end
		elseif args.type == "inventory-unequipped" then
			self.render:handle_inventory_unequip(args.slot, args.object)
			local effect = args.object.spec:get_effect("unequip")
			if effect then
				if not Operators.play:is_startup_period() then
					Main.effect_manager:play_object(effect.name, self, node)
				end
			end
		end
	end
end

--- Picks up an object.
-- @param self Actor.
-- @param src_id ID of the picked up object.
-- @param dst_id ID of the inventory where to place the object.
-- @param dst_slot Name of the inventory slot where to place the object.
Actor.pick_up = function(self, src_id, dst_id, dst_slot)
	self:animate("pick up")
	Timer{delay = self.spec.timing_pickup * 0.02, func = function(timer)
		Actions:move_from_world_to_inv(self, src_id, dst_id, dst_slot)
		timer:disable()
	end}
end

--- Randomizes the actor.
-- @param self Actor.
Actor.randomize = function(self)
	local spec = self:get_spec()
	-- Set the appearance.
	if spec.preset then
		local preset = Main.specs:find_by_name("ActorPresetSpec", spec.preset)
		if preset then
			for k,v in pairs(preset:get_actor()) do
				self[k] = v
			end
		end
	end
	self.brow_style = self.brow_style or spec:get_random_texture_style("brow")
	self.eye_color = self.eye_color or spec:get_random_eye_color()
	self.eye_style = self.eye_style or spec:get_random_texture_style("eye")
	self.face_style = self.face_style or spec:get_random_texture_style("face")
	self.head_style = self.head_style or spec:get_random_head()
	self.hair_color = self.hair_color or spec:get_random_hair_color() 
	self.hair_style = self.hair_style or spec:get_random_hair_style() 
	self.mouth_style = self.mouth_style or spec:get_random_texture_style("mouth")
	-- Populate the stats.
	self.stats:set_value("health", self.stats:get_maximum("health"))
	self.stats:set_value("willpower", self.stats:get_maximum("willpower"))
	-- Populate the inventory.
	for k,v in pairs(spec.inventory_items) do
		local itemspec = Main.specs:find_by_name("ItemSpec", k)
		if itemspec then
			if itemspec.stacking then
				local item = self.manager:create_object("Item")
				item:set_spec(itemspec)
				item:set_count(v)
				self.inventory:merge_object(item)
			else
				for i = 1,v do
					local item = self.manager:create_object("Item")
					item:set_spec(itemspec)
					self.inventory:merge_object(item)
				end
			end
		end
	end
	self.inventory:equip_best_objects()
	-- Create random loot.
	if spec.loot_categories then
		local num_cat = #spec.loot_categories
		local num_item
		if spec.loot_count_min or spec.loot_count_max then
			local min = spec.loot_count_min or 0
			local max = spec.loot_count_max or min
			num_item = math.random(min, max)
		else
			num_item = math.random(0, self.inventory.size)
		end
		for i = 1,num_item do
			local cat = spec.loot_categories[math.random(1, num_cat)]
			local itemspec = Main.specs:find_random_by_category("ItemSpec", cat)
			if itemspec then
				local item = self.manager:create_object("Item")
				item:set_spec(itemspec)
				self.inventory:merge_object(item)
			end
		end
	end
end

--- Removes a modifier.
-- @param self Actor.
-- @param name Modifier name.
Actor.remove_modifier = function(self, name)
	if self.modifiers and self.modifiers[name] then
		self.modifiers[name] = nil
		self:removed_modifier(name)
	end
end

--- Called when a modifier is removed.
-- @param self Actor.
-- @param name Modifier name.
Actor.removed_modifier = function(self, name)
	self:update_skills()
end

--- Teleports the object.
-- @param self Actor.
-- @param args Arguments.<ul>
--   <li>marker: Map marker name.</li>
--   <li>position: World position.</li>
--   <li>region: Region name.</li></ul>
-- @return True on success.
Actor.teleport = function(self, args)
	if SimulationObject.teleport(self, args) then
		-- Update skills.
		-- As long as actors always teleport between the overworld and the
		-- underworld, this ensures that view distance is update correctly.
		self:update_skills()
		return true
	end
end

--- Causes the summoned actor to be unsummoned.
-- @param self Actor.
Actor.unsummon = function(self)
	-- TODO: Some visual feedback would be nice.
	self:detach()
end

--- Updates the state of the actor.
-- @param self Actor.
-- @param secs Seconds since the last update.
Actor.update_client = function(self, secs)
	self.render:update(secs)
end

--- Updates the state of the actor.
-- @param self Actor.
-- @param secs Seconds since the last update.
Actor.update_server = function(self, secs)
	self.update_timer = self.update_timer + secs
	if self.update_timer > 0.3 then
		local tick = self.update_timer
		self:update_modifiers(tick)
		self.stats:update(tick)
		self:update_actions(tick)
		self:update_burdening(tick)
		self:update_summon(tick)
		self.update_timer = 0
	end
end

Actor.update_actions = function(self, secs)
	-- Update flying.
	if self.flying and self.tilt then
		local v = self:get_velocity()
		if math.abs(self:get_movement()) > 0.5 then
			local e = self.tilt.euler[3]
			if e > 0 then
				self:set_velocity(__vec1:set_xyz(v.x, math.max(v.y, 5*math.sin(e)), v.z))
			else
				self:set_velocity(__vec1:set_xyz(v.x, math.min(v.y, 5*math.sin(e)), v.z))
			end
		else
			self:set_velocity(__vec1:set_xyz(0, 0, 0))
		end
	end
	-- Update feat cooldown.
	if self.cooldown then
		self.cooldown = self.cooldown - secs
		if self.cooldown <= 0 then
			self.cooldown = nil
		end
	end
	-- Check for falling damage.
	-- Don't update every frame.
	self.fall_timer = (self.fall_timer or 0) + secs
	if self.fall_timer > 0.3 then
		if self.velocity_prev then
			local limity = self.spec.falling_damage_speed
			local prevy = self.velocity_prev
			local diffy = self:get_velocity().y - prevy
			if prevy < -limity and diffy > limity then
				local rate = self.spec.falling_damage_rate
				local damage = (diffy - limity) * rate * self.attributes.falling_damage
				if damage > 2 then
					self:damaged{amount = damage, type = "falling"}
					local effect = self.spec:get_effect("falling damage")
					if effect then
						Main.vision:object_effect(self, effect.name)
					end
				end
			end
		end
		self.velocity_prev = self:get_velocity().y
	end
end

--- Updates the AI state of the actor.<br/>
--
-- You can force the AI to recheck its state immediately by calling the function.
-- This can be useful when, for example, the player has initiated a dialog and
-- you want to set the NPC into the chat mode immediately instead of waiting for
-- the next AI update cycle.
--
-- @param self Actor.
Actor.update_ai_state = function(self)
	if self.ai then
		self.ai:update_state()
	end
end

Actor.update_burdening = function(self, secs)
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
		Main.messaging:server_event("inventory weight", self.client, curr_weight, curr_limit)
	end
	local curr_burden = (self.carried_weight > self:get_burden_limit())
	if curr_burden and not prev_burden then
		self:add_modifier("burdening")
	end
	-- Update speed.
	-- Skill regeneration affects speed too so this needs to be recalculated
	-- every now and then regardless of burdening.
	self:calculate_speed()
end

Actor.update_modifiers = function(self, secs)
	if not self.modifiers then return end
	-- Remove all modifiers if dead.
	if self.dead then
		for k,v in pairs(self.modifiers) do
			self.modifiers[k] = nil
			self:removed_modifier(k)
		end
		self.modifiers = nil
		return
	end
	-- Update each modifier.
	local remain
	for k,v in pairs(self.modifiers) do
		if not v:update(secs) then
			self.modifiers[k] = nil
			self:removed_modifier(k)
		else
			remain = true
		end
	end
	-- Remove empty modifier lists.
	if not remain then
		self.modifiers = nil
	end
end

--- Updates the skills and related attributes of the actor.
-- @param self Actor.
Actor.update_skills = function(self)
	-- Calculate the attributes.
	local attr = self.skills:calculate_attributes()
	if self.modifiers then
		for k,v in pairs(self.modifiers) do
			v:get_attributes(attr)
		end
	end
	attr.max_health = math.max(1, attr.max_health)
	-- Assign the attributes to the actor.
	self.attributes = attr
	self.stats:set_maximum("health", attr.max_health)
	self.stats:set_maximum("willpower", attr.max_willpower)
	-- Update the movement speed.
	self:calculate_speed()
	-- Update the vision radius.
	if self.vision then
		if self:get_position().y > 1600 then
			attr.view_distance = attr.view_distance * 1.5
		end
		self.vision:set_radius(attr.view_distance)
	end
end

--- Updates the summon status of the object.
-- @param self Actor.
-- @param secs Seconds since the last update.
Actor.update_summon = function(self, secs)
	if not self.summon_timer then return end
	self.summon_timer = self.summon_timer - secs
	if self.summon_timer < 0 then
		self:unsummon()
	end
end

Actor.get_attack_charge = function(self)
	if not self.attack_charge then return 0 end
	return math.min(1, (Time:get_secs() - self.attack_charge) / 2)
end

--- Returns true if the actor is behaded.
-- @param self Actor.
-- @return Boolean.
Actor.get_beheaded = function(self)
	return self.__beheaded
end

Actor.set_beheaded = function(self, value)
	if self.beheaded == value then return end
	if value then
		-- Check is allowed.
		if not self.spec.enable_decapitation then return end
		-- Behead.
		self.__beheaded = true
		Main.vision:object_event(self, "object-beheaded")
		-- Drop headwear.
		local hat = self.inventory:get_object_by_slot("head")
		if hat then
			local p = self:get_position()
			hat:detach()
			hat:set_position(p + self:get_rotation() * (self.dead and Vector(0,0.5,2) or Vector(0,2,0)))
			hat:set_velocity(Vector(math.random(), math.random(), math.random()))
			hat:set_visible(true)
		end
	else
		-- Unbehead.
		self.__beheaded = nil
		Main.vision:object_event(self, "object-beheaded")
	end
end

--- Enables or disables the blocking stance.
-- @param self Actor.
-- @param value True to block.
Actor.set_block = function(self, value)
	if self.jumping then return end
	if value and self.blocking then return end
	if not value and not self.blocking then return end
	if value then
		self.blocking = Time:get_secs()
		self:animate("block start")
	else
		self.blocking = nil
		self:animate("block stop")
	end
	self:calculate_speed()
	self:calculate_animation()
end

--- Returns the burdening limit of the actor.
-- @param self Actor.
-- @return Burdening limit in kilograms
Actor.get_burden_limit = function(self)
	-- TODO
	return 500
end

--- Returns true if the actor is burdened.
-- @param self Actor.
-- @return True if burdened. False otherwise.
Actor.get_burdened = function(self)
	if self:get_modifier("burdening") then return true end
	return false
end

--- Checks whether the actor is participating in combat.
-- @param self Actor.
-- @return True if in combat. False otherwise.
Actor.get_combat_active = function(self)
	if not self.ai then return end
	for k,v in pairs(self.ai.enemies) do
		return true
	end
end

--- Gets the hint on whether the actor is hostile at someone.
-- @param self Actor.
-- @return True if set. False otherwise.
Actor.get_combat_hint = function(self)
	if self.dead then return false end
	return self.__combat_hint or false
end

--- Sets the hint on whether the actor is hostile at someone.
-- @param self Actor.
-- @param Value True to set the hint. Nil otherwise.
Actor.set_combat_hint = function(self, value)
	if self.__combat_hint == value then return end
	self.__combat_hint = value
	Main.vision:object_event(self, "object-combat")
end

Actor.get_dead = function(self)
	return self.dead
end

Actor.set_dead = function(self, v)
	if v then
		if self.dead then return end
		-- Disable controls.
		self:set_movement(0)
		self:set_strafing(0)
		self.auto_attack = nil
		self.jumping = nil
		self.climbing = nil
		self.physics:set_shape("dead")
		self.physics:set_physics("rigid")
		-- Playback animation.
		-- NOTE: Animations are discarded after setting the 'dead' member.
		self:animate("death")
		self.dead = true
		-- Disable stats.
		self.stats.enabled = false
		self.stats:set_value("health", 0)
		-- Inform the AI.
		if self.ai then
			self.ai:set_dead(true)
		end
		-- Emit a vision event.
		Main.vision:object_event(self, "object-dead", {dead = true})
	else
		if not self.dead then return end
		-- Restore the idle animation.
		self.dead = nil
		self:animate("idle")
		-- Enable controls.
		self:set_beheaded(false)
		self.physics:set_shape("default")
		self.physics:set_physics("kinematic")
		-- Enable stats.
		self.stats.enabled = true
		self.stats:set_value("health", 1)
		-- Inform the AI.
		if self.ai then
			self.ai:set_dead(false)
		end
		-- Emit a vision event.
		Main.vision:object_event(self, "object-dead", {dead = false})
	end
end

Actor.set_dead_state = function(self, drop)
	-- Drop held items.
	if drop then
		local o = self.inventory:get_object_by_slot("hand.L")
		if o then
			o:detach()
			o:set_position(self:get_position())
			o:set_visible(true)
		end
		o = self:get_weapon()
		if o then
			o:detach()
			o:set_position(self:get_position())
			o:set_visible(true)
		end
	end
	-- Set the death mode.
	self:set_dead(true)
end

--- Gets the importance of the object.
-- @param self Object.
-- @return True if important. False otherwise.
Actor.get_important = function(self)
	if self.__important ~= nil and not self.dead then return self.__important end
	if self.spec.important then return true end
end

--- Gets a modifier by name.
-- @param self Actor.
-- @param name Modifer name.
Actor.get_modifier = function(self, name)
	if not self.modifiers then return end
	return self.modifiers[name]
end

--- Sets the forward/backward movement state of the actor.
-- @param self Actor.
-- @param value Movement rate.
Actor.set_movement = function(self, value)
	SimulationObject.set_movement(self, math.min(1, math.max(-1, value)))
	self:calculate_animation()
end

--- Gets the preferred physics mode of the object.
-- @param self Object.
-- @return String.
Actor.get_physics_mode = function(self)
	return self.dead and "rigid" or "kinematic"
end

--- Gets the rotation quaternion towards the given point.
-- @param self Actor.
-- @param point Position vector in world space.
-- @return Quaternion.
Actor.get_rotation_to_point = function(self, point)
	local dir = __vec1:set(point):subtract(self:get_position()):normalize()
	return Quaternion:new_from_dir(dir.x, 0, dir.z, 0, 1, 0)
end

--- Gets the running state of the actor.
-- @param self Actor.
-- @return True if running is enabled. False otherwise.
Actor.get_running = function(self)
	return self.running
end

--- Sets the running state of the actor.
-- @param self Actor.
-- @param v True to enable running. False otherwise.
Actor.set_running = function(self, v)
	if self.running == v then return end
	self.running = v
	self:calculate_speed()
end

--- Sets the actor spec of the object.
-- @param self Actor.
-- @param v ActorSpec.
Actor.set_spec = function(self, v)
	local spec = type(v) == "string" and Main.specs:find_by_name("ActorSpec", v) or v
	if self.spec == spec then return end
	SimulationObject.set_spec(self, spec)
	-- Configure physics.
	self.physics:set_collision_group(spec.collision_group)
	self.physics:set_collision_mask(spec.collision_mask)
	self.physics:set_friction_liquid(spec.water_friction)
	self.physics:set_gravity(spec.gravity)
	self.physics:set_gravity_liquid(spec.water_gravity)
	self.physics:set_mass(spec.mass)
	-- Initialize stats and skills.
	self.skills:clear()
	for k in pairs(spec.skills) do
		self.skills:add(k)
	end
	self:update_skills()
	-- Set the inventory size.
	self.inventory:set_size(spec.inventory_size)
	-- Create server data.
	if self:has_server_data() then
		-- Create the map marker.
		if spec.marker then
			self.marker = Main.markers:find_by_name(spec.marker)
			if self.marker then
				self.marker.position = self:get_position()
				self.marker.target = self:get_id()
			else
				self.marker = Main.markers:create(spec.marker, self:get_id(), self:get_position())
			end
		end
		-- Mark corpses as dead.
		if spec.dead then
			self.dead = true
		end
		-- Create the AI.
		if not spec.dead and spec.ai_enabled then
			ai_class = Main.ai:find_class_by_type(spec.ai_type or "npc")
			self.ai = ai_class and ai_class(Main.ai, self)
		end
	end
	-- Set the model.
	self:set_model_name(spec.model)
	self.animated = true
	-- FIXME: Client data.
	if self:has_client_data() then
		self.inventory:subscribe(self, function(args) self:handle_inventory_event(args) end)
	end
end

Actor.set_stat = function(self, s, v, m, diff)
	assert(self:has_client_data())
	-- Update the stat.
	if not self:has_server_data() then
		self.stats:set(s, v, m, 0)
	end
	-- Show health notifications.
	if diff and s == "health" then
		-- Show a health change text.
		Main.effect_manager:create_damage_text(self, nil, diff)
		-- Quake the camera if the player was hurt.
		if self == Main.client.player_object and diff < -2 then
			local amount = math.min(1, 0.01 * (2 - diff))
			Main.effect_manager:apply_quake(self:get_position(), amount)
		end
		-- Set the correct collision shape.
		-- Dead actors have a different collision shape. We switch between
		-- the two when the health changes between zero and non-zero.
		if v == 0 and self.animated then
			self.physics:set_shape("dead")
		else
			self.physics:set_shape("default")
		end
	end
end

Actor.get_storage_type = function(self)
	return "actor"
end

--- Sets the strafing state of the actor.
-- @param self Actor.
-- @param value Strafing rate.
Actor.set_strafing = function(self, value)
	SimulationObject.set_strafing(self, value)
	self:calculate_animation()
end

--- Sets the tilt angle of the object.
-- @param self Actor.
-- @param tilt Angle in radians.
-- @param predict True to use prediction.
Actor.set_tilt_angle = function(self, tilt, predict)
	-- Set the angle.
	SimulationObject.set_tilt_angle(self, tilt, predict)
	-- Back tilting.
	if self:has_client_data() and (not self.predict or not predict) and self.spec then
		local t = self.dead and 0 or tilt or 0
		t = t * self.spec.tilt_mult
		self.render:set_tilt(self.spec.tilt_bone, t)
	end
end

--- Sets the velocity of the object.
-- @param self Actor.
-- @param value Velocity vector.
-- @param predict True to use prediction
Actor.set_velocity = function(self, value, predict)
	self.fall_timer = nil
	self.velocity_prev = nil
	SimulationObject.set_velocity(self, value, predict)
end

--- Gets the currently wielded weapon.
-- @param self Actor.
-- @return Item, or nil.
Actor.get_weapon = function(self)
	return self.inventory:get_object_by_slot(self.spec.weapon_slot)
end

--- Sets the currently wielded weapon.
-- @param self Actor.
-- @param value Item, or nil.
-- @return True on success.
Actor.set_weapon = function(self, value)
	return self.inventory:equip_object(value, self.spec.weapon_slot)
end

return Actor
