local Class = require("system/class")
local Label = require("system/widgets/label")
local Widget = require("system/widget")

Widgets.Tooltip = Class("Tooltip", Widget)

Widgets.Tooltip.new = function(clss, text)
	local self = Widget.new(clss)
	self:set_depth(15)
	-- Create the label.
	local label = Label()
	label:set_text(text)
	label:set_request(150, nil)
	--self:append_row(label) -- FIXME: Use manual packing instead
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
	local w = self:get_width()
	local h = self.get_height()
	self:canvas_clear()
	self:set_request{internal = true, width = 100, height = 20}
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = {720,0},
		source_tiling = {10,80,21,10,30,21}}
	self:canvas_compile()
end
