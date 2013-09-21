local ModifierSpec = require("core/specs/modifier")

-- Decrease health.
local ColdDamageModifier = ModifierSpec:find_by_name("cold damage")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
ColdDamageModifier.start = function(modifier, value)
	if not modifier.object then return end
	-- Randomize the amount.
	local val = math.max(1, value + value * 0.5 * math.random())
	-- Apply unless friendly fire.
	if not modifier.owner.client or not modifier.object.client then
		modifier.object:damaged{amount = val, point = modifier.point, type = "cold"}
	end
	-- Anger hurt actors.
	if val > 0 then
		modifier.object:add_enemy(modifier.owner)
	end
end
