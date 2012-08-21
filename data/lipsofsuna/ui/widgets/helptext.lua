local Class = require("system/class")
require(Mod.path .. "label")

Widgets.Uihelptext = Class("Uihelptext", Widgets.Uilabel)

Widgets.Uihelptext.new = function(clss, text)
	local self = Widgets.Uilabel.new(clss, text)
	self.hint = "$$B\n$$U\n$$D"
	return self
end
