local ModifierSpec = require("core/specs/modifier")

-- Teleport to Lips.
local TravelModifier = ModifierSpec{
	name = "travel",
	actions =
	{
		["self spell"] = true
	},
	categories =
	{
		["spell"] = true
	},
	description = "Fast travel to Lips",
	effect = "sanctuary1", -- FIXME
	icon = "sanctuary1", -- FIXME
	spell_strength = 30,
	required_stats =
	{
		["willpower"] = 10
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
TravelModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.object:teleport{region = "Lips"}
end
