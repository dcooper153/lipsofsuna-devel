--- Manages the local player controls.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.control_state
-- @alias ControlState

local Class = require("system/class")

--- Manages the local player controls.
-- @type ControlState
local ControlState = Class("ControlState")

--- Creates a new control state.
-- @param clss ControlState class.
-- @return ControlState.
ControlState.new = function(clss)
	local self = Class.new(clss)
	self.__movement = 0
	self.__sidestep = 0
	return self
end

--- Updates the state.
-- @param self ControlState.
-- @param secs Seconds since the last update.
ControlState.update = function(self, secs)
	if not Ui:get_pointer_grab() then
		self:set_movement(0)
		self:set_sidestep(0)
	end
end

--- Sets the movement control.
-- @param self ControlState.
-- @param value Value.
ControlState.set_movement = function(self, value)
	local v = math.max(-1, math.min(1, -value))
	if v == self.__movement then return end
	self.__movement = v
	Main.messaging:client_event("walk", self.__movement)
end

--- Sets the sidestep control.
-- @param self ControlState.
-- @param value Value.
ControlState.set_sidestep = function(self, value)
	if value == self.__sidestep then return end
	self.__sidestep = value
	Main.messaging:client_event("sidestep", value)
end

return ControlState
