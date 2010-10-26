Widgets.Equipment = Class(Widget)

Widgets.Equipment.new = function(clss, args)
	local self = Widget.new(clss, args)
	self:set_request{
		internal = true,
		width = 200,
		height = 165}
	return self
end

Widgets.Equipment.reshaped = function(self)
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "equipment1",
		source_position = {56,0},
		source_tiling = {0,256,0,0,256,0}}
	self:canvas_compile()
end
