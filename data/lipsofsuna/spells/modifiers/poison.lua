local PoisonModifier = Main.specs:find_by_name("ModifierSpec", "poison")

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
