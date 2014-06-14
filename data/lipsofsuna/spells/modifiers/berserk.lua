-- Increase berserk duration.
local BerserkModifier = Main.specs:find_by_name("ModifierSpec", "berserk")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
BerserkModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.strength = value
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
BerserkModifier.update = function(modifier, secs)
	modifier.strength = modifier.strength - secs
	return modifier.strength > 0
end
