--- Prioritized hook management.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.hooks
-- @alias Hooks

local Class = require("system/class")

------------------------------------------------------------------------------

--- Prioritized hook management.
-- @type Hooks
local Hooks = Class("Hooks")

Hooks.STOP = "stop"

--- Creates a new hook manager.
-- @param clss Hooks class.
-- @return New hook manager.
Hooks.new = function(clss)
	local self = Class.new(clss)
	self.hooks = {}
	return self
end

--- Calls all the hooks.
-- @param self Hooks.
-- @param ... Optional arguments.
Hooks.call = function(self, ...)
	for k,v in ipairs(self.hooks) do
		if v.hook(...) == self.STOP then return end
	end
end

--- Registers an initialization hook.
-- @param self Hooks.
-- @param priority Priority.
-- @param hook Function.
Hooks.register = function(self, priority, hook)
	local h = {priority = priority, hook = hook}
	for k,v in ipairs(self.hooks) do
		if priority < v.priority then
			table.insert(self.hooks, k, h)
			return
		end
	end
	table.insert(self.hooks, h)
end

return Hooks
