local Material = require("system/material")
local ModifierSpec = require("core/specs/modifier")
local Obstacle = require("core/objects/obstacle")

-- Spawn trees and plants.
local WildernessModifier = ModifierSpec{
	name = "wilderness",
	actions =
	{
		["self spell"] = true
	},
	categories =
	{
		["spell"] = true
	},
	description = "Grow plants from soil",
	effect = "wilderness1",
	icon = "modifier-nature",
	influences =
	{
		["wilderness"] = 5
	},
	required_stats =
	{
		["willpower"] = 5
	}}

--- Applies the modifier.
-- @param modifier Modifier.
-- @param value Strength of the modifier.
-- @return True to enable effect-over-time updates. False otherwise.
WildernessModifier.start = function(modifier, value)
	-- FIXME: Not migrated yet.
	do return end
	-- Calculate spell radius.
	local s = math.min(args.value, 100) / 100
	local r = 4 * s
	if args.owner.spec.categories["devora"] then
		r = math.ceil(r * 1.5)
	end
	r = 2 + math.max(r,1)
	-- Create grass from soil.
	local soil = Material:find{name = "soil1"}
	local grass = Material:find{name = "grass1"}
	local org = (args.point * Voxel.tile_scale):floor()
	local pos = Vector()
	local hits = {}
	for x=org.x-r,org.x+r do
		pos.x = x
		for z=org.z-r,org.z+r do
			pos.z = z
			local blocked = true
			for y=org.y+r+1,org.y-r,-1 do
				pos.y = y
				local tile = Voxel:get_tile(pos)
				if not blocked then
					if tile == soil:get_id() then
						Voxel:set_tile(pos, grass:get_id())
					end
					if tile == soil:get_id() or tile == grass:get_id() then
						local vec = Vector(x,y,z)
						table.insert(hits, vec)
					end
				end
				blocked = (tile ~= 0)
			end
		end
	end
	-- Create wilderness obstacles.
	if #hits then
		for i=1,math.ceil(s/25) do
			local k = math.random(1,#hits)
			local p = hits[k]
			if p then
				hits[k] = nil
				local obstspec = Obstaclespec:random{category = "wilderness"}
				if obstspec then
					local pos = (p + Vector(0.5,0.5,0.5)) * Voxel.tile_size
					if Utils:check_room(pos, obstspec.model) then
						local o = Obstacle(args.owner.manager)
						o:set_position(pos)
						o:set_spec(obstspec)
						o:set_visible(true)
					end
				end
			end
		end
	end
end
