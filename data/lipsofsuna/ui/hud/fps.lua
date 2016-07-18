--- HUD FPS counter widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.hud.fps
-- @alias Hudfps

local Class = require("system/class")
local UiWindow = require("ui/windows/ui-window")

--- HUD FPS counter widget.
-- @type Hudfps
local Hudfps = Class("Hudfps", UiWindow)

Hudfps.new = function(clss)
	local self = UiWindow.new(clss)
	return self
end

Hudfps.reshaped = function(self)
	local w,h = self:get_size()
	self:canvas_clear()
	Theme:draw_scene_text(self, self.text, 0, 0, w, h, 1, {0, 0.5})
end

--- Called when the screen size changes.
-- @param self Hudfps.
-- @param width Screen width.
-- @param height Screen height.
Hudfps.screen_resized = function(self, w, h)
	self:set_position(w - Theme.text_height_1 * 4.5, h - Theme.text_height_1 - 5)
end

Hudfps.update = function(self, secs)
	local text = "FPS: " .. tostring(math.floor(Program:get_fps() + 0.5))
	self.font = Theme.text_font_1
	self.text = text
	self:reshaped()
end

return Hudfps
