Widgets.Button = Class(Widget)
Widgets.Button.class_name = "Widgets.Button"

Widgets.Button.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.font = self.font or "default"
	self.text = self.text or ""
	return self
end

Widgets.Button.pressed = function(self)
end

Widgets.Button.reshaped = function(self)
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {3,6,6,3},
		text = self.text}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = self.focused and {350,415} or {350,375},
		source_tiling = {12,64,12,11,14,13}}
	self:canvas_text{
		dest_position = {0,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {0.5,0.5},
		text_color = self.focused and {1,1,0,1} or {1,1,1,1},
		text_font = self.font}
	self:canvas_compile()
end

Widgets.Button:add_getters{
	focused = function(s) return rawget(s, "__focused") end,
	font = function(s) return rawget(s, "__font") end,
	text = function(s) return rawget(s, "__text") end}

Widgets.Button:add_setters{
	focused = function(s, v)
		if s.focused == v then return end
		rawset(s, "__focused", v)
		s:reshaped()
	end,
	font = function(s, v)
		if s.font == v then return end
		rawset(s, "__font", v)
		s:reshaped()
	end,
	text = function(s, v)
		if s.text == v then return end
		rawset(s, "__text", v)
		s:reshaped()
	end}
