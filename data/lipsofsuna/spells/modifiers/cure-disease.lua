local ModifierSpec = require("core/specs/modifier")

local CureDiseaseModifier = ModifierSpec:find_by_name("cure disease")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
CureDiseaseModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.object:remove_modifier("black haze")
	modifier.object:remove_modifier("poison")
end
