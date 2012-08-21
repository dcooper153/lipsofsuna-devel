local Class = require("system/class")
require(Mod.path .. "button")

Widgets.Uitransition = Class("Uitransition", Widgets.Uibutton)

Widgets.Uitransition.new = function(clss, label, state, changed)
	local self = Widgets.Uibutton.new(clss, label)
	self.hint = "$A: Enter\n$$B\n$$U\n$$D"
	self.target_state = state
	self.changed = changed
	return self
end

Widgets.Uitransition.apply = function(self)
	self:changed()
	Ui:push_state(self.target_state)
	Client.effects:play_global("uitransition1")
end

Widgets.Uitransition.changed = function(self)
end
