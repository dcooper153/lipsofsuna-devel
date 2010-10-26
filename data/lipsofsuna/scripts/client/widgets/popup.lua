Widgets.Popup = Class(Widget)

Widgets.Popup.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.margins = {5,5,5,5}
	return self
end

Widgets.Popup.reshaped = function(self)
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = {0,0},
		source_tiling = {6,52,6,6,52,6}}
	self:canvas_compile()
end
