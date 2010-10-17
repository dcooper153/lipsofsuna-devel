print "INFO: Loading server scripts."

local oldrequire = require
require = function(arg)
	local s,e = string.find(arg, "core/")
	if s then
		Program:load_extension(string.sub(arg, e + 1))
	else
		oldrequire(arg)
	end
end

require "core/database"
require "core/generator"
require "core/network"
require "core/object-physics"
require "core/tiles"
require "core/tiles-physics"
Voxel.blocks_per_line = 4
Voxel.tiles_per_line = 8

require "common/effect"
require "common/eventhandler"
require "common/faction"
require "common/feat"
require "common/protocol"
require "common/vision"
require "common/inventory"
require "common/itemspec"
require "common/material"
require "common/model"
require "common/names"
require "common/object"
require "common/obstaclespec"
require "common/pattern"
require "common/quest"
require "common/regionspec"
require "common/skills"
require "common/slots"
require "common/species"
require "common/thread"
require "common/timer"
require "content/effects"
require "content/factions"
require "content/feats"
require "content/items"
require "content/materials"
require "content/obstacles"
require "content/patterns"
require "content/regions"
require "content/quests"
require "content/species"
require "server/util"
require "server/serialize"
require "server/attack"
require "server/combat"
require "server/controls"
require "server/crafting"
require "server/dialog"
require "server/actions/moveitem"
require "server/feat"
require "server/feats/attack"
require "server/feats/attack-melee"
require "server/feats/attack-ranged"
require "server/feats/attack-throw"
require "server/feats/bite"
require "server/feats/criticalhit"
require "server/feats/fireball"
require "server/feats/heal"
require "server/feats/spinattack"
require "server/feats/vectorchange"
require "server/objects/object"
require "server/objects/creature"
require "server/objects/container"
require "server/objects/decayer"
require "server/objects/door"
require "server/objects/fragile"
require "server/objects/item"
require "server/objects/mover"
require "server/objects/obstacle"
require "server/objects/player"
require "server/npcs/lastboss"
require "server/admin"
require "server/quest"
require "server/quests/peculiarpet"
require "server/quests/test"
require "server/modifier"
require "server/modifiers/bleeding"
require "server/editing"
require "server/particles"

--- Places a predefined map pattern to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>name: Pattern name.</li></ul>
Voxel.place_pattern = function(clss, args)
	local pat = Pattern:find(args)
	if not pat then return end
	-- Create tiles.
	for k,v in pairs(pat.tiles) do
		local mat = Material:find{name = v[4]}
		if mat then
			local tile = Tile{terrain = mat.id}
			if v[5] then tile:rotate{axis = "y", step = v[5]} end
			Voxel:set_tile{
				point = args.point + Vector(v[1], v[2], v[3]),
				tile = tile}
		end
	end
	-- Create obstacles.
	for k,v in pairs(pat.obstacles) do
		local point = args.point + Vector(v[1], v[2], v[3])
		local spec = Obstaclespec:find{name = v[4]}
		if spec then
			Obstacle{
				spec = spec,
				position = point * Config.tilewidth,
				realized = true}
		end
	end
	-- Create items.
	for k,v in pairs(pat.items) do
		local point = args.point + Vector(v[1], v[2], v[3])
		local spec = Itemspec:find{name = v[4]}
		if spec then
			Item{
				itemspec = spec,
				position = point * Config.tilewidth,
				realized = true}
		end
	end
	-- Create creatures.
	for k,v in pairs(pat.creatures) do
		local point = args.point + Vector(v[1], v[2], v[3])
		local spec = Species:find{name = v[4]}
		if spec then
			Creature{
				species = spec,
				position = point * Config.tilewidth,
				realized = true}
		end
	end
end

-- Initialize map generation rules.
for k,reg in pairs(Regionspec.dict_name) do
	Generator:add_brush{name = reg.name, size = Vector(reg.size[1], reg.size[2], reg.size[3])}
end
for k,reg in pairs(Regionspec.dict_name) do
	for l,rule in pairs(reg.links) do
		local id = Generator:add_rule{name = reg.name}
		for m,link in pairs(rule) do
			Generator:add_link{
				name = reg.name,
				brush = link[1],
				offset = Vector(link[2], link[3], link[4]),
				rule = id}
		end
	end
end

restart = function()
	Network.closed = true
	for k,v in pairs(Network.clients) do
		Network:disconnect{object = v}
	end
	Program:unload_world()
	print("Generating map...")
	Generator:format{center = Config.center * Config.tilescale}
	Generator:expand{count = 2}
	for i,r in pairs(Generator.regions) do
		Voxel:fill_region{point = r.point, size = r.size}
		local spec = Regionspec:find{name = r.name}
		if spec then
			-- TODO: Actually fill the regions here.
			if spec.style == "lips" then
				Voxel:place_pattern{point = r.point + Vector(1,0,1), name = "house1"}
				Voxel:place_pattern{point = r.point + Vector(5,0,1), name = "house1"}
				Voxel:place_pattern{point = r.point + Vector(1,0,6), name = "house1"}
				Voxel:place_pattern{point = r.point + Vector(5,0,6), name = "house1"}
				Voxel:place_pattern{point = r.point + Vector(1.7,1,1.7), name = "peculiarpet"}
			end
		end
	end
	-- Save map.
	print("Saving map...")
	Serialize:save_world()
	Program:unload_world()
	print("Done")
	Network.closed = false
end

Voxel.fill = Tile:new{terrain = 1}
restart()
Network:host{port = 10101}

-- Simulate.
Eventhandler{type = "object-motion", func = function(self, event)
	Vision:event{type = "object-moved", object = event.object}
end}

-- Visibility.
Eventhandler{type = "object-visibility", func = function(self, event)
	if event.visible then
		Vision:event{type = "object-shown", object = event.object}
	else
		Vision:event{type = "object-hidden", object = event.object}
	end
end}

Voxel.block_changed_cb = function(index, stamp)
	Vision:event{type = "voxel-block-changed", index = index, point = ARGH, stamp = stamp}
end

math.randomseed(1000000 * Program.time)

-- Main loop.
while not Program.quit do
	-- Update program state.
	Program:update()
	-- Handle events.
	local event = Program:pop_event()
	while event do
		Eventhandler:event(event)
		event = Program:pop_event()
	end
end

