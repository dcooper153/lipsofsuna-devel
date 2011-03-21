Widgets.DialogLabel = Class(Widgets.Label)

Widgets.DialogLabel.new = function(clss, args)
	local self = Widgets.Label.new(clss, args)
	self.font = "bigger"
	self:set_request{width = 400}
	return self
end

Widgets.DialogLabel.reshaped = function(self)
	local wrap = self:get_request() or 400
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = self.text,
		width = wrap - 4}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {self.halign,self.valign},
		text_color = self.focused and {0.7,0.3,0.3,1} or {0,0,0,1},
		text_font = self.font}
	self:canvas_compile()
end
