local ModifierSpec = require("core/specs/modifier")

local HomeModifier = ModifierSpec{
	name = "home",
	categories =
	{
		["follow"] = true,
		["spell"] = true
	},
	actions =
	{
		["self spell"] = true
	},
	description = "Creates a home location where you can respawn from",
	effect = "light1",
	icon = "sanctuary1",
	influences = {["home"] = 1},
	required_stats =
	{
		["willpower"] = 1
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
HomeModifier.start = function(modifier, value)
	if not modifier.object then return end
	if not modifier.object.account then return end
	local home = modifier.object:get_position()
	modifier.object.account.spawn_point = home
	Main.messaging:server_event("create-marker", "home", home)
end
