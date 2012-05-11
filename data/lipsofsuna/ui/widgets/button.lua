require(Mod.path .. "widget")

Widgets.Uibutton = Class(Widgets.Uiwidget)
Widgets.Uibutton.class_name = "Widgets.Uibutton"

Widgets.Uibutton.new = function(clss, label, apply)
	local self = Widgets.Uiwidget.new(clss)
	self.apply = apply
	self.hint = "$A: Activate\n$$B\n$$U\n$$D"
	self.text = label
	return self
end

Widgets.Uibutton.apply = function(self)
end

Widgets.Uibutton.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the label.
	local w,h = Theme:measure_button(self.text, Theme.width_widget_1-10)
	size.x = math.max(size.x, w + 10)
	size.y = math.max(size.y, h + 10)
	return size
end

Widgets.Uibutton.rebuild_canvas = function(self)
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the button.
	Theme:draw_button(self, self.text,
		5, 3, self.size.x-10, self.size.y-6,
		self.focused, false)
end
