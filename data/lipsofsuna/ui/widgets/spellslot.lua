require(Mod.path .. "spell")

Widgets.Uispellslot = Class(Widgets.Uispell)
Widgets.Uispellslot.class_name = "Widgets.Uispellslot"

Widgets.Uispellslot.new = function(clss, mode, name, index)
	local self = Widgets.Uispell.new(clss, mode, name, true)
	self.mode = mode
	self.index = index
	self.hint = self.spec and "$A: Clear\n$$B\n$$U\n$$D" or "$A: Select\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uispellslot.apply = function(self)
	if self.mode == "type" then
		-- Choose a new spell type.
		Ui:push_state("spells/types")
	elseif self.spec then
		-- Clear the existing effect.
		Operators.spells:set_effect_index(self.index)
		Operators.spells:set_effect()
		Ui:restart_state()
	else
		-- Chose a new effect.
		Operators.spells:set_effect_index(self.index)
		Ui:push_state("spells/effects")
	end
	Client.effects:play_global("uitransition1")
end
