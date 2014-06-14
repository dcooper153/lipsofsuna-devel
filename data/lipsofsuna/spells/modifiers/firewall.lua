local AreaSpell = require("core/objects/areaspell")

local FirewallModifier = Main.specs:find_by_name("ModifierSpec", "firewall")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
FirewallModifier.start = function(modifier, value)
	-- Find an empty ground spot.
	local ctr = Utils:find_empty_ground(modifier.point)
	if not ctr then return end
	-- Select the creation direction.
	local d = ctr:copy():subtract(modifier.owner:get_position()):abs()
	local dir = (d.x < d.z) and Vector(1) or Vector(0,0,1)
	-- Create the flames.
	for i=-2,2 do
		local w = ctr + dir * i * Main.terrain.grid_size
		local p = Utils:find_empty_ground(w)
		if p then
			local spec = Main.specs:find_by_name("SpellSpec", "firewall1")
			local spell = AreaSpell(modifier.owner.manager)
			local modifiers = {["burning"] = 3}
			spell:set_spec(spec)
			spell:set_modifiers(modifiers)
			spell:set_owner(modifier.owner)
			spell.duration = 15
			spell.radius = 1.3
			spell:set_position(w)
			spell:set_visible(true)
		end
	end
end

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
FirewallModifier.start_terrain = function(modifier, value)
	FirewallModifier.start(modifier, value)
end
