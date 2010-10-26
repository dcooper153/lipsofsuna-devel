Widgets.Icon = Class(Widget)
Widgets.Icon.DEFAULT = { image="widgets1", offset={0,0}, size={16,16} }

Widgets.Icon.setter = function(self, key, value)
	if key == "icon" then
		if self.icon ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	else
		Widget.setter(self, key, value)
	end
end

Widgets.Icon.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.icon = self.icon or clss.DEFAULT
	return self
end

Widgets.Icon.reshaped = function(self)
	local icon = self.icon or self.DEFAULT
	self:set_request{
		internal = true,
		width = icon.size[1],
		height = icon.size[2]}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = icon.size,
		source_image = icon.image,
		source_position = icon.offset,
		source_tiling = {0,icon.size[1],0,0,icon.size[1],0}}
	self:canvas_compile()
end

Widgets.Icon.pressed = function(self, args)
end
