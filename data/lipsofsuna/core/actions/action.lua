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
	self.spec = spec
	self.object = object
	return self
end

return Action
