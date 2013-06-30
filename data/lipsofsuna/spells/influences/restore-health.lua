local RestoreHealthModifier = Feateffectspec{
	name = "restore health",
	categories =
	{
		["beneficial"] = true,
		["heal"] = true,
		["spell"] = true
	},
	actions =
	{
		["self spell"] = true,
		["touch spell"] = true
	},
	description = "Restore health",
	effect = "heal1",
	icon = "modifier-heal",
	influences = {["restore health"] = 15},
	required_stats =
	{
		["willpower"] = 5
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
RestoreHealthModifier.start = function(modifier, value)
	if not modifier.object then return end
	-- Randomize the amount.
	local val = math.max(1, value + value * 0.5 * math.random())
	-- Apply the healing.
	modifier.object:damaged{amount = -val, point = modifier.point, type = "cold"}
end
