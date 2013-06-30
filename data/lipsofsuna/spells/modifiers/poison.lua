local ModifierSpec = require("core/specs/modifier")

local PoisonModifier = ModifierSpec{
	name = "poison",
	categories =
	{
		["harmful"] = true,
		["plague"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true
	},
	description = "The target takes poison damage over time",
	effect = "spell1",
	icon = "modifier-black haze", --FIXME
	modifiers = {["poison"] = 10},
	projectile = "fireball1",
	required_stats =
	{
		["willpower"] = 10
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
PoisonModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.strength = value
	modifier.timer = 0
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
PoisonModifier.update = function(modifier, secs)
	-- Update the burning timer.
	modifier.timer = modifier.timer + secs
	-- Damage the object every second.
	if modifier.timer > 2 then
		modifier.object:damaged{amount = math.random(2,4), type = "poison"}
		modifier.timer = modifier.timer - 2
	end
	-- End after the timeout.
	modifier.strength = modifier.strength - secs
	return modifier.strength > 0
end
