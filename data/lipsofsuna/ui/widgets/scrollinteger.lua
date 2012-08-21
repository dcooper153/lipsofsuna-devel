local Class = require("system/class")
require(Mod.path .. "scrollfloat")

Widgets.Uiscrollinteger = Class("Uiscrollinteger", Widgets.Uiscrollfloat)

Widgets.Uiscrollinteger.new = function(clss, label, min, max, value, changed)
	local self = Widgets.Uiscrollfloat.new(clss, label, min, max, value, changed)
	self.text = tostring(value)
	self.step = 1
	return self
end

Widgets.Uiscrollinteger.get_value_at = function(self, x)
	return math.floor(Widgets.Uiscrollfloat.get_value_at(self, x) + 0.5)
end

Widgets.Uiscrollinteger.update_text = function(self)
	self.text = tostring(self.value)
end
