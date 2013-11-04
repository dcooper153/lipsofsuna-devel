local ModifierSpec = require("core/specs/modifier")
local Vector = require("system/math/vector")

local SpawnModifier = ModifierSpec:find_by_name("spawn")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
SpawnModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.point = modifier.object:get_position():copy()
	modifier.strength = value
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
SpawnModifier.update = function(modifier, secs)
	-- Keep stationary until spawned.
	modifier.object:set_position(modifier.point)
	modifier.object:set_velocity(Vector())
	-- Wait for the sector to load.
	if not Main.terrain:is_point_loaded(modifier.point) then
		return true
	end
end
