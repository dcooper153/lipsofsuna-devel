print "INFO: Loading server scripts."

require "core/database"
require "core/generator"
require "core/object-physics"

require "common/vision"
require "common/inventory"
require "common/skills"
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
require "server/quests/lipscitizen"
require "server/quests/peculiarpet"
require "server/modifier"
require "server/modifiers/bleeding"
require "server/editing"
require "server/particles"

--- Places a random heightmap to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>material: Material name.</li>
--   <li>mushroom_density: Per tile mushroom probability or nil.</li></ul>
--   <li>point: Position vector, in tiles.</li>
--   <li>size: Size vector, in tiles.</li>
--   <li>tree_density: Per tile tree probability or nil.</li></ul>
Voxel.make_heightmap = function(clss, args)
	local p = Vector(0, args.point.y, 0)
	local m = Material:find{name = args.material}
	if not m then return end
	local t = Tile{terrain = m.id}
	for z = args.point.z,args.point.z+args.size.z-1 do
		p.z = z
		for x = args.point.x,args.point.x+args.size.x-1 do
			local h = math.random()
			p.x = x
			t.damage = math.floor(255 * h)
			Voxel:set_tile{point = p, tile = t}
			if math.random() < (args.tree_density or 0) then
				Voxel:place_obstacle{name = "tree", point = p + Vector(0.5, 0.9 - h, 0.5)}
			elseif math.random() < (args.mushroom_density or 0) then
				Voxel:place_obstacle{name = "mushroom", point = p + Vector(0.5, 1 - h, 0.5)}
			end
		end
	end
end

--- Places a slope to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>heights: Desired heights of the four corners of the region.</li>
--   <li>material: Material name.</li>
--   <li>mushroom_density: Per tile mushroom probability or nil.</li></ul>
--   <li>point: Position vector, in tiles.</li>
--   <li>randomness: Randomness of height.</li>
--   <li>size: Size vector, in tiles.</li>
--   <li>tree_density: Per tile tree probability or nil.</li></ul>
Voxel.make_slope = function(clss, args)
	local c = args.heights
	local r = args.randomness or 0
	local p = Vector(0, args.point.y, 0)
	local m = Material:find{name = args.material}
	if not m then return end
	local t = Tile{terrain = m.id}
	for z = args.point.z,args.point.z+args.size.z-1 do
		p.z = z
		for x = args.point.x,args.point.x+args.size.x-1 do
			p.x = x
			-- Calculate the height with bilinear filtering.
			local u = (x - args.point.x) / (args.size.x - 1)
			local v = (z - args.point.z) / (args.size.z - 1)
			local h = (c[1] * (1 - u) + c[2] * u) * (1 - v) + 
			          (c[3] * (1 - u) + c[4] * u) * v;
			-- Add some randomness to the height.
			h = math.max(0, h + r * (0.5 - 0.5 * math.random()))
			-- Create the tile stack matching the height.
			for y = 0,math.floor(h) do
				p.y = args.point.y + y
				t.damage = 255 * (1 - math.min(1, h - y))
				Voxel:set_tile{point = p, tile = t}
			end
			-- Position extra objects.
			p.y = args.point.y
			if math.random() < (args.tree_density or 0) then
				Voxel:place_obstacle{name = "tree", point = p + Vector(0.5, h, 0.5)}
			elseif math.random() < (args.mushroom_density or 0) then
				Voxel:place_obstacle{name = "mushroom", point = p + Vector(0.5, h, 0.5)}
			end
		end
	end
end

--- Places a monster to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>name: Monster name.</li>
--   <li>point: Position vector, in tiles.</li></ul>
Voxel.place_creature = function(clss, args)
	local spec = Species:find(args)
	if not spec then return end
	Creature{
		species = spec,
		position = args.point * Config.tilewidth,
		realized = true}
end

--- Places an obstacle to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>name: Obstacle name.</li>
--   <li>point: Position vector, in tiles.</li></ul>
Voxel.place_obstacle = function(clss, args)
	local spec = Obstaclespec:find(args)
	if not spec then return end
	Obstacle{
		spec = spec,
		position = args.point * Config.tilewidth,
		realized = true}
end

--- Places a predefined map pattern to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>name: Pattern name.</li>
--   <li>point: Position vector, in tiles.</li></ul>
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
		clss:place_obstacle{
			name = v[4],
			point = args.point + Vector(v[1], v[2], v[3])}
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
	Generator:expand{count = 100}
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
				Voxel:place_creature{point = r.point + Vector(3,0,14), name = "lipscitizen"}
				Voxel:place_creature{point = r.point + Vector(8,0,9), name = "lipscitizen"}
				local p = Vector(10,0,0)
				local s = r.size - p
				Voxel:make_heightmap{point = r.point + p, size = s, material = "ground1"}
				p = p + Vector(3,1,3)
				s = s - Vector(2,1,2)
				Voxel:make_heightmap{point = r.point + p, size = s,
					material = "ground1", tree_density = 0.2}
			elseif spec.name == "dungeon-room" then
				Voxel:place_pattern{point = r.point + Vector(2,0,2), name = "house1"}
			elseif spec.name == "dungeon-slope-n" then
				Voxel:make_slope{point = r.point, size = r.size,
					heights = {2,2,3,3}, randomness = 1,
					material = "ground1", tree_density = 0.2}
			elseif spec.name == "dungeon-slope-s" then
				Voxel:make_slope{point = r.point, size = r.size,
					heights = {3,3,2,2}, randomness = 1,
					material = "ground1", tree_density = 0.2}
			elseif spec.name == "dungeon-slope-e" then
				Voxel:make_slope{point = r.point, size = r.size,
					heights = {2,3,2,3}, randomness = 1,
					material = "ground1", tree_density = 0.2}
			elseif spec.name == "dungeon-slope-w" then
				Voxel:make_slope{point = r.point, size = r.size,
					heights = {3,2,3,2}, randomness = 1,
					material = "ground1", tree_density = 0.2}
			elseif spec.name == "dungeon-corridor-ns" then
				Voxel:place_creature{point = r.point + Vector(1,1,3), name = "bloodworm"}
				Voxel:place_creature{point = r.point + Vector(2,1,8), name = "bloodworm"}
				Voxel:make_heightmap{point = r.point + Vector(0,0,0), size = r.size,
					material = "ground1", tree_density = 0.1,
					mushroom_density = 0.3}
			elseif spec.name == "dungeon-corridor-ew" then
				Voxel:place_creature{point = r.point + Vector(3,1,1), name = "bloodworm"}
				Voxel:place_creature{point = r.point + Vector(8,1,2), name = "bloodworm"}
				Voxel:make_heightmap{point = r.point + Vector(0,0,0), size = r.size,
					material = "ground1", tree_density = 0.1,
					mushroom_density = 0.3}
			end
		end
	end
	-- Save map.
	print("Saving map...")
	Serialize:save_world(true)
	Program:unload_world()
	-- Discard events emitted during map generation so that they
	-- don't trigger when the game starts.
	Program:update()
	repeat until not Program:pop_event()
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

