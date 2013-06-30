-- Dig terrain.
local DigModifier = Feateffectspec{
	name = "dig",
	categories =
	{
		["dig"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true
	},
	description = "Fire a digging ray",
	effect = "spell1",
	icon = "modifier-earthmove",
	influences = {["dig"] = 1},
	projectile = "fireball1", -- FIXME
	required_stats =
	{
		["willpower"] = 3
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
DigModifier.start = function(modifier, value)
	-- TODO
end
