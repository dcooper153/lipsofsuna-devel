require "client/widgets/iconbutton"

Widgets.ItemButton = Class(Widgets.IconButton)

Widgets.ItemButton.setter = function(self, key, value)
	if key == "count" then
		if self.count ~= value then
			Widgets.IconButton.setter(self, key, value)
			self:reshaped()
		end
	else
		Widgets.IconButton.setter(self, key, value)
	end
end

Widgets.ItemButton.new = function(clss, args)
	local self = Widgets.IconButton.new(clss, args)
	return self
end

Widgets.ItemButton.reshaped = function(self)
	self:set_request{
		internal = true,
		height = 20,
		width = 150}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	local icon = Iconspec:find{name = self.icon}
	if icon then
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {20,20},
			source_image = icon.image,
			source_position = icon.offset,
			source_tiling = {0,icon.size[1],0,0,icon.size[1],0}}
	end
	if self.text then
		self:canvas_text{
			dest_position = {22,0},
			dest_size = {w,h},
			text = self.text,
			text_alignment = {0,0.5},
			text_color = self.focused and {1,1,0,1} or {1,1,1,1},
			text_font = self.font}
	end
	if self.count and self.count > 1 then
		self:canvas_text{
			dest_position = {0,0},
			dest_size = {w,h},
			text = tostring(self.count),
			text_alignment = {1,0.5},
			text_color = self.focused and {1,1,0,1} or {1,1,1,1},
			text_font = self.font}
	end
	self:canvas_compile()
end

Widgets.ItemButton.pressed = function(self, args)
end
