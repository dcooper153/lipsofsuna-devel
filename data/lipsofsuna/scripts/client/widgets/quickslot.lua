Widgets.Quickslot = Class(Widget)

Widgets.Quickslot.setter = function(self, key, value)
	if key == "focused" then
		if self.focused ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "icon" then
		if self.icon ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	else
		Widget.setter(self, key, value)
	end
end

Widgets.Quickslot.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.margins = {3,2,5,4}
	return self
end

Widgets.Quickslot.reshaped = function(self)
	local icon = self.icon or self.DEFAULT
	self:set_request{
		internal = true,
		width = 34+5,
		height = 34}
	local w = self.width
	local h = self.height + 9
	-- Background.
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,-9},
		dest_size = {w, h},
		source_image = "widgets1",
		source_position = {810,379},
		source_tiling = {7,25,7,8,28,7}}
	-- Icon.
	if self.icon then
		self:canvas_image{
			dest_position = {(w-icon.size[1])/2+1,(h-icon.size[2])/2-9},
			dest_size = icon.size,
			source_image = icon.image,
			source_position = icon.offset,
			source_tiling = {0,icon.size[1],0,0,icon.size[1],0}}
	end
	-- Glass.
	if not self.focused then
		self:canvas_image{
			dest_position = {0,-9},
			dest_size = {w, h},
			source_image = "widgets1",
			source_position = {860,379},
			source_tiling = {7,25,7,8,28,7}}
	end
	self:canvas_compile()
end

Widgets.Quickslot.pressed = function(self, args)
end
