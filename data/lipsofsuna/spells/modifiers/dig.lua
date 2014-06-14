local BuildingUtils = require("building/building-utils")

-- Dig terrain.
local DigModifier = Main.specs:find_by_name("ModifierSpec", "dig")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
DigModifier.start = function(modifier, value)
	BuildingUtils:destroy_terrain_sphere(modifier.owner, modifier.point, 3)
end

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
DigModifier.start_terrain = function(modifier, value)
	DigModifier.start(modifier, value)
end
