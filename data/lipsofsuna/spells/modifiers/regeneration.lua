local ModifierSpec = require("core/specs/modifier")

-- Increase sanctuary duration.
local RegenerationModifier = ModifierSpec{
	name = "regeneration",
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
	description = "Regenerate health every second",
	effect = "berserk1",
	icon = "modifier-heal",
	spell_strength = 60,
	required_stats =
	{
		["willpower"] = 15
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
RegenerationModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.timer = 0
	modifier.strength = value
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
RegenerationModifier.update = function(modifier, secs)
	-- Update the timer.
	modifier.timer = modifier.timer + secs
	-- Heal the object every second.
	if modifier.timer > 1 then
		modifier.object:damaged{amount = -math.random(2,4), type = "physical"}
		modifier.timer = modifier.timer - 1
	end
	-- End after the timeout.
	modifier.strength = modifier.strength - secs
	return modifier.strength > 0
end
