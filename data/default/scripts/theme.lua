Widgets.Background = Class(Widget)
Widgets.Background.class_name = "Widgets.Background"

Widgets.Background.new = function(clss, args)
	local self = Widget.new(clss, args)
	return self
end

Widgets.Background.reshaped = function(self)
	self:set_request{
		internal = true,
		width = 0,
		height = 0}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	if self.image then
		self:canvas_image{
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = self.image,
			source_position = self.source_offset}
	end
	self:canvas_compile()
end

Widgets.Background:add_getters{
	image = function(s) return rawget(s, "__image") end}

Widgets.Background:add_setters{
	image = function(s, v)
		if s.image == v then return end
		rawset(s, "__image", v)
		s:reshaped()
	end}

Widgets:add_font_style("default", "serif", 14)
Widgets:add_font_style("bigger", "serif", 18)
Widgets:add_font_style("medium", "serif", 24)
Widgets:add_font_style("mainmenu", "serif", 64)
