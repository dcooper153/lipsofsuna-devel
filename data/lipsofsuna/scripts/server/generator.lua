
--- Called when an empty sector is created.
-- @param self Sectors.
-- @param sector Sector index.
Sectors.format_generated_sector = function(self, sector)
	local w = 128
	local sx = sector % w
	local sy = math.floor(sector / w) % w
	local sz = math.floor(sector / w / w) % w
	local org = Vector(sx, sy, sz) * Voxel.tiles_per_line
	local mats = {
		Material:find{name = "adamantium1"},
		Material:find{name = "aquanite1"},
		Material:find{name = "basalt1"},
		Material:find{name = "brittlerock1"},
		Material:find{name = "crimson1"},
		Material:find{name = "ferrostone1"},
		Material:find{name = "sand1"},
		Material:find{name = "soil1"}}
	--[[Material:find{name = "grass1"},
		Material:find{name = "ice1"},
		Material:find{name = "iron1"},
		Material:find{name = "magma1"},
		Material:find{name = "pipe1"},
		Material:find{name = "water1"},
		Material:find{name = "wood1"}]]
	local growdirs = {
		Vector(-1, 0, 0),
		Vector(1, 0, 0),
		Vector(0, -1, 0),
		Vector(0, 1, 0),
		Vector(0, 0, -1),
		Vector(0, 0, 1)}
	local growrand
	growrand = function(ctr, mat, dep)
		local p = org + ctr
		local t = Voxel:get_tile{point = p}
		if t == 0 then return end
		Voxel:set_tile{point = p, tile = mat}
		if dep > 4 then return end
		for k,v in pairs(growdirs) do
			if math.random() < (4 - dep) * 0.15 then
				local p = ctr + v
				p.x = math.max(math.min(p.x, Voxel.tiles_per_line - 1), 1)
				p.y = math.max(math.min(p.y, Voxel.tiles_per_line - 1), 1)
				p.z = math.max(math.min(p.z, Voxel.tiles_per_line - 1), 1)
				growrand(p, mat, dep + 1)
			end
		end
	end
	local p = Vector()
	for i=1,100 do
		p.x = math.random(1, Voxel.tiles_per_line - 1)
		p.y = math.random(1, Voxel.tiles_per_line - 1)
		p.z = math.random(1, Voxel.tiles_per_line - 1)
		local m = mats[math.random(1,#mats)]
		if m then growrand(p, m.id, 1) end
	end
end

--- Places a random heightmap to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>material: Material name.</li>
--   <li>mushroom_density: Per tile mushroom probability or nil.</li></ul>
--   <li>point: Position vector, in tiles.</li>
--   <li>size: Size vector, in tiles.</li>
--   <li>tree_density: Per tile tree probability or nil.</li></ul>
Voxel.make_heightmap = function(clss, args)
	local e = (args.height or 2) + 1
	local m = Material:find{name = args.material}
	if not m then return end
	for z = args.point.z,args.point.z+args.size.z-1 do
		for x = args.point.x,args.point.x+args.size.x-1 do
			local h = math.floor(e * math.random())
			for y = args.point.y,args.point.y+h-1 do
				Voxel:set_tile{point = Vector(x,y,z), tile = m.id}
			end
			if math.random() < (args.tree_density or 0) then
				Voxel:place_obstacle{category = "tree", point = Vector(x + 0.5, args.point.y + h, z + 0.5)}
			elseif math.random() < (args.mushroom_density or 0) then
				Voxel:place_obstacle{name = "mushroom", point = Vector(x + 0.5, args.point.y + h, z + 0.5)}
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
				Voxel:set_tile{point = p, tile = m.id}
			end
			-- Position extra objects.
			p.y = args.point.y
			if math.random() < (args.tree_density or 0) then
				Voxel:place_obstacle{category = "tree", point = p + Vector(0.5, h, 0.5)}
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
--   <li>point: Position vector, in tiles.</li>
--   <li>prob: Creation probability.</li></ul>
Voxel.place_creature = function(clss, args)
	local spec = Species:find(args)
	if not spec then return end
	if args.prob and math.random() > args.prob then return end
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
	local spec = Obstaclespec:random(args)
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
			Voxel:set_tile{
				point = args.point + Vector(v[1], v[2], v[3]),
				tile = mat.id}
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

------------------------------------------------------------------------------

--- Disables all region types.
-- @param clss Generator class.
-- @param args Arguments.<ul>
--   <li>except: Name of a rule not to enable.</li></ul>
Generator.disable_regions = function(clss, args)
	for k,reg in pairs(Regionspec.dict_name) do
		Generator:disable_brush{name = reg.name}
	end
	Generator:enable_brush{name = args and args.except}
end

--- Enables all region types.
-- @param clss Generator class.
-- @param args Arguments.<ul>
--   <li>except: Name of a rule not to disable.</li></ul>
Generator.enable_regions = function(clss, args)
	for k,reg in pairs(Regionspec.dict_name) do
		Generator:enable_brush{name = reg.name}
	end
	Generator:disable_brush{name = args and args.except}
end

Generator.generate = function(clss, args)
	-- Generate the town and its surroundings.
	Generator:format{center = Config.center * Config.tilescale}
	Generator:disable_brush{name = "rootsofworld-grove"}
	Generator:expand{count = 10}
	-- Generate Chara's root grove.
	local success
	repeat
		Generator:disable_regions{except = "rootsofworld-grove"}
		success = Generator:expand{count = 1}
		Generator:enable_regions{except = "rootsofworld-grove"}
		Generator:expand{count = 1}
	until success
	-- Generate lots of pointless random regions.
	Generator:expand{count = 89}
	-- Populate the generated regions.
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
				Voxel:place_pattern{point = r.point + Vector(9,2,9), name = "mourningadventurer_town"}
				Voxel:place_pattern{point = r.point + Vector(4,1,4), name = "peculiarpet"}
				Voxel:place_creature{point = r.point + Vector(3,0,14), name = "lipscitizen"}
				Voxel:place_creature{point = r.point + Vector(8,0,9), name = "lipscitizen"}
				local p = Vector(10,0,0)
				local s = r.size - p
				Voxel:make_heightmap{point = r.point + p, size = s, material = "grass1"}
				p = p + Vector(3,1,3)
				s = s - Vector(2,1,2)
				Voxel:make_heightmap{point = r.point + p, size = s,
					material = "grass1", tree_density = 0.05}
			elseif spec.name == "dungeon-room" then
				Voxel:place_pattern{point = r.point + Vector(2,0,2), name = "house1"}
			elseif spec.name == "dungeon-slope-n" then
				Voxel:make_slope{point = r.point, size = r.size,
					heights = {2,2,3,3}, randomness = 1,
					material = "soil1", tree_density = 0.05}
			elseif spec.name == "dungeon-slope-s" then
				Voxel:make_slope{point = r.point, size = r.size,
					heights = {3,3,2,2}, randomness = 1,
					material = "soil1", tree_density = 0.05}
			elseif spec.name == "dungeon-slope-e" then
				Voxel:make_slope{point = r.point, size = r.size,
					heights = {2,3,2,3}, randomness = 1,
					material = "soil1", tree_density = 0.05}
			elseif spec.name == "dungeon-slope-w" then
				Voxel:make_slope{point = r.point, size = r.size,
					heights = {3,2,3,2}, randomness = 1,
					material = "soil1", tree_density = 0.05}
			elseif spec.name == "dungeon-corridor-ns" then
				Voxel:place_creature{point = r.point + Vector(1,1,3), name = "bloodworm", prob = 0.25}
				Voxel:place_creature{point = r.point + Vector(2,1,8), name = "bloodworm", prob = 0.25}
				Voxel:make_heightmap{point = r.point + Vector(0,0,0), size = r.size,
					material = "grass1", tree_density = 0.05,
					mushroom_density = 0.3}
			elseif spec.name == "dungeon-corridor-ew" then
				Voxel:place_creature{point = r.point + Vector(3,1,1), name = "bloodworm", prob = 0.25}
				Voxel:place_creature{point = r.point + Vector(8,1,2), name = "bloodworm", prob = 0.25}
				Voxel:make_heightmap{point = r.point + Vector(0,0,0), size = r.size,
					material = "grass1", tree_density = 0.01,
					mushroom_density = 0.3}
			elseif spec.name == "rootsofworld-grove" then
				Voxel:place_pattern{point = r.point + Vector(4,0,4), name = "rootsofworld"}
				Voxel:place_pattern{point = r.point + Vector(2,0,2), name = "mourningadventurer_lost"}
			end
		end
	end
	-- Randomize tiles.
	Program:update()
	while true do
		local e = Program:pop_event()
		if not e then break end
		if e.type == "sector-load" then
			Sectors:format_generated_sector(e.sector)
		end
	end
end
