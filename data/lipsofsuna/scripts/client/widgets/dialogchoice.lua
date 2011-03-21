Widgets.DialogChoice = Class(Widgets.Label)

Widgets.DialogChoice.new = function(clss, args)
	local self = Widgets.Label.new(clss, args)
	self.font = "bigger"
	self:set_request{width = 400}
	return self
end

Widgets.DialogChoice.reshaped = function(self)
	local text = "> " .. (self.text or "")
	local wrap = self:get_request() or 400
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = text,
		width = wrap - 4}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {w,h},
		text = text,
		text_alignment = {self.halign,self.valign},
		text_color = self.focused and {0.7,0.3,0.3,1} or {0,0.7,0,1},
		text_font = self.font}
	self:canvas_compile()
end
