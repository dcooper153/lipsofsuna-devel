require(Mod.path .. "label")

Widgets.Uihelptext = Class(Widgets.Uilabel)
Widgets.Uihelptext.class_name = "Widgets.Uihelptext"

Widgets.Uihelptext.new = function(clss, text)
	local self = Widgets.Uilabel.new(clss, text)
	self.hint = "$$B\n$$U\n$$D"
	return self
end
