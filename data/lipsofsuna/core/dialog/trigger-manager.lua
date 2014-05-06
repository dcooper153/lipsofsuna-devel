--- Manages dialog triggers.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.dialog.trigger_manager
-- @alias TriggerManager

local Class = require("system/class")

--- Manages dialog triggers.
-- @type Trigger
local TriggerManager = Class("TriggerManager")

TriggerManager.classes = {}

--- Creates a new dialog trigger manager.
-- @param clss TriggerManager class.
-- @return TriggerManager.
TriggerManager.new = function(clss)
	local self = Class.new(clss)
	self.triggers = setmetatable({}, {mode = "k"})
	return self
end

--- Registers a trigger and activates it.
-- @param self TriggerManager.
-- @param trigger Trigger.
TriggerManager.add_triggers = function(self, triggers)
	self.triggers[triggers] = true
end

--- Disables and unregisters a trigger.
-- @param self TriggerManager.
-- @param trigger Trigger.
TriggerManager.remove_triggers = function(self, trigger)
	self.triggers[triggers] = nil
end

--- Updates the triggers.
-- @param self TriggerManager.
-- @param secs Seconds since the last update.
TriggerManager.update = function(self, secs)
	for k in pairs(self.triggers) do
		k:update(secs)
	end
end

--- Registers a trigger class.
-- @param clss TriggerManager class.
-- @param name Trigger name.
-- @param update Update function.
TriggerManager.register = function(clss, name, update)
	clss.classes[name] = update
end

return TriggerManager
