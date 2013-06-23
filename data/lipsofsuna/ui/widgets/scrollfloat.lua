local Class = require("system/class")
local Input = require("system/input")
require(Mod.path .. "widget")

Widgets.Uiscrollfloat = Class("Uiscrollfloat", Widgets.Uiwidget)

Widgets.Uiscrollfloat.new = function(clss, label, min, max, value, changed)
	local self = Widgets.Uiwidget.new(clss, label)
	self.min = min
	self.max = max
	self.value = value
	self.changed = changed
	self.step = (max - min) / 10
	self.hint = "$R: Increment\n$L: Decrement\n$$U\n$$D"
	return self
end

Widgets.Uiscrollfloat.left = function(self)
	if self.value == self.min then return true end
	self.value = math.max(self.value - self.step, self.min)
	self.need_repaint = true
	self:update_text()
	self:changed()
	Client.effects:play_global("uislider1")
end

Widgets.Uiscrollfloat.right = function(self)
	if self.value == self.max then return end
	self.value = math.min(self.value + self.step, self.max)
	self.need_repaint = true
	self:update_text()
	self:changed()
	Client.effects:play_global("uislider1")
end

Widgets.Uiscrollfloat.changed = function(self)
end

Widgets.Uiscrollfloat.rebuild_canvas = function(self)
	local w = self.size.x - Theme.width_label_1 - 5
	local h = self.size.y - 10
	local v = (self.value - self.min) / (self.max - self.min)
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the background.
	Theme:draw_slider(self, v,
		Theme.width_label_1, 5, w, h,
		self.focused, self.focused)
	-- Add the label of the bar.
	if self.text then
		self:canvas_text{
			dest_position = {Theme.width_label_1,Theme.text_pad_1+5},
			dest_size = {w,h},
			text = self.text,
			text_alignment = {0.5,0.5},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_1}
	end
end

Widgets.Uiscrollfloat.update_text = function(self)
end

Widgets.Uiscrollfloat.get_value_at = function(self, x)
	local pos = math.max(0, x - self:get_x() - Theme.width_label_1 - Theme.width_slider_button_1)
	local frac = math.min(1, pos / (self.size.x - Theme.width_label_1 - Theme.width_slider_button_1 * 2 - 5))
	return frac * (self.max - self.min) + self.min
end

Widgets.Uiscrollfloat.set_value_at = function(self, x)
	self.value = self:get_value_at(x)
	self.need_repaint = true
	self:update_text()
	self:changed()
end
