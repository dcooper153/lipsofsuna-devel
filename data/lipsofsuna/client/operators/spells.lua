Operators.spells = Client

--- Gets the index of the currently edited spell.
--
-- Context: The spell editing mode must have been initialized.
--
-- @param self Client.
-- @return Spell index.
Operators.spells.get_spell_index = function(self, value)
	return self.data.spells.slot
end

--- Sets the index of the currently edited spell.
--
-- Context: The spell editing mode must have been initialized.
--
-- @param self Client.
-- @param value Spell index.
Operators.spells.set_spell_index = function(self, value)
	self.data.spells.slot = value
end

--- Gets the spell in the given slot.
--
-- Context: The spell editing mode must have been initialized, and the spell
-- index must have been set.
--
-- @param self Client.
-- @param slot Slot number.
-- @return Spell.
Operators.spells.get_spell = function(self)
	local feat = Quickslots.feats.buttons[self.data.spells.slot].feat
	if feat then return feat end
	return Feat{animation = "ranged spell"}
end

--- Assigns a spell to the given slot.
--
-- Context: The spell editing mode must have been initialized, and the spell
-- index must have been set.
--
-- @param self Client.
-- @param type Spell type.
-- @param effects Spell effects.
Operators.spells.set_spell = function(self, type, effects)
	local feat = Feat{animation = type, effects = effects}
	Quickslots:assign_feat(self.data.spells.slot, feat)
end

--- Sets the index of the currently edited spell effect.
--
-- Context: The spell editing mode must have been initialized, and the spell
-- index must have been set.
--
-- @param self Client.
-- @param slot Slot number.
Operators.spells.set_effect_index = function(self, index)
	self.data.spells.effect = index
end

--- Sets the effect in the active effect index of the active spells.
--
-- Context: The spell editing mode must have been initialized, the spell index
-- must have been set, and the spell effect index must have been set.
--
-- @param self Client.
-- @param value Spell effect name.
Operators.spells.set_effect = function(self, value)
	local spell = self:get_spell()
	spell.effects[self.data.spells.effect] = value
	self:set_spell(spell.animation, spell.effects)
end
