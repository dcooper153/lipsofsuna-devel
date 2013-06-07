--- Manages the local player state.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.player_state
-- @alias PlayerState

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

--- Manages the local player state.
-- @type PlayerState
local PlayerState = Class("PlayerState")

--- Creates a new player state.
-- @param clss PlayerState class.
-- @return PlayerState.
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

--- Tilts the player object on the client side.
-- @param self PlayerState.
-- @param value Tilting speed as radians per second.
PlayerState.tilt = function(self, value)
	self.tilt_speed = self.tilt_speed + value
end

--- Turns the player object on the client side.
-- @param self PlayerState.
-- @param value Turning speed as radians per second.
PlayerState.turn = function(self, value)
	self.turn_speed = self.turn_speed + value
end

--- Updates the local player state.
-- @param self PlayerState.
-- @param secs Seconds since the last update.
PlayerState.update = function(self, secs)
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

--- Gets the world space crosshair position.
-- @param self PlayerState.
-- @return Vector, or nil.
PlayerState.get_crosshair_position = function(self)
	return self.crosshair
end

--- Gets the client side turning angle of the player object.
-- @param self PlayerState.
-- @return Number.
PlayerState.get_turn_angle = function(self)
	return self.rotation_curr.euler[1]
end

--- Gets the object currently under the crosshair.
-- @param self PlayerState.
-- @return Object, or nil.
PlayerState.get_targeted_object = function(self)
	return self.target_object
end

--- Sets the object currently under the crosshair.
-- @param self PlayerState.
-- @param object Object, or nil.
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
