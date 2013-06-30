-- Decrease health.
local FireDamageModifier = Feateffectspec{
	name = "fire damage",
	categories =
	{
		["fire"] = true,
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
	description = "Inflict fire damage",
	effect = "explosion1",
	icon = "modifier-fireball",
	influences = {["fire damage"] = 5},
	projectile = "fireball1",
	required_stats =
	{
		["willpower"] = 1
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
FireDamageModifier.start = function(modifier, value)
	if not modifier.object then return end
	-- Randomize the amount.
	local val = math.max(1, value + value * 0.5 * math.random())
	-- Apply unless friendly fire.
	if not modifier.owner.client or not modifier.object.client then
		modifier.object:damaged{amount = val, point = modifier.point, type = "fire"}
	end
	-- Anger hurt actors.
	if val > 0 then
		modifier.object:add_enemy(modifier.owner)
	end
end
