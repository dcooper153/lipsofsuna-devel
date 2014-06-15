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
local Graphics = require("system/graphics")
local Vector = require("system/math/vector")
local Widget = require("system/widget")

--- HUD FPS counter widget.
-- @type Hudfps
local Hudfps = Class("Hudfps", Widget)

local __vec1 = Vector()

Hudfps.new = function(clss)
	local self = Widget.new(clss)
	return self
end

Hudfps.reshaped = function(self)
	local w = self:get_width()
	local h = self:get_height()
	self:canvas_clear()
	Theme:draw_scene_text(self, self.text, 0, 0, w, h, 1, {0, 0.5})
end

Hudfps.update = function(self, secs)
	local mode = Graphics:get_video_mode()
	local text = "FPS: " .. tostring(math.floor(Program:get_fps() + 0.5))
	self:set_offset(__vec1:set_xyz(mode[1] - Theme.text_height_1 * 4.5, mode[2] - Theme.text_height_1 - 5))
	self.font = Theme.text_font_1
	self.text = text
	self:reshaped()
end

return Hudfps
