local Class = require("system/class")
local Widget = require("system/widget")

Widgets.Quickslot = Class("Quickslot", Widget)

Widgets.Quickslot.new = function(clss, args)
	local self = Widget.new(clss, args)
	return self
end

Widgets.Quickslot.reshaped = function(self)
	local icon = self.icon or self.DEFAULT
	self:set_request(39, 34, true)
	local w = self:get_width()
	local h = self:get_height() + 9
	-- Background.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,-9},
		dest_size = {w, h},
		source_image = "widgets1",
		source_position = {810,379},
		source_tiling = {7,25,7,8,28,7}}
	-- Icon.
	if self.icon then
		self:canvas_image{
			dest_position = {(w-icon.size[1])/2+1,(h-icon.size[2])/2-9},
			dest_size = icon.size,
			source_image = icon.image,
			source_position = icon.offset,
			source_tiling = {0,icon.size[1],0,0,icon.size[1],0}}
	end
	-- Number.
	if self.index then
		self:canvas_text{
			dest_position = {6,-3},
			dest_size = {28,28},
			text = string.format("%d.", self.index),
			text_alignment = {0,0},
			text_color = {1,1,1,0.5},
			text_font = "tiny"}
	end
	-- Text.
	if self.text then
		self:canvas_text{
			dest_position = {6,0},
			dest_size = {28,28},
			text = self.text,
			text_alignment = {0,1},
			text_color = {1,1,1,1},
			text_font = "tiny"}
	end
	-- Glass.
	if not self.focused then
		self:canvas_image{
			dest_position = {0,-9},
			dest_size = {w, h},
			source_image = "widgets1",
			source_position = {860,379},
			source_tiling = {7,25,7,8,28,7}}
	end
	self:canvas_compile()
end

Widgets.Quickslot.pressed = function(self, args)
end
