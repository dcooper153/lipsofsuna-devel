local AreaSpell = require("core/objects/areaspell")
local Feat = require("arena/feat")

Feateffectspec:extend{
	name = "firewall",
	ranged = function(self, args)
		-- Find an empty ground spot.
		local ctr = Utils:find_empty_ground(args.point)
		if not ctr then return end
		-- Select the creation direction.
		local d = ctr:copy():subtract(args.object:get_position()):abs()
		local dir = (d.x < d.z) and Vector(1) or Vector(0,0,1)
		-- Create the flames.
		for i=-2,2 do
			local w = ctr + dir * i * Voxel.tile_size
			local t = w:copy():multiply(Voxel.tile_scale):add_xyz(0,0.5,0):floor()
			if Voxel:get_tile(t) == 0 and Voxel:get_tile(t - Vector(0,1)) ~= 0 then
				local feat = Feat("area spell", {{"burning", 1}})
				local spec = Spellspec:find{name = "firewall1"}
				AreaSpell{duration = 15, radius = 1.3, feat = feat, owner = args.owner,
					position = w, realized = true, spec = spec}
			end
		end
	end}
