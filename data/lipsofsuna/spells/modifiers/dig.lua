local ModifierSpec = require("core/specs/modifier")

-- Dig terrain.
local DigModifier = ModifierSpec:find_by_name("dig")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
DigModifier.start = function(modifier, value)
	-- TODO
end
