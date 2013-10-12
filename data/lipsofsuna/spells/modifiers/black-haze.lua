local Actor = require("core/objects/actor")
local Modifier = require("core/server/modifier")
local ModifierSpec = require("core/specs/modifier")

-- Summon plagued beasts.
local BlackHazeModifier = ModifierSpec:find_by_name("black haze")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
BlackHazeModifier.start = function(modifier, value)
	-- Handle the infection mode.
	if not value then
		modifier.timer = 0
		return true
	end
	-- Choose a random plague monster.
	local s = Actorspec:random{category = "plague"}
	if not s then return end
	-- Spawn the monster.
	local p = modifier.point + Vector(
		-1 + 2 * math.random(),
		-1 + 2 * math.random(),
		-1 + 2 * math.random())
	local o = Actor(modifier.owner.manager)
	o:set_spec(s)
	o:set_position(p)
	o:randomize()
	o:set_visible(true)
	-- Add the plague modifier to the monster.
	o:add_modifier(modifier.spec.name)
end

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
BlackHazeModifier.start_terrain = function(modifier, value)
	BlackHazeModifier.start(modifier, value)
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
			local m = v:add_modifier(modifier.spec.name)
		end
	end
	return true
end
