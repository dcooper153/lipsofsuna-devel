local ModifierSpec = require("core/specs/modifier")

local RestoreWillpowerModifier = ModifierSpec:find_by_name("restore willpower")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
RestoreWillpowerModifier.start = function(modifier, value)
	if not modifier.object then return end
	-- Randomize the amount.
	local val = math.max(1, value + value * 0.5 * math.random())
	-- Apply the restoration.
	local value = modifier.object.stats:get_value("willpower")
	if not value then return end
	modifier.object.stats:set_value("willpower", value + val)
end
