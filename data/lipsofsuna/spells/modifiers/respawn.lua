local ModifierSpec = require("core/specs/modifier")

local RespawnModifier = ModifierSpec{
	name = "respawn",
	influences =
	{
		["respawn"] = 1
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
RespawnModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.strength = value
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
RespawnModifier.update = function(self, mod, secs)
	-- Wait for the sector to load.
	if not Main.terrain:is_point_loaded(modifier.object:get_position()) then
		return true
	end
	-- Find the terrain surface.
	local src = modifier.object:get_position()
	local dst = Vector(src.x, 0, src.z)
	local p = Main.terrain.terrain:cast_ray(src, dst)
	-- Try another position if failed.
	if not p then
		modifier.object:set_position(src.x + 10 * math.random(), 1000, src.z + 10 * math.random())
		return true
	end
	-- Snap to the terrain surface.
	modifier.object:set_position(p)
end
