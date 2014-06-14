-- Teleport to Lips.
local TravelModifier = Main.specs:find_by_name("ModifierSpec", "travel")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
TravelModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.object:teleport{region = "Lips"}
end
