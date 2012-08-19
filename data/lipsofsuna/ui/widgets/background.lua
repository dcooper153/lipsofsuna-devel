Widgets.Uibackground = Class(Widget)

Widgets.Uibackground.new = function(clss, image)
	local self = Widget.new(clss)
	self.fullscreen = true
	self.image = image
	return self
end

Widgets.Uibackground.reshaped = function(self)
	self:set_request{
		internal = true,
		width = 0,
		height = 0}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	if self.image then
		local tw,th = 1024,1024
		if w > h then
			local hh = th*h/w
			self:canvas_image{
				dest_position = {0,0},
				dest_size = {w,h},
				source_image = self.image,
				source_position = {0,(th-hh)/2},
				source_tiling = {-1,tw,-1,-1,hh,-1}}
		else
			local ww = tw*w/h
			self:canvas_image{
				dest_position = {0,0},
				dest_size = {w,h},
				source_image = self.image,
				source_position = {(tw-ww)/2,0},
				source_tiling = {-1,ww,-1,-1,th,-1}}
		end
	end
end
