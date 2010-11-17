Widgets.IconButton = Class(Widget)

Widgets.IconButton.setter = function(self, key, value)
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
	elseif key == "icon" then
		if self.icon ~= value then
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

Widgets.IconButton.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.font = self.font or "default"
	self.icon = self.icon or ""
	self.text = self.text or ""
	return self
end

Widgets.IconButton.reshaped = function(self)
	self:set_request{
		internal = true,
		font = self.font,
		text = self.text,
		paddings = {0,32,32,0}}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = self.focused and {64,0} or {0,0},
		source_tiling = {6,52,6,6,52,6}}
	local icon = Iconspec:find{name = self.icon}
	if icon then
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {20,20},
			source_image = icon.image,
			source_position = icon.offset,
			source_tiling = {0,icon.size[1],0,0,icon.size[1],0}}
	end
	self:canvas_text{
		dest_position = {18,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {0,0.5},
		text_color = self.focused and {1,1,0,1} or {1,1,1,1},
		text_font = self.font}
	self:canvas_compile()
end

Widgets.IconButton.pressed = function(self, args)
end
