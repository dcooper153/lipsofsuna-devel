require "client/movement-prediction"
require "client/objects/base"
require "client/objects/speedline"

RemoteObject = Class(BaseObject)
RemoteObject.class_name = "RemoteObject"

--- Creates a new object that can be synchronized with a server side object.
-- @param clss Remote object class.
-- @param args Arguments.
-- @returns New object.
RemoteObject.new = function(clss, args)
	local self = BaseObject.new(clss, args)
	self.inventory = Inventory{id = self.id}
	self.prediction = MovementPrediction()
	return self
end

--- Adds an animation from the spec of the object by name.
-- @param self Object.
-- @param name Animation name.
-- @param time Optional start time, or nil.
-- @return Animation arguments.
RemoteObject.add_animation = function(self, name, time)
	-- Play the animation.
	local args = {animation = name, fade_in = 0.3, fade_out = 0.3, time = time}
	local anim = self.spec and self.spec.get_animation and self.spec:get_animation(name)
	if anim then
		for k,v in pairs(anim:get_arguments()) do args[k] = v end
	end
	self:animate(args)
	-- Mark as active.
	self.animated = true
	self:activate(5)
	return args
end

--- Adds a speed line effect for the object.
-- @param self Object.
-- @param args Speed line construction arguments.
RemoteObject.add_speedline = function(self, args)
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

--- Sets the dialog state of the object.
-- @param self Object.
-- @param type Dialog type. ("choice"/"message"/nil)
-- @param args List of choices, the message arguments, or nil.
RemoteObject.set_dialog = function(self, type, args)
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

--- Sets the model and special effects of the object from its spec.
-- @param self Object.
-- @param skip True to only set the special effects.
RemoteObject.set_model = function(self, skip)
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
RemoteObject.update_model = function(self)
end

--- Plays footstep sounds for actors.
-- @param self Object.
-- @param secs Seconds since the last update.
RemoteObject.update_sound = function(self, secs)
end

--- Sets the client overridden rotation and tilt of the object.
-- @param self Object.
-- @param quat Rotation quaternion.
-- @param tilt Tilt angle in radians.
RemoteObject.set_local_rotation = function(self, quat, tilt)
	if quat then self.rotation = quat end
	if tilt then self.tilt = tilt end
	self.prediction:set_target_rotation(quat, tilt)
end

--- Sets the target motion state for movement prediction.
-- @param self Object.
-- @param pos Position vector.
-- @param rot Rotation quaternion.
-- @param vel Velocity vector.
-- @param tilt Tilt angle.
RemoteObject.set_motion_state = function(self, pos, rot, vel, tilt)
	self.prediction:set_target_state(pos, rot or Quaternion(), tilt, vel)
	self:activate(5)
end
