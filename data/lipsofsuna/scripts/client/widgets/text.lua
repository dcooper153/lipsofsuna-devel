require "client/widgets/frame"

Widgets.Text = Class(Widgets.Frame)

Widgets.Text.setter = function(self, key, value)
	if key == "color" then
		if self.color ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "font" then
		if self.font ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "text" then
		if type(value) == "string" then
			Widget.setter(self, key, {{value}})
		else
			Widget.setter(self, key, value)
		end
		self:build()
		self:reshaped()
	else
		Widgets.Frame.setter(self, key, value)
	end
end

Widgets.Text.new = function(clss, args)
	local arg = function(n, d) return args and args[n] or d end
	local self = Widgets.Frame.new(clss, {cols = 1, rows = 0, margins = {30,30,40,30}, style = "paper"})
	self.color = arg("color", {0,0,0,1})
	self.font = arg("font", "default")
	self.halign = arg("halign", 0)
	self.valign = arg("valign", 0.5)
	self.text = arg("text", {})
	return self
end

Widgets.Text.build = function(self)
	self.rows = 0
	if self.valign > 0 then
		self:append_row()
		self:set_expand{row = 1}
	end
	for k,v in ipairs(self.text) do
		local w = Widgets.Label{
			color = self.color,
			font = v[2] or self.font,
			text = v[1],
			valign = self.valign}
		w:set_request{width = 270}
		self:append_row(w)
	end
	if self.valign < 1 then
		self:append_row()
		self:set_expand{row = self.rows}
	end
end
