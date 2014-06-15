--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.scene.movement_prediction
-- @alias MovementPrediction

local Class = require("system/class")
local Quaternion = require("system/math/quaternion")

--- TODO:doc
-- @type MovementPrediction
local MovementPrediction = Class("MovementPrediction")

--- Creates a new movement predictor.
-- @param clss Movement prediction class.
-- @return Movement predictor.
MovementPrediction.new = function(clss)
	local self = Class.new(clss)
	self.enabled = true
	self.timer = 0
	self.tick = 1/60
	self.pred_position = Vector()
	self.pred_rotation = Quaternion()
	self.pred_tilt = 0
	self.target_position = Vector()
	self.target_rotation = Quaternion()
	self.target_tilt = 0
	self.target_velocity = Vector()
	self.ipol_velocity = Vector()
	self.position_max_error = 20
	self.position_corr_rate = 0.2
	self.rotation_corr_rate = 0.2
	self.prediction_velocity_decay = 0.99
	return self
end

--- Marks the current target state as a known state.
-- @param self Movement prediction.
MovementPrediction.mark = function(self)
	self.ipol_timer = 0
	self.ipol_velocity = self.target_velocity:copy()
end

--- Enables or disables movement prediction.
-- @param self Movement prediction.
-- @param value True to enable.
MovementPrediction.set_enabled = function(self, value)
	self.enabled = value
	if not value then self:warp() end
end

--- Gets the current predicted position.
-- @param self Movement prediction.
-- @return Vector.
MovementPrediction.get_predicted_position = function(self)
	return self.pred_position
end

--- Gets the current predicted rotation.
-- @param self Movement prediction.
-- @return Quaternion.
MovementPrediction.get_predicted_rotation = function(self)
	return self.pred_rotation
end

--- Gets the current predicted tilt.
-- @param self Movement prediction.
-- @return Number.
MovementPrediction.get_predicted_tilt = function(self)
	return self.pred_tilt
end

--- Sets the target rotation.
-- @param self Movement prediction.
-- @param rot Rotation quaternion.
MovementPrediction.set_target_rotation = function(self, rot)
	self.target_rotation = rot or self.pred_rotation:copy()
	if not self.enabled then self:warp() end
end

--- Sets the target tilt angle.
-- @param self Movement prediction.
-- @param tilt Tilt angle.
MovementPrediction.set_target_tilt = function(self, tilt)
	self.target_tilt = tilt or 0
	if not self.enabled then self:warp() end
end

--- Sets the target velocity.
-- @param self Movement prediction.
-- @param vel Velocity vector.
MovementPrediction.set_target_velocity = function(self, vel)
	self.target_velocity = vel or Vector()
end

--- Sets the target position.
-- @param self Movement prediction.
-- @param value Position vector.
MovementPrediction.set_target_position = function(self, value)
	self.target_position = value
	if not self.enabled then self:warp() end
end

--- Updates the movement prediction.
-- @param self Movement prediction.
-- @param secs Seconds since the last update.
MovementPrediction.update = function(self, secs)
	-- Check if enabled.
	if not self.enabled then
		return self:warp()
	end
	-- Update the timer.
	if not self.ipol_timer then return self:warp() end
	self.timer = self.timer + secs
	-- Handle big warpings in time.
	if self.timer > 1 then
		return self:warp()
	end
	-- Handle big warpings in position.
	if (self.target_position - self.pred_position).length > self.position_max_error then
		return self:warp()
	end
	-- Update periodically to ensure frame rate independence.
	while self.timer > self.tick do
		self.timer = self.timer - self.tick
		-- Damp velocity to reduce overshoots.
		self.ipol_timer = self.ipol_timer + self.tick
		self.ipol_velocity:multiply(self.prediction_velocity_decay)
		-- Apply rotation smoothing over time.
		self.pred_rotation:nlerp(self.target_rotation, self.rotation_corr_rate)
		self.pred_tilt = self.pred_tilt * (1 - self.rotation_corr_rate) + self.target_tilt * self.rotation_corr_rate
		-- Apply position change predicted by the velocity.
		self.pred_position:add(self.ipol_velocity * self.tick)
		-- Correct position prediction errors over time.
		self.pred_position:lerp(self.target_position, self.position_corr_rate)
	end
end

--- Warps the predictor to the last known position.
-- @param self Movement prediction.
MovementPrediction.warp = function(self)
	self.timer = 0
	self.ipol_timer = 1
	self.pred_position = self.target_position
	self.pred_rotation = self.target_rotation
end

return MovementPrediction


