local ModifierSpec = require("core/specs/modifier")

local CurseModifier = ModifierSpec{
	name = "curse",
	categories =
	{
		["harmful"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["self spell"] = true,
		["touch spell"] = true
	},
	description = "Damages the maximum health and willpower of the target",
	effect = "spell1",
	icon = "firewall", --FIXME
	influences = {["curse"] = 60},
	projectile = "fireball1",
	required_stats =
	{
		["willpower"] = 10
	}}

--- Gets the attribute modifications of the modifier.
-- @param modifier Modifier.
-- @param attr Dictionary of attributes.
CurseModifier.attributes = function(modifier, attr)
	attr.max_health = attr.max_health - 20
	attr.max_willpower = attr.max_willpower - 20
end

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
CurseModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.strength = value
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
CurseModifier.update = function(modifier, secs)
	modifier.strength = modifier.strength - secs
	return modifier.strength > 0
end
