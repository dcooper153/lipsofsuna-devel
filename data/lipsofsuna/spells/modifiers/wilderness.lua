local ModifierSpec = require("core/specs/modifier")
local Obstacle = require("core/objects/obstacle")
local TerrainMaterialSpec = require("core/specs/terrain-material")

-- Spawn trees and plants.
local WildernessModifier = Main.specs:find_by_name("ModifierSpec", "wilderness")

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
WildernessModifier.start = function(modifier, value)
	-- Find an empty ground spot under the player.
	local org = Utils:find_empty_ground(modifier.point)
	if not org then return end
	-- Find empty ground spots around the player.
	local r = 5
	local pos = org:copy()
	local hits = {}
	for x=-r,r do
		pos.x = org.x + Main.terrain.grid_size * x
		for z=-r,r do
			pos.z = org.z + Main.terrain.grid_size * z
			local ground = Utils:find_empty_ground(pos)
			if ground then
				local gx,gz = Main.terrain:get_grid_xz_by_point(ground.x, ground.z)
				local y,h,mat = Main.terrain.terrain:get_stick(gx, gz, ground.y - 0.01)
				local matspec = Main.specs:find_by_id("TerrainMaterialSpec", mat)
				if matspec and (matspec.name == "soil" or matspec.name == "grass") then
					table.insert(hits, ground)
				end
			end
		end
	end
	-- Create wilderness obstacles.
	if #hits > 0 then
		for i=1,3 do
			local k = math.random(1,#hits)
			local p = hits[k]
			if p then
				hits[k] = nil
				local obstspec = Obstaclespec:random{category = "wilderness"}
				if obstspec then
					if Utils:check_room(p, obstspec.model) then
						local o = Obstacle(modifier.owner.manager)
						o:set_position(p)
						o:set_spec(obstspec)
						o:set_visible(true)
						Main.vision:world_effect(p, "wilderness growth 1")
					end
				end
			end
		end
	end
end

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
WildernessModifier.start_terrain = function(modifier, value)
	WildernessModifier.start(modifier, value)
end
