--- Action performed by an object.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.actions.action
-- @alias Action

local Class = require("system/class")

--- Action performed by an object.
-- @type Action
local Action = Class("Action")

--- Creates a new action.
-- @param clss Action class.
-- @param spec Actionspec.
-- @param object Object.
-- @return New action.
Action.new = function(clss, spec, object)
	local self = Class.new(clss)
	self.name = spec.name
	self.spec = spec
	self.object = object
	return self
end

--- Gets the range of the action.
-- @param self Action.
-- @return Range in world units.
Action.get_range = function(self)
	if not self.spec.get_range then return self.object.spec.aim_ray_end * 0.9 end
	return self.spec.get_range(self)
end

--- Gets the AI score of the action.
-- @param self Action.
-- @return Range in world units.
Action.get_score = function(self)
	if not self.spec.get_score then return -1 end
	return self.spec.get_score(self)
end

--- Checks if the action is usable.
-- @param self Action.
-- @return True if usable. False otherwise.
Action.get_usable = function(self)
	if not self.spec.get_usable then return true end
	return self.spec.get_usable(self)
end

return Action
