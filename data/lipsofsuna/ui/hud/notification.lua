local Class = require("system/class")
local Widget = require("system/widget")

Widgets.Hudnotification = Class("Hudnotification", Widget)

Widgets.Hudnotification.new = function(clss)
	local self = Widget.new(clss)
	return self
end

Widgets.Hudnotification.reshaped = function(self)
	local w = Theme.text_height_1*10
	self:calculate_request{
		font = Theme.text_font_1,
		internal = true,
		text = self.__text,
		paddings = {7,7,7,7},
		width = w-14}
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,self:get_height()},
		source_image = "widgets2",
		source_position = {300,0},
		source_tiling = {7,86,7,7,86,7}}
	self:canvas_text{
		dest_position = {7,7},
		dest_size = {w-14,self:get_height()-10},
		text = self.__text,
		text_alignment = {0.5,0.5},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
end

Widgets.Hudnotification.update = function(self, secs)
	if not self:get_visible() then return end
	if self.__text and self.timer < 5 then
		-- Show.
		local mode = Program:get_video_mode()
		local off = 1
		if self.timer < 0.5 then off = 2 * self.timer end
		if self.timer > 4.5 then off = 2 * (5 - self.timer) end
		self:set_offset(Vector(mode[1], mode[2]) - Vector(self:get_width(), self:get_height() * off))
		self.timer = self.timer + secs
	else
		-- Hide.
		self.timer = nil
		self:set_visible(false)
	end
end

Widgets.Hudnotification.get_text = function(self)
	return self.__text
end

Widgets.Hudnotification.set_text = function(self, v)
	if self.__text == v then return end
	self.__text = v
	self.timer = 0
	self:reshaped()
	self:set_visible(true)
end
