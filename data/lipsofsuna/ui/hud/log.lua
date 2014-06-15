--- HUD message log widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.hud.log
-- @alias Hudlog

local Class = require("system/class")
local Graphics = require("system/graphics")
local Widget = require("system/widget")

--- HUD message log widget.
-- @type Hudlog
local Hudlog = Class("Hudlog", Widget)

Hudlog.new = function(clss)
	local self = Widget.new(clss)
	self.lines = {}
	return self
end

Hudlog.append = function(self, text)
	table.insert(self.lines, 1, {text, 0, 1})
	self.need_relayout = true
end

Hudlog.reshaped = function(self)
	self:canvas_clear()
	local w = self:get_width()
	local h = self:get_height()
	local y = 200
	for k,v in ipairs(self.lines) do
		local tw,th = Graphics:measure_text(Theme.text_font_1, v[1], w)
		y = y - th
		Theme:draw_scene_text(self, v[1], 0, y, w, th, v[3], {0.5,0})
	end
end

Hudlog.update = function(self, secs)
	-- Check for a relayout.
	local changed = self.need_relayout
	self.need_relayout = true
	-- Update the screen offset.
	local mode = Graphics:get_video_mode()
	if self:get_width() ~= mode[1] or self:get_offset().y ~= mode[2] - 200 then
		self:set_offset(Vector(5, mode[2] - Theme.text_height_1 * 3 - 200))
		self:set_request(mode[1], 200)
		changed = true
	end
	-- Fade out messages.
	local i = 1
	while self.lines[i] do
		local line = self.lines[i]
		line[2] = line[2] + secs * math.exp((i-1)*0.3)
		if line[2] < 6 then
			if line[2] > 5 then
				line[3] = 6 - line[2]
				changed = true
			end
			i = i + 1
		else
			table.remove(self.lines, i)
			changed = true
		end
	end
	-- Update the display.
	if changed then
		self:reshaped()
	end
end

return Hudlog
