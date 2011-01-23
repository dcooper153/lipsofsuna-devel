--- Returns true if the requested range of tiles is empty.
-- @param src Tile range start.
-- @param dst Tile range end.
-- @return True if the range is empty.
Voxel.check_empty = function(clss, src, dst)
	local v = Vector()
	for x = src.x,dst.x do
		v.x = x
		for y = src.y,dst.y do
			v.y = y
			for z = src.z,dst.z do
				v.z = z
				if clss:get_tile{point = v} ~= 0 then
					return
				end
			end
		end
	end
	return true
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
	local spec = Species:random(args)
	if not spec then return end
	Creature{
		spec = spec,
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
		-- Get tile type.
		local tile = 0
		if v[4] then
			local mat = Material:find{name = v[4]}
			tile = mat and mat.id or 0
		end
		if v[5] then
			-- Fill volume of tiles.
			for x = v[1],v[1]+v[5] do
				for y = v[2],v[2]+v[6] do
					for z = v[3],v[3]+v[7] do
						Voxel:set_tile{
							point = args.point + Vector(x, y, z),
							tile = tile}
					end
				end
			end
		else
			-- Fill individual tile.
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
				spec = spec,
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
				spec = spec,
				position = point * Config.tilewidth,
				realized = true}
		end
	end
end

------------------------------------------------------------------------------

Region = Class()

Region.new = function(clss, args)
	local self = Class.new(clss, args)
	self.links = {}
	self.linked_regions = {}
	return self
end

Region.create_link = function(self, region)
	local link = {self, region}
	table.insert(self.links, link)
	table.insert(region.links, link)
	self.linked_regions[region] = true
	return link
end

------------------------------------------------------------------------------

Generator = Class()
Generator.map_size = Vector(500, 500, 500)
Generator.map_start = Vector(1500, 2750, 1500) - Generator.map_size * 0.5
Generator.map_end = Vector(1500, 2750, 1500) + Generator.map_size * 0.5
Generator.map_version = "1"

--- Informs clients of the generator status.
-- @param clss Generator class.
-- @param client Specific client to inform or nil to inform all.
-- @return Network packet.
Generator.inform_clients = function(clss, client)
	local p = Packet(packets.GENERATOR_STATUS,
		"string", clss.prev_message or "",
		"float", clss.prev_fraction or 0)
	if client then
		Network:send{client = client, packet = p}
	else
		for k,v in pairs(Network.clients) do
			Network:send{client = v, packet = p}
		end
	end
end

--- Draws a corridor in the map.
-- @param clss Generator class.
-- @param src Source point in tiles.
-- @param dst Destination point in tiles.
Generator.paint_corridor = function(clss, src, dst)
	local dir = (dst - src):normalize()
	local len = math.floor((dst - src).length)
	local pos = Vector(src.x, src.y, src.z)
	local ipos = Vector()
	local l = 0
	while l < len do
		ipos.x = math.floor(pos.x + 0.5) + math.random(0, 3)
		ipos.y = math.floor(pos.y + 0.5) + math.random(0, 3)
		ipos.z = math.floor(pos.z + 0.5) + math.random(0, 3)
		Voxel:place_pattern{name = "corridor1", point = ipos}
		local step = math.random(1, 3)
		pos = pos + dir * step
		l = l + step
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
		rel = Vector(math.random(clss.map_start.x, clss.map_end.x), 0, math.random(clss.map_start.z, clss.map_end.z))
		dist = {nil, 0.1 * clss.map_size.x, 0.1 * clss.map_size.x}
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
	local aabb = Aabb{point = pos, size = size + Vector(6,6,6)}
	if dist then
		repeat
			pos.x = math.random(dist[2], dist[3])
			pos.y = math.random(reg.depth[1], reg.depth[2])
			pos.z = math.random(dist[2], dist[3])
			if math.random(0, 1) == 1 then pos.x = -pos.x end
			if math.random(0, 1) == 1 then pos.z = -pos.z end
			pos = pos + rel
			aabb.point = pos - Vector(3,3,3)
		until clss:validate_region_position(aabb)
	else
		pos.x = rel.x
		pos.y = math.random(reg.depth[1], reg.depth[2])
		pos.z = rel.z
		aabb.point = pos - Vector(5,5,5)
	end
	-- Store the region.
	local region = Region{aabb = aabb, point = pos, size = size, spec = reg}
	table.insert(clss.regions_dict_id, region)
	clss.regions_dict_name[reg.name] = region
	return region
end

--- Checks if a region can be placed in the given position.
-- @param clss Generator class.
-- @param aabb Position of the region.
-- @return True if the position is valid.
Generator.validate_region_position = function(clss, aabb)
	if aabb.point.x < clss.map_start.x then return end
	if aabb.point.y < clss.map_start.y then return end
	if aabb.point.z < clss.map_start.z then return end
	if aabb.point.x + aabb.size.x > clss.map_end.x then return end
	if aabb.point.y + aabb.size.y > clss.map_end.y then return end
	if aabb.point.z + aabb.size.z > clss.map_end.z then return end
	for k,v in pairs(clss.regions_dict_id) do
		if aabb:intersects(v.aabb) then return end
	end
	return true
end

--- Generates the world map.
-- @param clss Generator class.
-- @param args Arguments.
Generator.generate = function(clss, args)
	-- Remove all player characters.
	for k,v in pairs(Player.clients) do
		v:detach(true)
	end
	Player.clients = {}
	-- Initialize state.
	clss.links = {}
	clss.regions_dict_id = {}
	clss.regions_dict_name = {}
	-- Reset the world.
	clss:update_status(0, "Resetting world")
	Marker:reset()
	Sectors.instance:unload_world()
	-- Place special areas.
	-- Regions have dependencies so we need to place them in several passes.
	-- The region tables are filled but the map is all empty after this.
	clss:update_status(0, "Placing regions")
	local special = Regionspec:find{category = "special"}
	while true do
		local skipped = 0
		for name,reg in pairs(special) do
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
	clss:update_status(0, "Placing primary paths")
	local linkn = 0
	for _,reg1 in pairs(clss.regions_dict_name) do
		for _,name in ipairs(reg1.spec.links) do
			local reg2 = clss.regions_dict_name[name]
			local link = reg1:create_link(reg2)
			linkn = linkn + 1
			clss.links[linkn] = link
		end
	end
	-- Randomize connectivity.
	-- Links between regions are subdivided and random regions are created.
	clss:update_status(0, "Randomizing primary paths")
	for i = 1,linkn do
		local link = clss.links[i]
		clss:subdivide_link(link)
		clss:update_status(i / linkn)
	end
	linkn = #clss.links
	-- Connect random rooms together.
	-- We try to make the maze less linear by taking some rooms with roughly
	-- the same Y offset and connecting them with a new random path.
	clss:update_status(0, "Placing secondary paths")
	local mapsize = clss.map_size.y
	local regionn = #clss.regions_dict_id
	local reglink = function(reg1, reg2)
		if reg1 == reg2 then return end
		if reg1.linked_regions[reg2] then return end
		if (reg1.point - reg2.point).length > 0.5 * mapsize then return end
		if #reg1.links > 7 then return end
		if #reg2.links > 7 then return end
		local link = reg1:create_link(reg2)
		linkn = linkn + 1
		clss.links[linkn] = link
		clss:subdivide_link(link)
		return true
	end
	for layer = 0,20 do
		local y = clss.map_start.y + clss.map_size.y * (layer / 20)
		local limit = 0.1 * clss.map_size.y
		local retry = 0
		local created = 0
		while retry < 100 and created < 20 do
			local reg1 = clss.regions_dict_id[math.random(1, regionn)]
			local reg2 = clss.regions_dict_id[math.random(1, regionn)]
			if math.abs(reg1.point.y - y) < limit and reglink(reg1, reg2) then
				created = created + 1
			else
				retry = retry + 1
			end
		end
		clss:update_status(layer / 20)
	end
	linkn = #clss.links
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
			Voxel:place_pattern{point = r.point + Vector(2,0,2), name = "house1"}
			Voxel:place_pattern{point = r.point + Vector(13,0,1), name = "house1"}
			Voxel:place_pattern{point = r.point + Vector(14,0,1), name = "house1"}
			Voxel:place_pattern{point = r.point + Vector(12,0,12), name = "spawnpoint1"}
			Voxel:place_pattern{point = r.point + Vector(9,2,9), name = "mourningadventurer_town"}
			Voxel:place_pattern{point = r.point + Vector(4,1,4), name = "peculiarpet"}
			Voxel:place_creature{point = r.point + Vector(3,0,14), name = "lipscitizen"}
			Voxel:place_creature{point = r.point + Vector(8,0,9), name = "lipscitizen"}
		end,
		["Chara's Root Grove"] = function(r)
			Voxel:place_pattern{point = r.point + Vector(4,0,4), name = "rootsofworld"}
			Voxel:place_pattern{point = r.point + Vector(2,0,2), name = "mourningadventurer_lost"}
		end,
		["Random: Dungeon"] = function(r)
			Voxel:place_pattern{point = r.point, name = "dungeon1"}
		end,
		["Random: Nature"] = function(r)
			Voxel:place_pattern{point = r.point, name = "nature1"}
		end,
		["Sanctuary"] = function(r)
			Voxel:place_pattern{point = r.point + Vector(3,0,3), name = "sanctuary1"}
		end}
	for _,reg in pairs(clss.regions_dict_id) do
		Voxel:fill_region{point = reg.point, size = reg.size}
		local func = region_funcs[reg.spec.name]
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
	clss:update_status(0, "Creating resource deposits")
	local index = 0
	for k in pairs(sectors) do
		Sectors:format_generated_sector(k)
		clss:update_status(index / sectorn)
		index = index + 1
	end
	-- Save the map.
	clss:update_status(0, "Saving the map")
	Sectors.instance:save_world(true, function(p) clss:update_status(p) end)
	Sectors.instance:unload_world()
	Serialize:set_value("map_version", Generator.map_version)
	-- Save map markers.
	clss:update_status(0, "Saving map markers")
	Serialize:save_markers(true)
	-- Discard events emitted during map generation so that they
	-- don't trigger when the game starts.
	clss:update_status(0, "Finishing")
	Program:update()
	repeat until not Program:pop_event()
	Program:update()
	repeat until not Program:pop_event()
	-- Inform players of the generation being complete.
	local status = Packet(packets.CHARACTER_CREATE)
	for k,v in pairs(Network.clients) do
		Network:send{client = v, packet = status}
	end
end

--- Subdivides the link between the region and another region.
-- @param self Generator class.
-- @param link Link to subdivide.
-- @return Link or nil.
Generator.subdivide_link = function(clss, link)
	clss:subdivide_link_recursive(link)
end

Generator.subdivide_link_recursive = function(clss, link)
	-- Select the type for the new region.
	local spec = Regionspec:random{category = "random"}
	if not spec then return end
	-- Calculate the subdivision.
	local src = link[1].point
	local dst = link[2].point
	local len = (dst - src).length
	if len - 2 * (link[1].size.length + link[2].size.length) < 1 then return end
	local rel = src + (dst - src) * (0.3 + 0.4 * math.random())
	-- Find the position for the new region.
	local ok = nil
	local pos = Vector()
	local size = Vector(spec.size[1], spec.size[2], spec.size[3])
	local aabb = Aabb{point = pos, size = size + Vector(10,10,10)}
	local dist = {0,math.ceil(0.3 * len)}
	for i = 1,20 do
		pos.x = math.random(dist[1], dist[2])
		pos.y = math.random(dist[1], dist[2])
		pos.z = math.random(dist[1], dist[2])
		if math.random(0, 1) == 1 then pos.x = -pos.x end
		if math.random(0, 1) == 1 then pos.y = -pos.y end
		if math.random(0, 1) == 1 then pos.z = -pos.z end
		pos = (pos + rel):floor()
		aabb.point = pos - Vector(5,5,5)
		if clss:validate_region_position(aabb) then
			ok = true
			break
		end
	end
	if not ok then return end
	-- Create the new region.
	local region = Region{aabb = aabb, point = pos, size = size, spec = spec}
	table.insert(clss.regions_dict_id, region)
	-- Link the new region to the path.
	local link1 = region:create_link(link[2])
	link[2] = region
	table.insert(clss.links, link1)
	-- Recursively subdivide the new paths.
	clss:subdivide_link_recursive(link)
	clss:subdivide_link_recursive(link1)
	return link1
end

--- Updates the network status while the generator is active.
-- @param clss Generator class.
Generator.update_network = function(clss)
	Network:update()
	while true do
		local event = Program:pop_event()
		if not event then break end
		if event.type == "login" then
			Generator:inform_clients(event.client)
		end
	end
end

--- Updates the status message of the generator.
-- @param clss Generator class.
-- @param frac Fraction of the task completed.
-- @param msg Message string.
Generator.update_status = function(clss, frac, msg)
	if msg then
		print(math.ceil(frac * 100) .. "% " .. msg)
		clss.prev_message = msg
		clss.prev_fraction = frac
		clss:inform_clients()
		clss:update_network()
	elseif frac == 1 or frac > clss.prev_fraction + 0.05 then
		print(math.ceil(frac * 100) .. "% " .. clss.prev_message)
		clss.prev_fraction = frac
		clss:inform_clients()
		clss:update_network()
	end
end
