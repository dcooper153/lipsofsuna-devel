Widgets.Uiicon = Class(Widget)
Widgets.Uiicon.DEFAULT = {image="icons1", offset={0,0}, size={34,34}}

Widgets.Uiicon.new = function(clss, icon, pos, rot, tooltip)
	local self = Widget.new(clss)
	self.icon = icon
	if pos then self.offset = pos end
	if rot then self.rotation = rot end
	self.tooltip = tooltip
	return self
end

Widgets.Uiicon.reshaped = function(self)
	local icon = self.icon or self.DEFAULT
	self:set_request{
		internal = true,
		width = icon.size[1],
		height = icon.size[2]}
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = icon.size,
		rotation = self.rotation,
		rotation_center = Vector(icon.size[1],icon.size[2])*0.5,
		source_image = icon.image,
		source_position = icon.offset,
		source_tiling = {0,icon.size[1],0,0,icon.size[2],0}}
end
