local Class = require("system/class")
local Widget = require("system/widget")

local Button = Class("Button", Widget)

Button.new = function(clss)
	local self = Widget.new(clss)
	self.font = "default"
	self.text = ""
	self.enabled = true
	return self
end

Button.handle_event = function(self, args)
	if args.type ~= "mousepress" then return true end
	self:pressed()
end

Button.pressed = function(self)
end

Button.reshaped = function(self)
	-- Calculate the size.
	self:calculate_request{
		font = self.font,
		internal = true,
		paddings = {3,6,6,3},
		text = self.text}
	local w = self:get_width()
	local h = self:get_height()
	-- Rebuild the canvas.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets2",
		source_position = self.focused and {100,100} or {100,0},
		source_tiling = {7,86,7,7,86,7}}
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {0.5,0.5},
		text_color = {0,0,0,1},
		text_font = self.font}
end

Button.get_enabled = function(self)
	return self.enabled
end

Button.set_enabled = function(self, v)
	if self.enabled == v then return end
	self.enabled = v
	self:reshaped()
end

Button.get_focused = function(self)
	return self.focused
end

Button.set_focused = function(self, v)
	if self.focused == v then return end
	self.focused = v
	self:reshaped()
end

Button.get_font = function(self)
	return self.font
end

Button.set_font = function(self, v)
	if self.font == v then return end
	self.font = v
	self:reshaped()
end

Button.get_text = function(self)
	return self.text
end

Button.set_text = function(self, v)
	if self.text == v then return end
	self.text = v
	self:reshaped()
end

return Button
