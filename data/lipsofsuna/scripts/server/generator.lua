
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
		if v[4] then
			local mat = Material:find{name = v[4]}
			if mat then
				Voxel:set_tile{
					point = args.point + Vector(v[1], v[2], v[3]),
					tile = mat.id}
			end
		else
			Voxel:set_tile{
				point = args.point + Vector(v[1], v[2], v[3]),
				tile = 0}
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

------------------------------------------------------------------------------

Generator = Class()

--- Draws a corridor in the map.
-- @param clss Generator class.
-- @param src Source point in tiles.
-- @param dst Destination point in tiles.
Generator.paint_corridor = function(clss, src, dst)
	local dir = (dst - src):normalize()
	local len = math.floor((dst - src).length)
	local pos = Vector(src.x, src.y, src.z)
	local ipos = Vector()
	for l = 0,len do
		ipos.x = math.floor(pos.x + 0.5)
		ipos.y = math.floor(pos.y + 0.5)
		ipos.z = math.floor(pos.z + 0.5)
		Voxel:place_pattern{name = "corridor1", point = ipos}
		pos = pos + dir
	end
end

--- Places a region to the map.
-- @param clss Generator class.
-- @param reg Region spec.
Generator.place_region = function(clss, reg)
	-- Determine the approximate position.
	-- Regions can be placed randomly or relative to each other. Here we
	-- decide the range of positions that are valid for the region.
	local rel = nil
	local dist = nil
	if reg.position then
		rel = Vector(reg.position[1], 0, reg.position[2])
	elseif not reg.distance then
		rel = Vector(math.random(500, 2500), 0, math.random(500, 2500))
		dist = {nil, 300, 300}
	elseif clss.regions_dict_name[reg.distance[1]] then
		rel = clss.regions_dict_name[reg.distance[1]].point
		rel = Vector(rel.x, 0, rel.z)
		dist = reg.distance
	else return end
	-- Determine the final position.
	-- Regions with a random position range are placed at different
	-- positions until one is found where they fit. Regions with fixed
	-- positions are simply placed there without any checks.
	local pos = Vector()
	local size = Vector(reg.size[1], reg.size[2], reg.size[3])
	local aabb = Aabb{point = pos, size = size + Vector(10,10,10)}
	if dist then
		repeat
			pos.x = math.random(dist[2], dist[3])
			pos.y = math.random(reg.depth[1], reg.depth[2])
			pos.z = math.random(dist[2], dist[3])
			if math.random(0, 1) == 1 then pos.x = -pos.x end
			if math.random(0, 1) == 1 then pos.z = -pos.z end
			pos = pos + rel
			aabb.point = pos - Vector(5,5,5)
		until clss:validate_region_position(aabb)
	else
		pos.x = rel.x
		pos.y = math.random(reg.depth[1], reg.depth[2])
		pos.z = rel.z
		aabb.point = pos - Vector(5,5,5)
	end
	-- Store the region.
	local region = {aabb = aabb, links = {}, point = pos, size = size, spec = reg}
	table.insert(clss.regions_dict_id, region)
	clss.regions_dict_name[reg.name] = region
	return region
end

--- Checks if a region can be placed in the given position.
-- @param clss Generator class.
-- @param aabb Position of the region.
-- @return True if the position is valid.
Generator.validate_region_position = function(clss, aabb)
	for k,v in pairs(clss.regions_dict_id) do
		if aabb:intersects(v.aabb) then return end
	end
	return true
end

--- Generates the world map.
-- @param clss Generator class.
-- @param args Arguments.
Generator.generate = function(clss, args)
	-- Initialize state.
	clss.links = {}
	clss.regions_dict_id = {}
	clss.regions_dict_name = {}
	-- Place special areas.
	-- Regions have dependencies so we need to place them in several passes.
	-- The region tables are filled but the map is all empty after this.
	clss:update_status(0, "Placing regions")
	while true do
		local skipped = 0
		for name,reg in pairs(Regionspec.dict_name) do
			if not clss.regions_dict_name[name] then
				if not clss:place_region(reg) then
					skipped = skipped + 1
				end
			end
		end
		if skipped == 0 then break end
	end
	-- Initialize connectivity.
	-- A cyclic graph is created out of the special areas by creating links
	-- between them according to the rules in the region specs.
	clss:update_status(0, "Connecting regions")
	local linkn = 0
	for _,reg1 in pairs(clss.regions_dict_name) do
		for _,name in ipairs(reg1.spec.links) do
			local reg2 = clss.regions_dict_name[name]
			local link = {reg1, reg2}
			reg1.links[reg2.spec.name] = link
			reg2.links[reg1.spec.name] = link
			table.insert(clss.links, link)
			linkn = linkn + 1
		end
	end
	-- TODO: Randomize connectivity.
	-- Paint corridors.
	clss:update_status(0, "Creating corridors")
	for i,link in ipairs(clss.links) do
		local src = (link[1].point + link[1].size * 0.5):floor()
		local dst = (link[2].point + link[2].size * 0.5):floor()
		clss:paint_corridor(src, dst)
		clss:update_status(i / linkn)
	end
	-- Paint regions.
	clss:update_status(0, "Creating regions")
	local region_funcs = {
		-- TODO
		["Lips"] = function(r)
			Voxel:place_pattern{point = r.point + Vector(1,0,1), name = "house1"}
			Voxel:place_pattern{point = r.point + Vector(5,0,1), name = "house1"}
			Voxel:place_pattern{point = r.point + Vector(1,0,6), name = "house1"}
			Voxel:place_pattern{point = r.point + Vector(5,0,6), name = "house1"}
			Voxel:place_pattern{point = r.point + Vector(9,2,9), name = "mourningadventurer_town"}
			Voxel:place_pattern{point = r.point + Vector(4,1,4), name = "peculiarpet"}
			Voxel:place_creature{point = r.point + Vector(3,0,14), name = "lipscitizen"}
			Voxel:place_creature{point = r.point + Vector(8,0,9), name = "lipscitizen"}
--[[
			local p = Vector(10,0,0)
			local s = r.size - p
			Voxel:make_heightmap{point = r.point + p, size = s, material = "grass1"}
			p = p + Vector(3,1,3)
			s = s - Vector(2,1,2)
			Voxel:make_heightmap{point = r.point + p, size = s,
				material = "grass1", tree_density = 0.05}]]
		end,
		["Chara's Root Grove"] = function(r)
			Voxel:place_pattern{point = r.point + Vector(4,0,4), name = "rootsofworld"}
			Voxel:place_pattern{point = r.point + Vector(2,0,2), name = "mourningadventurer_lost"}
		end}
	for name,reg in pairs(clss.regions_dict_name) do
		Voxel:fill_region{point = reg.point, size = reg.size}
		local func = region_funcs[name]
		if func then func(reg) end
	end
	-- Find used sectors.
	clss:update_status(0, "Counting sectors")
	local sectorn = 0
	local sectors = Program.sectors
	for k in pairs(sectors) do
		sectorn = sectorn + 1
	end
	-- Randomize tiles.
	clss:update_status(0, "Creating ore veins")
	local index = 0
	for k in pairs(sectors) do
		Sectors:format_generated_sector(k)
		clss:update_status(index / sectorn)
		index = index + 1
	end
end

Generator.update_status = function(clss, frac, msg)
	if msg then
		print(math.ceil(frac * 100) .. "% " .. msg)
		clss.prev_message = msg
		clss.prev_fraction = frac
	elseif frac == 1 or frac > clss.prev_fraction + 0.1 then
		print(math.ceil(frac * 100) .. "% " .. clss.prev_message)
		clss.prev_fraction = frac
	end
end
