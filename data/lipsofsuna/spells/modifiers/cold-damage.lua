local ModifierSpec = require("core/specs/modifier")

-- Decrease health.
local ColdDamageModifier = ModifierSpec{
	name = "cold damage",
	categories =
	{
		["cold"] = true,
		["harmful"] = true,
		["melee"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["self spell"] = true,
		["touch spell"] = true
	},
	description = "Inflict cold damage",
	effect = "explosion1", -- FIXME
	icon = "modifier-ice",
	projectile = "fireball1", -- FIXME
	spell_strength = 5,
	required_stats =
	{
		["willpower"] = 1
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
ColdDamageModifier.start = function(modifier, value)
	if not modifier.object then return end
	-- Randomize the amount.
	local val = math.max(1, value + value * 0.5 * math.random())
	-- Apply unless friendly fire.
	if not modifier.owner.client or not modifier.object.client then
		modifier.object:damaged{amount = val, point = modifier.point, type = "cold"}
	end
	-- Anger hurt actors.
	if val > 0 then
		modifier.object:add_enemy(modifier.owner)
	end
end
