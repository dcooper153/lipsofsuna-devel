local ResurrectModifier = Feateffectspec{
	name = "resurrect",
	categories =
	{
		["beneficial"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["touch spell"] = true
	},
	description = "Resurrect the targeted creature",
	effect = "heal1",
	icon = "modifier-heal",
	influences = {["resurrect"] = 1},
	required_stats =
	{
		["willpower"] = 20
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
ResurrectModifier.start = function(modifier, value)
	if not modifier.object then return end
	if modifier.object.class == Player and not modifier.object.client then
		modifier.owner:send_message("The corpse is devoid of life force...")
	else
		modifier.object:action("resurrect")
	end
end
