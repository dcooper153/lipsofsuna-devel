local Class = require("system/class")
local Label = require("system/widgets/label")
local Widget = require("system/widget")

Widgets.Tooltip = Class("Tooltip", Widget)

Widgets.Tooltip.new = function(clss, text)
	local self = Widget.new(clss)
	self:set_depth(15)
	self.text = text
	return self
end

Widgets.Tooltip.popup = function(self, point)
	local mode = Program:get_video_mode()
	self:set_floating(true)
	if point.x > mode[1] - self:get_width() then
		self:set_x(mode[1] - self:get_width())
	else
		self:set_x(point.x)
	end
	if point.y > mode[2] - self:get_height() then
		self:set_y(mode[2] - self:get_height())
	else
		self:set_y(point.y)
	end
end

Widgets.Tooltip.reshaped = function(self)
	self:calculate_request{
		font = Theme.text_font_1,
		internal = true,
		paddings = {22,2,2,2},
		text = self.text,
		width = 150 * Theme.scale}
	local w = self:get_width()
	local h = self:get_height()
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = {720,0},
		source_tiling = {10,80,21,10,30,21}}
	self:canvas_text{
		dest_position = {2,5},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {0,0},
		text_color = Theme.text_color_3,
		text_font = Theme.text_font_1}
	self:canvas_compile()
end
