require "client/widgets/label"

Widgets.DialogLabel = Class(Widgets.Label)

Widgets.DialogLabel.new = function(clss, args)
	local self = Widgets.Label.new(clss, args)
	self.font = "medium"
	self:set_request{width = 800}
	return self
end

Widgets.DialogLabel.reshaped = function(self)
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = self.text,
		width = self:get_request()}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_text{
		dest_position = {2,2},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {self.halign,self.valign},
		text_color = {0,0,0,1},
		text_font = self.font}
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {self.halign,self.valign},
		text_color = self.focused and {1,1,0,1} or {1,1,1,1},
		text_font = self.font}
	self:canvas_compile()
end
