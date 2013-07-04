local ModifierSpec = require("core/specs/modifier")

local BurningModifier = ModifierSpec{
	name = "burning",
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
	description = "Inflict damage over time by putting the target on fire",
	effect = "berserk1",
	icon = "firewall", -- FIXME
	spell_strength = 10,
	required_stats =
	{
		["willpower"] = 5
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
BurningModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.strength = value
	modifier.timer = 0
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
BurningModifier.update = function(modifier, secs)
	-- Update the burning timer.
	modifier.timer = modifier.timer + secs
	-- Damage the object every second.
	if modifier.timer > 1 then
		modifier.object:damaged{amount = math.random(4,7), type = "burning"}
		modifier.timer = modifier.timer - 1
	end
	-- End after the timeout.
	modifier.strength = modifier.strength - secs
	return modifier.strength > 0
end
