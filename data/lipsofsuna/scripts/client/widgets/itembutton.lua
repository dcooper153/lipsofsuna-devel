Widgets.ItemButton = Class(Widget)

Widgets.ItemButton.setter = function(self, key, value)
	if key == "count" then
		if self.count ~= value then
			Widgets.IconButton.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "drag" then
		if self.drag ~= value then
			Widgets.IconButton.setter(self, key, value)
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

Widgets.ItemButton.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.font = self.font or "default"
	self.icon = self.icon or ""
	self.text = self.text or ""
	return self
end

Widgets.ItemButton.reshaped = function(self)
	self:set_request{
		internal = true,
		height = 20,
		width = 150}
	local w = self.width
	local h = self.height
	local a = self.drag and 0.2 or 1
	-- Background.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = self.focused and {650,395} or {650,375},
		source_tiling = {20,109,20,6,7,6}}
	-- Icon.
	local icon = Iconspec:find{name = self.icon}
	if icon then
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {20,20},
			color = {1,1,1,a},
			source_image = icon.image,
			source_position = icon.offset,
			source_tiling = {0,icon.size[1],0,0,icon.size[1],0}}
	end
	-- Text.
	if self.text then
		self:canvas_text{
			dest_position = {22,0},
			dest_size = {w,h},
			text = self.text,
			text_alignment = {0,0.5},
			text_color = self.focused and {1,1,0,a} or {1,1,1,a},
			text_font = self.font}
	end
	-- Count.
	if self.count and self.count > 1 then
		self:canvas_text{
			dest_position = {0,0},
			dest_size = {w,h},
			text = tostring(self.count),
			text_alignment = {1,0.5},
			text_color = self.drag and {1,1,1,a} or self.focused and {1,1,0,a} or {1,1,1,a},
			text_font = self.font}
	end
	self:canvas_compile()
end

Widgets.ItemButton.pressed = function(self, args)
end
