local BurdeningModifier = Main.specs:find_by_name("ModifierSpec", "burdening")

--- Restarts the modifier with a new value.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
BurdeningModifier.restart = function(modifier, value)
	if not modifier.object then return end
	modifier.inventory = modifier.inventory or not value
	modifier.spell = math.max(modifier.spell or 0, value or 0)
	return true
end

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
BurdeningModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.inventory = not value
	modifier.spell = value or 0
	modifier.object:send_message("You're now burdened.")
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
BurdeningModifier.update = function(modifier, secs)
	-- Update spell burdening.
	if modifier.spell then
		modifier.spell = math.max(0, modifier.spell - secs)
	end
	-- Update inventory burdening.
	if modifier.inventory and modifier.object.carried_weight then
		modifier.inventory = (modifier.object.carried_weight > modifier.object:get_burden_limit())
	end
	-- Wait for both to end.
	if modifier.inventory then return true end
	if modifier.spell > 0 then return true end
	modifier.object:send_message("You're no longer burdened.")
end

--- Gets the duration of the modifier.
-- @param self Modifier.
-- @param attr Dictionary of attributes.
BurdeningModifier.get_duration = function(modifier)
	if modifier.inventory then return end
	return modifier.spell
end
