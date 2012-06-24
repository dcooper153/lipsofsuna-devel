require "client/movement-prediction"
require "client/objects/speedline"

Object.dict_active = setmetatable({}, {__mode = "k"})
Object.physics_position_correction = Vector(0, 0, 0)

local oldinit = Object.new
Object.new = function(clss, args)
	local self = oldinit(clss, args)
	self.inventory = Inventory{id = self.id}
	self.prediction = MovementPrediction()
	return self
end

Object.set_active_time = function(self, duration)
	Object.dict_active[self] = duration
end

Object.add_speedline = function(self, args)
	-- Stop the old speedline
	if self.speedline then
		self.speedline.object = nil
		self.speedline = nil
	end
	-- Create the new speedline
	if not args then args = {} end
	args.object = self
	self.speedline = Speedline(args)
end

Object.animate_spec = function(self, name)
	return self:set_anim(name)
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
	-- Stop the speed line.
	if self.speedline then
		self.speedline.object = nil
	end
	-- Hide self.
	self.realized = false
	Object.dict_active[self] = nil
end

Object.set_anim = function(self, name, time)
	-- Play the animation.
	local args = {animation = name, fade_in = 0.3, fade_out = 0.3, time = time}
	local anim = self.spec and self.spec.get_animation and self.spec:get_animation(name)
	if anim then
		for k,v in pairs(anim:get_arguments()) do args[k] = v end
	end
	self:animate(args)
	-- Mark as active.
	self.animated = true
	Object.dict_active[self] = 5.0
	return args
end

Object.set_dialog = function(self, type, args)
	-- Update the dialog state.
	if type == "choice" then
		self.dialog = {type = type, choices = args}
	elseif type == "message" then
		self.dialog = {type = type, character = args.character, message = args.message}
	else
		self.dialog = nil
	end
	-- Update the dialog UI.
	if Client.active_dialog_object == self then
		Ui:restart_state()
	end
end

Object.update = function(self, secs)
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
end

--- Sets the model and special effects of the object from its spec.
-- @param self Object.
-- @param skip True to only set the special effects.
Object.set_model = function(self, skip)
	if not self.spec then return end
	-- Set the new model.
	if not skip then
		local shape = self.shape
		self.model = Model:find_or_load{file = self.spec.model}
		self.shape = shape
	end
	-- Detach old special effects.
	if self.special_effects then
		for k,v in pairs(self.special_effects) do v:detach() end
		self.special_effects = nil
	end
	self.special_effects = nil
	-- Create new special effects.
	local effects = self.spec.get_special_effects and self.spec:get_special_effects()
	if effects then
		self.special_effects = {}
		for k,v in pairs(effects) do
			if v.light then
				local fx = Light{ambient = v.light_ambient, diffuse = v.light_diffuse, node = v.node,
					equation = v.light_equation, position = self.position, enabled = true}
				table.insert(self.special_effects, fx)
			end
			if v.particle then
				local fx = Object{particle = v.particle, node = v.node, position = self.position, realized = true}
				table.insert(self.special_effects, fx)
			end
		end
	end
end

--- Rebuilds the character model of the object.
-- @param self Object.
Object.update_model = function(self)
end

--- Plays footstep sounds for actors.
-- @param self Object.
-- @param secs Seconds since the last update.
Object.update_sound = function(self, secs)
end

--- Updates the rotation and tilt of the object.
-- @param self Object.
-- @param quat Rotation quaternion.
-- @param tilt Tilt angle in radians.
Object.update_rotation = function(self, quat, tilt)
	self.rotation = quat
	self.tilt = tilt
	self.prediction:set_target_rotation(quat, tilt)
end

Object.update_motion_state = function(self, secs)
	self.prediction:update(secs)
	self.position = self.prediction:get_predicted_position()
	if self.dead or self ~= Client.player_object then
		self.rotation = self.prediction:get_predicted_rotation()
		self.tilt = self.prediction:get_predicted_tilt()
	end
end

Object.set_motion_state = function(self, pos, rot, vel, tilt)
	self.prediction:set_target_state(pos, rot, tilt, vel)
	Object.dict_active[self] = 5.0
end

-- Dummy functions for testing without object rendering.
if not Object.animate then
	Object.animate = function() end
end
if not Object.find_node then
	Object.find_node = function() end
end
if not Object.get_animation then
	Object.get_animation = function() end
end
