require "client/objects/speedline"

Object.physics_position_correction = Vector(0, 0, 0)

Object.animate_spec = function(self, name)
	-- Find the animation from the spec.
	if not self.spec then return end
	if not self.spec.animations then return end
	local a = self.spec.animations[name]
	if not a then return end
	-- Play the animation.
	self:animate{
		animation = a.animation,
		channel = a.channel,
		fade_in = a.fade_in or 0.5,
		fade_out = a.fade_out or 0.5,
		permanent = a.permanent,
		repeat_start = a.repeat_start,
		time_scale = a.time_scale,
		weight = a.weight or 2}
	return a
end

Object.detach = function(self)
	-- Hide special effects.
	if self.special_effects then
		for k,v in pairs(self.special_effects) do
			if v.enabled then
				v.enabled = false
			else
				v.realized = false
			end
		end
		self.special_effects = nil
	end
	-- Hide speed lines.
	if self.speedline then
		self.speedline.realized = false
		self.speedline = nil
	end
	-- Hide equipment.
	if self.slots then
		for k,v in pairs(self.slots.slots) do
			v:detach()
		end
		self.slots = nil
	end
	-- Hide self.
	self.realized = false
end

Object.set_anim = function(self, name, time)
	local args = {animation = name, fade_in = 0.3, fade_out = 0.3, time = t}
	local anim = self.spec and self.spec.animations[name]
	if anim then
		for k,v in pairs(anim) do args[k] = v end
	end
	self:animate(args)
end

Object.set_dialog = function(self, type, args)
	-- Update the dialog state.
	if type == "choice" then
		self.dialog = {type = type, choices = args}
	elseif type == "message" then
		self.dialog = {type = type, message = args}
	else
		self.dialog = nil
	end
	-- Update the dialog UI.
	if Gui.active_dialog == self.id then
		Gui:set_dialog(self.id)
	end
end

Object.set_skill = function(self, s, v, m)
	-- Update player skills.
	if self == Player.object then
		Client.views.skills:update(s, v, m)
	end
	-- Display health changes.
	if s == "health" then
		if self.health then
			-- Show a health change text.
			local diff = v - self.health
			if math.abs(diff) > 2 then
				local code = (diff > 0 and 0x01 or 0x00) + (self == Player.object and 0x10 or 0x00)
				local colors = {
					[0x00] = {1,1,0,1},
					[0x01] = {0,1,1,1},
					[0x10] = {1,0,0,1},
					[0x11] = {0,1,0,1}}
				EffectObject{
					life = 3,
					object = self,
					position = Vector(0,2,0),
					realized = true,
					text = tostring(diff),
					text_color = colors[code],
					text_fade_time = 1,
					text_font = "medium",
					velocity = Vector(0,0.5,0)}
			end
			-- Quake the camera if the player was hurt.
			if self == Player.object and diff < -5 then
				Client:apply_quake(self.position, 0.01 * (5 - diff))
			end
		end
		self.health = v
		-- Set the correct collision shape.
		-- Dead creatures have a different collision shape. We switch between
		-- the two when the health changes between zero and non-zero.
		if self.health == 0 and self.animated then
			self.shape = "dead"
		else
			self.shape = "default"
		end
	end
end

Object.set_slot = function(self, slot, spec, count)
	if not self.slots then self.slots = Slots() end
	local slots = self.slots
	spec = Itemspec:find{name = spec}
	-- Update the model.
	if not spec then
		-- Missing spec.
		slots:set_object{slot = slot}
		if self.equipment and self.equipment[slot] then
			self.equipment[slot] = nil
			if self.realized then self:update_model() end
		end
	elseif spec.equipment_models then
		-- Replacer equipment.
		slots:set_object{slot = slot}
		self.equipment = self.equipment or {}
		self.equipment[slot] = spec.name
		if self.realized then self:update_model() end
	else
		-- Add-on equipment.
		slots:set_object{slot = slot, model = spec.model, spec = spec}
		self.equipment = self.equipment or {}
		self.equipment[slot] = spec.name
		if self.realized then self:update_model() end
	end
	-- Equip animations.
	local a
	if spec then
		a = self:animate_spec(spec.animation_hold)
	end
	if self.equipment_animations then
		if not a and self.equipment_animations[slot] then
			self:animate{channel = self.equipment_animations[slot].channel}
		end
		self.equipment_animations[slot] = a
	else
		self.equipment_animations = {[slot] = a}
	end
end

Object.update = function(self, secs)
	-- Update slots.
	if self.slots then
		local species = self.spec
		for name,object in pairs(self.slots.slots) do
			local slot = species and species.equipment_slots[name]
			if slot and slot.node and self.realized then
				-- Show slot.
				local p,r = self:find_node{name = slot.node, space = "world"}
				if p then
					local h = object:find_node{name = "#handle"}
					if h then p = p - r * h end
					object.position = p
					object.rotation = r
					object.realized = true
					object:update(secs)
				else
					object:detach()
				end
			else
				-- Hide slot.
				object:detach()
			end
		end
	end
	-- Update special effects.
	if self.realized then
		if self.special_effects then
			for k,v in pairs(self.special_effects) do
				local p = self.position
				local r = self.rotation
				if v.node then
					local np,nr = self:find_node{name = v.node}
					if np then
						p = p + r * np
						r = r * nr
					end
				end
				if v.offset then p = p + v.offset end
				if v.rotate then v.rotation = r end
				v.position = p
			end
		end
	end
	-- Update speed lines.
	if self.speedline then
		self.speedline:update(secs)
	end
end

Object.replace_model = function(self, model)
	-- Apply the new model.
	local shape = self.shape
	self.model = model
	self.shape = shape
	-- Initialize the pose.
	if self.spec and self.spec.type == "species" then
		self.animated = true
		self:update_animations{secs = 0}
		if Program.opengl_version < 3.2 and model.name then
			local m = self.model:copy()
			m:changed()
			self.model = m
		end
	end
	-- Apply body scale.
	if self.spec and self.spec.body_scale then
		local factor = self.body_scale or 0.5
		local scale = self.spec.body_scale[1] * (1 - factor) + self.spec.body_scale[2] * factor
		self:animate{animation = "empty", channel = Animation.CHANNEL_CUSTOMIZE,
			weight = 0, weight_scale = 1000, fade_in = 0, fade_out = 0, permanent = true}
		self:edit_pose{channel = Animation.CHANNEL_CUSTOMIZE, node = "mover", scale = scale}
	end
	-- Create the tilting channel.
	if self.spec and (self.spec.models or self.spec.tilt_bone) then
		self:animate{animation = "empty", channel = Animation.CHANNEL_TILT,
			additive = true, weight = 1, permanent = true}
	end
	-- Detach old special effects.
	if self.special_effects then
		for k,v in pairs(self.special_effects) do v:detach() end
		self.special_effects = nil
	end
	-- Create new special effects.
	if self.spec and self.spec.special_effects and #self.spec.special_effects then
		self.special_effects = {}
		for k,v in pairs(self.spec.special_effects) do
			if v.type == "light" then
				local fx = Light{ambient = v.ambient, diffuse = v.diffuse, node = v.node,
					equation = v.equation, position = self.position, enabled = true}
				table.insert(self.special_effects, fx)
			else
				local fx = Object{model = v.model, node = v.node, position = self.position, realized = true}
				table.insert(self.special_effects, fx)
			end
		end
	else
		self.special_effects = nil
	end
end

Object.update_model = function(self)
	if not Model.morph then return end
	if self.spec and self.spec.models then
		-- Build the character model in a separate thread.
		-- The result is handled in the tick handler in event.lua.
		Client.threads.model_builder:push_message(tostring(self.id), serialize{
			beheaded = Bitwise:bchk(self.flags or 0, Protocol.object_flags.BEHEADED),
			body_scale = self.body_scale,
			body_style = self.body_style,
			equipment = self.equipment,
			eye_color = self.eye_color,
			eye_style = self.eye_style,
			face_style = self.face_style,
			hair_color = self.hair_color,
			hair_style = self.hair_style,
			nudity = Client.views.options.nudity_enabled,
			species = self.spec.name,
			skin_color = self.skin_color,
			skin_style = self.skin_style})
	elseif self.spec and self.spec.model then
		-- Replace the model and the special effects.
		local model = Model:find_or_load{file = self.spec.model}
		self:replace_model(model)
	else
		-- Replace the special effects.
		self:replace_model(self.model)
	end
end

--- Plays footstep sounds for creatures.
-- @param self Object.
-- @param secs Seconds since the last update.
Object.update_sound = function(self, secs)
	-- Check for an applicable species.
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

--- Updates the rotation and tilt of the object.
-- @param self Object.
-- @param quat Rotation quaternion.
-- @param tilt Tilt angle in radians.
Object.update_rotation = function(self, quat, tilt)
	self.rotation = quat
	self.tilt = tilt
	local spec = self.spec
	if spec and spec.tilt_bone then
		local nodes = spec.tilt_bone
		if type(nodes) ~= "table" then nodes = {nodes} end
		local angle = self.dead and 0 or -tilt
		local rot = Quaternion{axis = Vector(1,0,0), angle = angle / #nodes}
		for k,v in pairs(nodes) do
			self:edit_pose{channel = Animation.CHANNEL_TILT, node = v, rotation = rot}
		end
	end
end

Object.update_motion_state = function(self, tick)
	if not self.interpolation then return end
	-- Damp velocity to reduce overshoots.
	self.interpolation = self.interpolation + tick
	if self.interpolation > 0.3 then
		self.velocity = self.velocity * 0.93
	end
	-- Apply position change predicted by the velocity.
	self.position = self.position + self.velocity * tick
	-- Correction prediction errors over time.
	self.position = self.position + self.correction * 0.07
	self.correction = self.correction * 0.93
end

--- Writes the appearance preset of the object to a string.
-- @param self Object.
-- @return String.
Object.write_preset = function(self)
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

Object.set_motion_state = function(self, pos, rot, vel, tilt)
	-- Store the prediction error so that it can be corrected over time.
	if (pos - self.position).length < 5 then
		self.correction = pos - self.position
	else
		self.position = pos
		self.correction = Vector()
	end
	-- Store the current velocity so that we can predict future movements.
	self.velocity = Vector(vx, vy, vz)
	self.interpolation = 0
	-- Set rotation unless controlled by the local player.
	if self ~= Player.object then
		self:update_rotation(rot, tilt)
	end
end
