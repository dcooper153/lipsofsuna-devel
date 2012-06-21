Actor = Class(Object)

Actor.new = function(clss, args)
	local self = Object.new(clss, args)
	self.stats = {}
	self.inventory:subscribe(self, function(args) self:update_inventory(args) end)
	Object.dict_active[self] = 1.0
	Client.options:apply_object(self)
	return self
end

Actor.detach = function(self)
	-- Hide equipment.
	self.inventory:clear()
	-- Call base.
	Object.detach(self)
end

--- Disables the equipment holding animation for the given slot.
-- @param self Actor.
-- @param slot Slot being modified.
Actor.disable_equipment_holding_animation = function(self, slot)
	if not self.equipment_animations then return end
	if not self.equipment_animations[slot] then return end
	self:animate_fade{channel = self.equipment_animations[slot].channel}
	self.equipment_animations[slot] = nil
end

--- Enables the equipment holding animation for the given slot.
-- @param self Actor.
-- @param slot Slot being modified.
-- @param item Item added to the slot.
Actor.enable_equipment_holding_animation = function(self, slot, item)
	-- Enable the animation if supported by the actor.
	local anim = self:animate_spec(item.spec.animation_hold)
	if anim then
		if self.equipment_animations then
			self.equipment_animations[slot] = anim
		else
			self.equipment_animations = {[slot] = anim}
		end
	else
		self:disable_equipment_holding_animation(slot)
	end
end

Actor.update_body_scale = function(self, value)
	if self.spec.body_scale_min or self.spec.body_scale_max then
		local min = self.spec.body_scale_min or 1
		local max = self.spec.body_scale_max or 1
		local factor = self.body_scale or 0.5
		local scale = min * (1 - factor) + max * factor
		local anim = Animation("scale")
		anim:set_transform{frame = 1, node = "mover", scale = scale}
		self:animate{channel = Animation.CHANNEL_CUSTOMIZE, animation = anim,
			fade_in = 0, fade_out = 0, permanent = true, replace = true, weight = 0, weight_scale = 1000}
	end
end

Actor.set_model = function(self)
	if self.spec.models then
		-- Create a custom model.
		Object.set_model(self, self.model)
		self:request_model_rebuild()
	else
		-- Use a static model.
		local m = Model:find_or_load{file = self.spec.model}
		if not m then return end
		self.model = m
	end
	-- Update body scale.
	self:update_body_scale()
	-- Update special effects
	Object.set_model(self, true)
	self.animated = true
end

Actor.set_stat = function(self, s, v, m)
	-- Find or create the skill.
	local stat = self.stats[s]
	if not stat then
		self.stats[s] = {cap = m, value = v}
		return
	end
	-- Update the stat.
	local diff = v - stat.value
	stat.value = v
	stat.cap = m
	-- Apply health effects.
	local player = Client.player_object
	if s == "health" then
		-- Show a health change text.
		if math.abs(diff) > 2 then
			local code = (diff > 0 and 0x01 or 0x00) + (self == player and 0x10 or 0x00)
			local colors = {
				[0x00] = {1,1,0,1},
				[0x01] = {0,1,1,1},
				[0x10] = {1,0,0,1},
				[0x11] = {0,1,0,1}}
			Client:add_damage_text{object = self, color = colors[code], text = tostring(diff)}
		end
		-- Quake the camera if the player was hurt.
		if self == player and diff < -5 then
			Client:apply_quake(self.position, 0.01 * (5 - diff))
		end
		-- Set the correct collision shape.
		-- Dead actors have a different collision shape. We switch between
		-- the two when the health changes between zero and non-zero.
		if v == 0 and self.animated then
			self.shape = "dead"
		else
			self.shape = "default"
		end
	end
end

--- Updates the positions of equipment tagged to the actor.
-- @param self Actor.
-- @param secs Seconds since the last update.
Actor.update = function(self, secs)
	-- Call the base class.
	Object.update(self, secs)
	-- Handle model rebuilding.
	--
	-- Equipment changes can occur frequently when newly appearing actors are
	-- being setup. Because of that, a delay is used to avoid too many expensive
	-- rebuilds.
	if self.model_rebuild_timer then
		self.model_rebuild_timer = self.model_rebuild_timer - secs
		if self.model_rebuild_timer <= 0 then
			self.model_rebuild_timer = nil
			self:update_model()
		end
	end
	-- Apply built models.
	--
	-- Models are built asynchronously to avoid stuttering when multiple models
	-- are built at the same time. As building is completed, the merged model
	-- can be popped from the model merger of the object.
	if self.model_merger then
		local m = self.model_merger:pop_model()
		if m then
			m:changed()
			self.model = m
		end
	end
	-- Get the actor spec.
	if not self.realized then return end
	local spec = self.spec
	if not spec then return end
	-- Update objects in equipment slots.
	for slot,index in pairs(self.inventory.equipped) do
		local node = spec.equipment_slots[slot]
		local object = self.inventory:get_object_by_index(index)
		if node ~= "" and object then
			local p,r = self:find_node{name = node, space = "world"}
			if not p then p,r = self.position,self.rotation end
			if object.spec.equipment_anchor then
				local h,s = object:find_node{name = object.spec.equipment_anchor}
				if h then
					h = s.conjugate * h
					p = p - r * h
					r = r * s.conjugate
				end
			end
			object.position = p
			object.rotation = r
			object.realized = true
			object:update(secs)
		end
	end
end

--- Called when the inventory of the actor is updated.
-- @param self Actor.
-- @param args Inventory event arguments.
Actor.update_inventory = function(self, args)
	-- Get slot information from the actor spec.
	if not self.spec then return end
	local node = self.spec.equipment_slots[args.slot]
	if not node then return end
	-- Update achored equipment or the actor model.
	if args.type == "inventory-equipped" then
		if node ~= "" then
			-- Enable the anchored item.
			args.object.collision_group = 0
			args.object.collision_mask = 0
			args.object.realized = true
			args.object:set_model()
			self:enable_equipment_holding_animation(args.slot, args.object)
		else
			-- Require a model rebuild.
			self:request_model_rebuild()
		end
		if args.object.spec.effect_equip then
			if not Operators.play:is_startup_period() then
				Effect:play_object(args.object.spec.effect_equip, self, node)
			end
		end
	elseif args.type == "inventory-unequipped" then
		if node ~= "" then
			-- Disable the anchored item.
			args.object:detach()
			self:disable_equipment_holding_animation(args.slot)
		else
			-- Require a model rebuild.
			self:request_model_rebuild()
		end
		if args.object.spec.effect_equip then
			if not Operators.play:is_startup_period() then
				Effect:play_object(args.object.spec.effect_unequip, self, node)
			end
		end
	end
end

--- Queues a model rebuild for the actor.
-- @param self Actor.
Actor.request_model_rebuild = function(self)
	self.model_rebuild_timer = 0.1
end

Actor.update_model = function(self)
	if not self.spec then return end
	if not self.spec.models then return end
	-- Create the equipment list.
	equipment = {}
	for k in pairs(self.spec.equipment_slots) do
		local object = self.inventory:get_object_by_slot(k)
		if object then equipment[k] = object.spec.name end
	end
	-- Build the character model in a separate thread.
	-- The result is handled in the tick handler in event.lua.
	local m = ModelBuilder:build(self, {
		actor = self.spec.name,
		beheaded = Bitwise:bchk(self.flags or 0, Protocol.object_flags.BEHEADED),
		body_scale = self.body_scale,
		body_style = self.body_style,
		equipment = equipment,
		eye_color = self.eye_color,
		eye_style = self.eye_style,
		face_style = self.face_style,
		hair_color = self.hair_color,
		hair_style = self.hair_style,
		head_style = self.head_style,
		nudity = Client.options.nudity_enabled,
		skin_color = self.skin_color,
		skin_style = self.skin_style,
		spec = self.spec})
end

--- Updates the rotation and tilt of the object.
-- @param self Object.
-- @param quat Rotation quaternion.
-- @param tilt Tilt angle in radians.
Actor.update_rotation = function(self, quat, tilt)
	Object.update_rotation(self, quat, tilt)
	local spec = self.spec
	if spec and spec.tilt_bone then
		-- Calculate the tilting rotation.
		local nodes = spec.tilt_bone
		local angle = self.dead and 0 or -tilt
		local rot = Quaternion{axis = Vector(1,0,0), angle = angle / #nodes}
		-- Create the tilting channel.
		local anim = Animation("tilt")
		for k,v in pairs(nodes) do
			anim:set_transform{frame = 1, node = v, rotation = rot}
		end
		self:animate{animation = anim, channel = Animation.CHANNEL_TILT,
			additive = true, fade_in = 0, fade_out = 0, permanent = true, replace = true, weight = 1}
	end
end

--- Plays footstep sounds for actors.
-- @param self Object.
-- @param secs Seconds since the last update.
Actor.update_sound = function(self, secs)
	-- Check for an applicable actor.
	local spec = self.spec
	if not spec or not spec.footstep_sound then return end
	-- Check for an applicable animation.
	local anim = self:get_animation{channel = 1}
	if not anim then return end
	if anim.animation == "dead" or anim.animation == "idle" then return end
	-- Find the foot anchors.
	-- These are needed for foot position tracking so that we know when and
	-- where to play the positional sound.
	local lnode = self:find_node{name = "#foot.L", space = "world"}
	if not lnode then return end
	local rnode = self:find_node{name = "#foot.R", space = "world"}
	if not rnode then return end
	-- Ground check.
	-- We don't want to play footsteps if the character is flying.
	if not Physics:cast_ray{src = lnode, dst = lnode - Vector(0,spec.footstep_height)} and
	   not Physics:cast_ray{src = rnode, dst = rnode - Vector(0,spec.footstep_height)} then
		self.lfoot_prev = nil
		self.rfoot_prev = nil
		return
	end
	-- Left foot.
	-- We play the sound when the node crosses from the local positive Z axis
	-- to the negative. Using the Y distance to the ground is too error prone
	-- so this approximation is the best approach I have found so far.
	self.lfoot_prev = self.lfoot_curr
	self.lfoot_curr = self:find_node{name = "#foot.L"}
	if self.lfoot_prev and self.lfoot_curr.z < 0 and self.lfoot_prev.z >= 0 then
		Effect:play_world(spec.footstep_sound, lnode)
	end
	-- Right foot.
	-- Works the same way with the left foot.
	self.rfoot_prev = self.rfoot_curr
	self.rfoot_curr = self:find_node{name = "#foot.R"}
	if self.rfoot_prev and self.rfoot_curr.z < 0 and self.rfoot_prev.z >= 0 then
		Effect:play_world(spec.footstep_sound, rnode)
	end
	return true
end

--- Writes the appearance preset of the object to a string.
-- @param self Object.
-- @return String.
Actor.write_preset = function(self)
	return serialize{
		body_scale = self.body_scale,
		body_style = self.body_style,
		eye_color = self.eye_color,
		eye_style = self.eye_style,
		face_style = self.face_style,
		hair_color = self.hair_color,
		hair_style = self.hair_style,
		skin_color = self.skin_color,
		skin_style = self.skin_style}
end
