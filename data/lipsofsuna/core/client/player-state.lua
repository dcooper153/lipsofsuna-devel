local Class = require("system/class")
local Simulation = require("core/client/simulation")

local radian_wrap = function(x)
	local y = x
	while y < -math.pi do
		y = y + 2 * math.pi
	end
	while y > math.pi do
		y = y - 2 * math.pi
	end
	return y
end

local PlayerState = Class()

PlayerState.new = function(clss)
	local self = Class.new(clss)
	self.tilt_speed = 0
	self.tilt_state = 0
	self.turn_speed = 0
	self.turn_state = 0
	self.rotation_curr = Quaternion()
	self.rotation_prev = Quaternion()
	self.rotation_sync_timer = 0
	return self
end

PlayerState.tilt = function(self, value)
	self.tilt_speed = self.tilt_speed + value
end

PlayerState.turn = function(self, value)
	self.turn_speed = self.turn_speed + value
end

PlayerState.update = function(self, secs)
	-- Update targeting.
	if Client.player_object then
		local r1,r2 = Client.camera1:get_picking_ray()
		if r1 then
			local p,o = Simulation:pick_scene_by_ray(r1, r2)
			self:set_targeted_object(o)
			self.crosshair = (p or r2) - (r2 - r1):normalize() * 0.1
		end
	else
		self:set_targeted_object()
		self.crosshair = nil
	end
	-- Update rotation.
	if Client.player_object and not Client.player_object.dead then
		local spec = Client.player_object.spec
		-- Update turning.
		self.turn_state = self.turn_state + self.turn_speed * secs
		self.turn_state = radian_wrap(self.turn_state)
		self.turn_speed = 0
		-- Update tilting.
		self.tilt_state = self.tilt_state + self.tilt_speed * secs
		if spec then
			self.tilt_state = math.min(spec.tilt_limit, self.tilt_state)
			self.tilt_state = math.max(-spec.tilt_limit, self.tilt_state)
		end
		self.tilt_speed = 0
		-- Synchronize with the server.
		if Game.initialized then
			Game.messaging:client_event("rotate", self.turn_state, self.tilt_state)
		end
	end
end

PlayerState.get_crosshair_position = function(self)
	return self.crosshair
end

PlayerState.get_turn_angle = function(self)
	return self.rotation_curr.euler[1]
end

PlayerState.get_targeted_object = function(self)
	return self.target_object
end

PlayerState.set_targeted_object = function(self, object)
	-- Check for changes.
	if self.target_object == object then return end
	-- Update highlighting.
	if self.target_object then
		self.target_object.render:set_effect()
	end
	if object then
		object.render:set_effect{shader = "highlight"}
	end
	self.target_object = object
	-- Update the interaction text.
	if object and object.spec and object.spec.interactive then
		local key = Client.bindings:get_control_name("use")
		local name = object.name ~= "" and object.name or object.spec.name
		Client:set_target_text(string.format("%s Interact with %s", key, name))
	else
		Client:set_target_text()
	end
end

return PlayerState
