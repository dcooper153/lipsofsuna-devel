local ModifierSpec = require("core/specs/modifier")

local CureDiseaseModifier = ModifierSpec{
	name = "cure disease",
	categories =
	{
		["beneficial"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["self spell"] = true,
		["touch spell"] = true
	},
	description = "Cure the target of poison and diseases",
	effect = "spell1",
	icon = "modifier-heal", --FIXME
	influences = {["cure disease"] = 1},
	projectile = "fireball1",
	required_stats =
	{
		["willpower"] = 10
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
CureDiseaseModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.object:remove_modifier("black haze")
	modifier.object:remove_modifier("poison")
end
