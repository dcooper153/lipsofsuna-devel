--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.widgets.label
-- @alias Label

local Class = require("system/class")
local Widget = require("system/widget")

--- TODO:doc
-- @type Label
local Label = Class("Label", Widget)

Label.new = function(clss)
	local self = Widget.new(clss)
	self.text = ""
	self.halign = 0
	self.valign = 0.5
	return self
end

Label.reshaped = function(self)
	local wrap,_ = self:get_request()
	self:calculate_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = self.text,
		width = wrap and (wrap - 4)}
	local f = self.focused
	local p = self.pressed
	self:canvas_clear()
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {self:get_width(),self:get_height()},
		text = self.text,
		text_alignment = {self.halign,self.valign},
		text_color = self.color or (f and p and {0.6,0.6,0,1} or p and {0.6,0.6,0.6,0.6} or {1,1,1,1}),
		text_font = self.font}
	self:canvas_compile()
end

Label.get_color = function(self)
	return self.color
end

Label.get_focused = function(self)
	return self.focused
end

Label.get_font = function(self)
	return self.font
end

Label.get_text = function(self)
	return self.text
end

Label.set_color = function(self, v)
	if not v then return end
	self.color = v
	self:reshaped()
end

Label.set_focused = function(self, v)
	if self.focused == v then return end
	self.focused = v
	self:reshaped()
end

Label.set_halign = function(self, v)
	if not v then return end
	if self.halign == v then return end
	self.halign = v
	self:reshaped()
end

Label.set_font = function(self, v)
	if not v then return end
	if self.font == v then return end
	self.font = v
	self:reshaped()
end

Label.set_text = function(self, v)
	if not v then return end
	if self.text == v then return end
	self.text = v
	self:reshaped()
end

Label.set_valign = function(self, v)
	if not v then return end
	if self.valign == v then return end
	self.halign = v
	self:reshaped()
end

return Label


