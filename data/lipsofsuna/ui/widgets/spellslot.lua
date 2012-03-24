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
		local slot = Client.data.spells.slot
		local spell = Client:get_spell(slot)
		spell.effects[self.index] = nil
		Client:set_spell(slot, spell.animation, spell.effects)
		Ui:restart_state()
	else
		-- Chose a new effect.
		Client.data.spells.effect = self.index
		Ui:push_state("spells/effects")
	end
end
