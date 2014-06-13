local Class = require("system/class")
local UiWidget = require("ui/widgets/widget")

local UiToggle = Class("UiToggle", UiWidget)

UiToggle.new = function(clss, label, changed)
	local self = UiWidget.new(clss, label)
	self.value = false
	self.changed = changed
	self.hint = "$A: Toggle\n$$B\n$$U\n$$D"
	return self
end

UiToggle.apply = function(self)
	self.value = not self.value
	self.need_repaint = true
	self:changed()
	Main.effect_manager:play_global(self.value and "uitoggle1" or "uitoggle2")
end

UiToggle.changed = function(self)
end

UiToggle.rebuild_size = function(self)
	-- Get the base size.
	local size = UiWidget.rebuild_size(self)
	-- Resize to fit the label.
	local w,h = Theme:measure_button("Yes", Theme.width_widget_1-10)
	size.x = math.max(size.x, w + 10)
	size.y = math.max(size.y, h + 10)
	return size
end

UiToggle.rebuild_canvas = function(self)
	-- Add the base.
	UiWidget.rebuild_canvas(self)
	-- Add the button.
	Theme:draw_button(self,
		self.value and "Yes" or "No",
		Theme.width_label_1, 3, self.size.x-Theme.width_label_1-5, self.size.y-6,
		self.focused, self.value)
end

return UiToggle
