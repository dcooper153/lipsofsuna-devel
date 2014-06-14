local ModifierSpec = require("core/specs/modifier")

local BlessModifier = Main.specs:find_by_name("ModifierSpec", "bless")

--- Gets the attribute modifications of the modifier.
-- @param modifier Modifier.
-- @param attr Dictionary of attributes.
BlessModifier.attributes = function(modifier, attr)
	attr.max_health = attr.max_health + 20
	attr.max_willpower = attr.max_willpower + 20
end

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
BlessModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.strength = value
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
BlessModifier.update = function(modifier, secs)
	modifier.strength = modifier.strength - secs
	return modifier.strength > 0
end
