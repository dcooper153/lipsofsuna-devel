local Class = require("system/class")
local UiButton = require("ui/widgets/button")

local UiTransition = Class("UiTransition", UiButton)

UiTransition.new = function(clss, label, state, changed)
	local self = UiButton.new(clss, label)
	self.hint = "$A: Enter\n$$B\n$$U\n$$D"
	self.target_state = state
	self.changed = changed
	return self
end

UiTransition.apply = function(self)
	self:changed()
	Ui:push_state(self.target_state)
	Main.effect_manager:play_global("uitransition1")
end

UiTransition.changed = function(self)
end

return UiTransition
