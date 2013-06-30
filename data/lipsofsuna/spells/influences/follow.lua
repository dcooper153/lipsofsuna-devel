local FollowModifier = Feateffectspec{
	name = "follow",
	categories =
	{
		["follow"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true,
		["touch spell"] = true
	},
	description = "Forces the target to follow you",
	effect = "light1",
	icon = "modifier-light",
	influences = {["follow"] = 30},
	required_stats = {["willpower"] = 5},
	projectile = "magicmissile1"}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
FollowModifier.start = function(modifier, value)
	if not modifier.object then return end
	modifier.strength = value
	return true
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
FollowModifier.update = function(modifier, secs)
	modifier.timer = modifier.timer + secs
	if modifier.timer > 1 then
		modifier.object:face_point{point = modifier.owner:get_position()}
		modifier.object:set_movement(10)
		modifier.object:climb()
		modifier.timer = modifier.timer - 1
	end
	modifier.strength = modifier.strength - secs
	return modifier.strength > 0
end
