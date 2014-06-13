local Class = require("system/class")
require("ui/widgets/button")

local UiTransition = Class("UiTransition", Widgets.Uibutton)

UiTransition.new = function(clss, label, state, changed)
	local self = Widgets.Uibutton.new(clss, label)
	self.hint = "$A: Enter\n$$B\n$$U\n$$D"
	self.target_state = state
	self.changed = changed
	return self
end

UiTransition.apply = function(self)
	self:changed()
	Ui:push_state(self.target_state)
	Client.effects:play_global("uitransition1")
end

UiTransition.changed = function(self)
end

return UiTransition
