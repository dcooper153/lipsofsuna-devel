local ModifierSpec = require("core/specs/modifier")

local MindlessMarchModifier = Main.specs:find_by_name("ModifierSpec", "mindless march")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
MindlessMarchModifier.start = function(modifier, value)
	if not modifier.object then return end
	if modifier.object.spec.type ~= "actor" then return end
	modifier.timer = 0
	modifier.strength = value
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
MindlessMarchModifier.update = function(modifier, secs)
	modifier.object:set_movement(1)
	modifier.strength = modifier.strength - secs
	return modifier.strength > 0
end
