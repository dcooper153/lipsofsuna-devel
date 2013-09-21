local ModifierSpec = require("core/specs/modifier")

-- Teleport to Lips.
local TravelModifier = ModifierSpec:find_by_name("travel")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
TravelModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.object:teleport{region = "Lips"}
end
