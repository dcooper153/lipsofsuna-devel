--- Manages the windows of the user interface.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.window_manager
-- @alias WindowManager

local Class = require("system/class")

--- Manages the windows of the user interface.
-- @type WindowManager
local WindowManager = Class("WindowManager")

--- Creates a new window manager.
-- @param clss WindowManager class.
-- @return WindowManager.
WindowManager.new = function(clss)
	local self = Class.new(clss)
	return self
end

--- Handles an input event.
-- @param self WindowManager.
-- @param args Event arguments.
-- @return True if the caller should handle the event.
WindowManager.handle_event = function(self, args)
	return Ui:handle_event(args)
end

--- Updates the user interface.
-- @param self WindowManager.
-- @param secs Seconds since the last update.
WindowManager.update = function(self, secs)
	-- Emit key repeat events.
	local t = Program:get_time()
	for k,v in pairs(Client.input.pressed) do
		if t - v.time > 0.05 then
			v.type = "keyrepeat"
			v.mods = Client.input.mods
			v.time = t
			self:handle_event(v)
		end
	end
	-- Update the user interface state.
	Ui:update(secs)
	-- Update the window size.
	if Ui.was_resized then
		local v = Program:get_video_mode()
		Client.options.window_width = v[1]
		Client.options.window_height = v[2]
		Client.options.fullscreen = v[3]
		Client.options.vsync = v[4]
		Client.options:save()
	end
end

return WindowManager
