require(Mod.path .. "button")

Widgets.Uitransition = Class(Widgets.Uibutton)
Widgets.Uitransition.class_name = "Widgets.Uitransition"

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
end

Widgets.Uitransition.changed = function(self)
end
