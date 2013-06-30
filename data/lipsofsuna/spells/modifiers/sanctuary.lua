local ModifierSpec = require("core/specs/modifier")

-- Increase sanctuary duration.
local SanctuaryModifier = ModifierSpec{
	name = "sanctuary",
	actions =
	{
		["self spell"] = true,
		["touch spell"] = true
	},
	categories =
	{
		["beneficial"] = true,
		["spell"] = true
	},
	description = "Protect from death",
	effect = "sanctuary1",
	icon = "sanctuary1",
	influences = {["sanctuary"] = 30},
	required_stats =
	{
		["willpower"] = 20
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
SanctuaryModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.strength = value
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
SanctuaryModifier.update = function(modifier, secs)
	modifier.strength = modifier.strength - secs
	return modifier.strength > 0
end
