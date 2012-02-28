require(Mod.path .. "spell")

Widgets.Uispellslot = Class(Widgets.Uispell)
Widgets.Uispellslot.class_name = "Widgets.Uispellslot"

Widgets.Uispellslot.new = function(clss, name, index)
	local self = Widgets.Uispell.new(clss, name, true)
	self.index = index
	self.hint = self.effect and "$A: Clear\n$$B\n$$U\n$$D" or "$A: Select\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uispellslot.apply = function(self)
	if self.effect then
		-- Clear the existing effect.
		Client.views.feats:set_effect(self.index)
		Ui:restart_state()
	else
		-- Chose a new effect.
		Client.views.feats.select_effect_slot = self.index
		Ui:push_state("spells/effects")
	end
end
