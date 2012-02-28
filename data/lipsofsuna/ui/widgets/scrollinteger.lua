require(Mod.path .. "scrollfloat")

Widgets.Uiscrollinteger = Class(Widgets.Uiscrollfloat)
Widgets.Uiscrollinteger.class_name = "Widgets.Uiscrollinteger"

Widgets.Uiscrollinteger.new = function(clss, label, min, max, value, changed)
	local self = Widgets.Uiscrollfloat.new(clss, label, min, max, value, changed)
	self.text = tostring(value)
	self.step = 1
	return self
end

Widgets.Uiscrollinteger.get_value_at = function(self, x)
	return math.floor(Widgets.Uiscrollfloat.get_value_at(self, x) + 0.5)
end

Widgets.Uiscrollinteger.set_value_at = function(self, x)
	self.value = self:get_value_at(x)
	self.text = tostring(self.value)
	self.need_repaint = true
	self:changed()
end
