local StunModifier = Main.specs:find_by_name("ModifierSpec", "stun")

--- Restarts the modifier with a new value.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
StunModifier.restart = function(modifier, value)
	if not modifier.object then return end
	modifier.time = math.max(modifier.time or 0, value or 0)
	return true
end

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
StunModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.time = value or 5
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
StunModifier.update = function(modifier, secs)
	modifier.object.cooldown = math.max(modifier.cooldown or 0, 1)
	modifier.time = modifier.time - secs
	if modifier.time > 0 then return true end
end

--- Gets the duration of the modifier.
-- @param self Modifier.
-- @param attr Dictionary of attributes.
StunModifier.get_duration = function(modifier)
	if modifier.inventory then return end
	return modifier.time
end
