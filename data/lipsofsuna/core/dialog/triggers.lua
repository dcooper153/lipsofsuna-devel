--- Dialog triggers of one actor.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.dialog.triggers
-- @alias Triggers

local Class = require("system/class")

--- Dialog triggers of one actor.
-- @type Trigger
local Triggers = Class("Triggers")

--- Creates new dialog triggers.
-- @param clss Triggers class.
-- @param object Object affected by the triggers.
-- @return Triggers.
Triggers.new = function(clss, manager, object)
	local self = Class.new(clss)
	self.object = object
	self.triggers_active = {}
	self.triggers_inactive = {}
	self.manager = manager
	self.manager:add_triggers(self)
	return self
end

--- Registers a trigger and activates it.
-- @param self Triggers.
-- @param trigger Trigger.
Triggers.add_trigger = function(self, trigger)
	self.triggers_active[trigger.name] = trigger
	self.triggers_inactive[trigger.name] = nil
end

--- Disables a trigger.
-- @param self Triggers.
-- @param trigger Trigger.
Triggers.disable_trigger = function(self, trigger)
	self.triggers_active[trigger.name] = nil
	self.triggers_inactive[trigger.name] = trigger
end

--- Enables a trigger.
-- @param self Triggers.
-- @param trigger Trigger.
Triggers.enable_trigger = function(self, trigger)
	self.triggers_active[trigger.name] = trigger
	self.triggers_inactive[trigger.name] = nil
end

--- Disables and unregisters a trigger.
-- @param self Triggers.
-- @param trigger Trigger.
Triggers.remove_trigger = function(self, trigger)
	self.triggers_active[trigger.name] = nil
	self.triggers_inactive[trigger.name] = nil
end

--- Updates the triggers.
-- @param self Triggers.
-- @param secs Seconds since the last update.
Triggers.update = function(self, secs)
	for k,v in pairs(self.triggers_active) do
		local dlg = v:update(secs)
		if dlg then
			Main.dialogs:cancel(self.object)
			Main.dialogs:execute_custom(self.object, self.object.owner, dlg)
			break
		end
	end
end

return Triggers
