local Actor = require("core/objects/actor")
local Modifier = require("core/server/modifier")

-- Summon plagued beasts.
local BlackHazeModifier = Feateffectspec{
	name = "black haze",
	categories =
	{
		["harmful"] = true,
		["plague"] = true,
		["spell"] = true
	},
	actions =
	{
		["missile spell"] = true,
		["ranged spell"] = true
	},
	description = "Conjure infectious plagued monsters",
	effect = "spell1",
	icon = "modifier-black haze",
	influences = {["black haze"] = 1},
	projectile = "fireball1",
	required_stats =
	{
		["willpower"] = 20
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
BlackHazeModifier.start = function(modifier)
	-- Choose a random plague monster.
	local s = Actorspec:random{category = "plague"}
	if not s then return end
	-- Spawn the monster.
	local p = modifier.owner:get_position() + Vector(
		-1 + 2 * math.random(),
		-1 + 2 * math.random(),
		-1 + 2 * math.random())
	local o = Actor(modifier.owner.manager)
	o:set_spec(s)
	o:set_position(p)
	o:randomize()
	o:set_visible(true)
	-- Add the plague modifier to the monster.
	local m = Modifier(modifier.spec, o)
	m.timer = 0
	o:add_modifier(m)
end

--- Updates the modifier for effect-over-time.
-- @param modifier Modifier.
-- @param secs Seconds since the last update.
-- @return True to continue effect-over-time updates. False otherwise.
BlackHazeModifier.update = function(modifier, secs)
	-- Wait five seconds.
	modifier.timer = modifier.timer + secs
	if modifier.timer < 5 then return true end
	modifier.timer = modifier.timer - 5
	-- Damage the actor.
	modifier.object:damaged{amount = 5, type = "disease"}
	-- Infect nearby actors.
	local near = modifier.object.manager:find_by_point(modifier.object:get_position(), 5)
	for k,v in pairs(near) do
		if math.random() > 0.1 then
			local m = Modifier(modifier.spec, v)
			m.timer = 0
			v:add_modifier(m)
		end
	end
	return true
end
