local FireElementalModifier = Main.specs:find_by_name("ModifierSpec", "fire elemental")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
FireElementalModifier.start = function(modifier, value)
	-- Find an empty ground spot.
	if not modifier.object then return end
	local ctr = Utils:find_summon_point(modifier.object:get_position())
	if not ctr then return end
	-- Create the summon.
	local summon = modifier.owner.manager:create_object("Actor", "fire elemental")
	if not summon then return end
	summon:set_position(ctr)
	summon:randomize()
	summon:set_visible(true)
	summon.summon_owner = modifier.owner
	summon.summon_timer = value
end
