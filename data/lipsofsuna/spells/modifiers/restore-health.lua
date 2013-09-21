local ModifierSpec = require("core/specs/modifier")

local RestoreHealthModifier = ModifierSpec:find_by_name("restore health")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
RestoreHealthModifier.start = function(modifier, value)
	if not modifier.object then return end
	-- Randomize the amount.
	local val = math.max(1, value + value * 0.5 * math.random())
	-- Apply the healing.
	modifier.object:damaged{amount = -val, point = modifier.point, type = "cold"}
end
