--- Triggers dialogs.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.dialog.trigger
-- @alias Trigger

local Class = require("system/class")

--- Triggers dialogs.
-- @type Trigger
local Trigger = Class("Trigger")

--- Creates a new dialog trigger.
-- @param clss Trigger class.
-- @param manager Triggers.
-- @param name Name.
-- @return Trigger.
Trigger.new = function(clss, manager, name)
	local self = Class.new(clss)
	self.manager = manager
	self.name = name
	return self
end

--- Updates the trigger.
-- @param self Trigger.
-- @param secs Seconds since the last update.
-- @return Dialog name if triggered. Nil otherwise.
Trigger.update = function(self, secs)
end

return Trigger
