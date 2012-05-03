Operators.spells = Class()
Operators.spells.data = {slot = 1}

--- Resets the spell editor.
--
-- Context: Any.
--
-- @param self Operator.
Operators.spells.reset = function(self)
	self.data.slot = 1
	self.data.effect = nil
end

--- Gets the index of the currently edited spell.
--
-- Context: The spell editing mode must have been initialized.
--
-- @param self Operator.
-- @return Spell index.
Operators.spells.get_spell_index = function(self, value)
	return self.data.slot
end

--- Sets the index of the currently edited spell.
--
-- Context: The spell editing mode must have been initialized.
--
-- @param self Operator.
-- @param value Spell index.
Operators.spells.set_spell_index = function(self, value)
	self.data.slot = value
end

--- Gets the spell in the current slot.
--
-- Context: The spell editing mode must have been initialized, and the spell
-- index must have been set.
--
-- @param self Operator.
-- @return Spell.
Operators.spells.get_spell = function(self)
	local feat = Quickslots.feats.buttons[self.data.slot].feat
	if feat then return feat end
	return Feat{animation = "ranged spell"}
end

--- Assigns a spell to the given slot.
--
-- Context: The spell editing mode must have been initialized, and the spell
-- index must have been set.
--
-- @param self Operator.
-- @param type Spell type.
-- @param effects Spell effects.
Operators.spells.set_spell = function(self, type, effects)
	local feat = Feat{animation = type, effects = effects}
	Quickslots:assign_feat(self.data.slot, feat)
end

--- Gets the list of spells in all slots.
--
-- Context: The spell editing mode must have been initialized, and the spell
-- index must have been set.
--
-- @param self Operator.
-- @return List of spell.
Operators.spells.get_spells = function(self)
	local res = {}
	for k,v in ipairs(Quickslots.feats.buttons) do
		res[k] = v.feat or false
	end
	return res
end

--- Sets the index of the currently edited spell effect.
--
-- Context: The spell editing mode must have been initialized, and the spell
-- index must have been set.
--
-- @param self Operator.
-- @param index Slot number.
Operators.spells.set_effect_index = function(self, index)
	self.data.effect = index
end

--- Sets the effect in the active effect index of the active spells.
--
-- Context: The spell editing mode must have been initialized, the spell index
-- must have been set, and the spell effect index must have been set.
--
-- @param self Operator.
-- @param value Spell effect name.
Operators.spells.set_effect = function(self, value)
	local spell = self:get_spell()
	spell.effects[self.data.effect] = value
	self:set_spell(spell.animation, spell.effects)
end
