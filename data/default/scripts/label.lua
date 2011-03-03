Widgets.Label = Class(Widget)

Widgets.Label.setter = function(self, key, value)
	if key == "focused" then
		if self.focused ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "font" then
		if self.font ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "text" then
		if self.text ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	else
		Widget.setter(self, key, value)
	end
end

Widgets.Label.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.font = self.font or "default"
	self.text = self.text or ""
	self.halign = self.halign or 0
	self.valign = self.valign or 0.5
	return self
end

Widgets.Label.reshaped = function(self)
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = self.text,
		width = self:get_request()}
	local w = self.width
	local h = self.height
	local f = self.focused
	local p = self.pressed
	self:canvas_clear()
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {self.halign,self.valign},
		text_color = f and p and {0.6,0.6,0,1} or p and {0.6,0.6,0.6,0.6} or {1,1,1,1},
		text_font = self.font}
	self:canvas_compile()
end
