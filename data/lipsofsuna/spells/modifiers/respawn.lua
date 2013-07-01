local ModifierSpec = require("core/specs/modifier")
local Vector = require("system/math/vector")

local RespawnModifier = ModifierSpec{
	name = "respawn",
	modifiers =
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
RespawnModifier.update = function(modifier, secs)
	-- Wait for the sector to load.
	local src = modifier.object:get_position()
	if not Main.terrain:is_point_loaded(src) then
		return true
	end
	-- Find the terrain surface.
	local dst = Vector(src.x, 0, src.z)
	local p = Main.terrain.terrain:cast_ray(src, dst)
	-- Try another position if failed.
	if not p then
		dst.x = src.x + 10 * math.random()
		dst.y = 1000
		dst.z = src.z + 10 * math.random()
		modifier.object:set_position(dst)
		modifier.object.physics:set_velocity(Vector())
		return true
	end
	-- Snap to the terrain surface.
	modifier.object:set_position(p)
end
