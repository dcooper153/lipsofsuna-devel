Widgets.Modifier = Class(Widget)

Widgets.Modifier.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.icon = self.icon or "missing1"
	return self
end

Widgets.Modifier.reshaped = function(self)
	self:set_request{
		internal = true,
		width = Theme.width_icon_1 + 2,
		height = Theme.width_icon_1 + Theme.text_height_1 + 2}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	Theme:draw_icon_scaled(self, self.icon, 0, 0, Theme.width_icon_1, Theme.width_icon_1)
	if self.time_coarse < 10000 then
		self:canvas_text{
			dest_position = {2,2},
			dest_size = {w,h},
			text = tostring(self.time_coarse),
			text_alignment = {0.5,1},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_1}
		self:canvas_text{
			dest_position = {0.5,0},
			dest_size = {w,h},
			text = tostring(self.time_coarse),
			text_alignment = {0.5,1},
			text_color = Theme.text_color_3,
			text_font = Theme.text_font_1}
	end
end

Widgets.Modifier.pressed = function(self, args)
end

Widgets.Modifier:add_getters{
	icon = function(self) return rawget(self, "__icon") end,
	name = function(self) return rawget(self, "__name") end,
	time = function(self) return rawget(self, "__time") end}

Widgets.Modifier:add_setters{
	icon = function(self, value)
		if self.icon == value then return end
		rawset(self, "__icon", value)
		self:reshaped()
	end,
	name = function(self, value)
		rawset(self, "__name", value)
		self.tooltip = Widgets.Tooltip()
		self.tooltip:append(Widgets.Label{text = value})
	end,
	time = function(self, value)
		rawset(self, "__time", value)
		local t = math.ceil((value + 0.1) / 5) * 5
		if t ~= self.time_coarse then
			self.time_coarse = t
			self:reshaped()
		end
	end}
