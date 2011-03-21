require "system/widgets/label"

Widgets.MenuItem = Class(Widgets.Label)

Widgets.MenuItem.setter = function(self, key, value)
	if key == "arrow" then
		if self.arrow ~= value then
			Widgets.Label.setter(self, key, value)
			self:reshaped()
		end
	else
		Widgets.Label.setter(self, key, value)
	end
end

Widgets.MenuItem.new = function(clss, args)
	local self = Widgets.Label.new(clss, args)
	self.color = args.color or {1,1,1,1}
	self.color_focus = args.color_focus or {1,1,0,1}
	return self
end

Widgets.MenuItem.reshaped = function(self)
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = (self.text or "") .. "  >"}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {self.halign,self.valign},
		text_color = self.focused and self.color_focus or self.color,
		text_font = self.font}
	if self.arrow then
		self:canvas_text{
			dest_position = {0,0},
			dest_size = {w,h},
			text = ">",
			text_alignment = {1,self.valign},
			text_color = self.focused and self.color_focus or self.color,
			text_font = self.font}
	end
	self:canvas_compile()
end
