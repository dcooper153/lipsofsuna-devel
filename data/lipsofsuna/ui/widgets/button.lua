local Class = require("system/class")
local UiWidget = require("ui/widgets/widget")

Widgets.Uibutton = Class("Uibutton", UiWidget)

Widgets.Uibutton.new = function(clss, label, apply)
	local self = UiWidget.new(clss)
	self.apply = apply
	self.hint = "$A: Activate\n$$B\n$$U\n$$D"
	self.text = label
	return self
end

Widgets.Uibutton.apply = function(self)
end

Widgets.Uibutton.rebuild_size = function(self)
	-- Get the base size.
	local size = UiWidget.rebuild_size(self)
	-- Resize to fit the label.
	local w,h = Theme:measure_button(self.text, Theme.width_widget_1-10)
	size.x = math.max(size.x, w + 10)
	size.y = math.max(size.y, h + 10)
	return size
end

Widgets.Uibutton.rebuild_canvas = function(self)
	Theme:draw_button(self, self.text,
		0, 0, self.size.x, self.size.y,
		self.focused, false)
end

return Widgets.Uibutton
