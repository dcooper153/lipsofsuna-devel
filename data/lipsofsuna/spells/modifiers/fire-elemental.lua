local Actor = require("core/objects/actor")
local ModifierSpec = require("core/specs/modifier")

local FireElementalModifier = Main.specs:find_by_name("ModifierSpec", "fire elemental")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
FireElementalModifier.start = function(modifier, value)
	-- Find an empty ground spot.
	if not modifier.object then return end
	local ctr = Utils:find_summon_point(modifier.object:get_position())
	if not ctr then return end
	-- Create the summon.
	local spec = Main.specs:find_by_name("Actorspec", "fire elemental")
	if not spec then return end
	local summon = Actor(modifier.owner.manager)
	summon:set_spec(spec)
	summon:set_position(ctr)
	summon:randomize()
	summon:set_visible(true)
	summon.summon_owner = modifier.owner
	summon.summon_timer = value
end
