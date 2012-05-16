require(Mod.path .. "widget")

Widgets.Uitoggle = Class(Widgets.Uiwidget)
Widgets.Uitoggle.class_name = "Widgets.Uitoggle"

Widgets.Uitoggle.new = function(clss, label, changed)
	local self = Widgets.Uiwidget.new(clss, nil, label)
	self.value = false
	self.changed = changed
	self.hint = "$A: Toggle\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uitoggle.apply = function(self)
	self.value = not self.value
	self.need_repaint = true
	self:changed()
	Effect:play_global(self.value and "uitoggle1" or "uitoggle2")
end

Widgets.Uitoggle.changed = function(self)
end

Widgets.Uitoggle.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the label.
	local w,h = Theme:measure_button("Yes", Theme.width_widget_1-10)
	size.x = math.max(size.x, w + 10)
	size.y = math.max(size.y, h + 10)
	return size
end

Widgets.Uitoggle.rebuild_canvas = function(self)
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the button.
	Theme:draw_button(self,
		self.value and "Yes" or "No",
		Theme.width_label_1, 3, self.size.x-Theme.width_label_1-5, self.size.y-6,
		self.focused, self.value)
end
