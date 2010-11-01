Widgets.Check = Class(Widget)

Widgets.Check.setter = function(self, key, value)
	if key == "active" then
		if self.active ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "focused" then
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

Widgets.Check.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.font = self.font or "default"
	self.text = self.text or ""
	return self
end

Widgets.Check.pressed = function(self)
	self.active = not self.active
end

Widgets.Check.reshaped = function(self)
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = self.text}
	local w = self.width
	local h = self.height
	local f = (self.focused and not self.active) or (not self.focused and self.active)
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = f and {64,0} or {0,0},
		source_tiling = {6,52,6,6,52,6}}
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {0.5,0.5},
		text_color = f and {1,1,0,1} or {1,1,1,1},
		text_font = self.font}
	self:canvas_compile()
end
