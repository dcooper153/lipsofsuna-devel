local ModifierSpec = require("core/specs/modifier")

local ResurrectModifier = Main.specs:find_by_name("ModifierSpec", "resurrect")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
ResurrectModifier.start = function(modifier, value)
	if not modifier.object then return end
	if modifier.object.class == Player and not modifier.object.client then
		modifier.owner:send_message("The corpse is devoid of life force...")
	else
		modifier.object:action("resurrect")
	end
end
