local ModifierSpec = require("core/specs/modifier")

-- Increase light duration.
local LightModifier = ModifierSpec{
	name = "light",
	categories =
	{
		["beneficial"] = true,
		["light"] = true,
		["spell"] = true
	},
	actions =
	{
		["self spell"] = true
	},
	description = "Illuminate your surroundings",
	effect = "light1",
	icon = "modifier-light",
	modifiers = {["light"] = 60},
	required_stats =
	{
		["willpower"] = 5
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
LightModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.strength = value
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
LightModifier.update = function(modifier, secs)
	modifier.strength = modifier.strength - secs
	return modifier.strength > 0
end
