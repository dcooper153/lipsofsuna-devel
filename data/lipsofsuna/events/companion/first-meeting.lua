--- Companion event trigger.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module events.companion.first_meeting
-- @alias FirstMeetingTrigger

local TriggerManager = require("core/dialog/trigger-manager")

TriggerManager:register("first meeting", function(self, secs)
	if Main.quests:get_dialog_variable(self.manager.object, "player_met") then
		self.manager:disable_trigger(self)
		return
	end
	return self.name
end)
