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

--- Gets the depth layer range of the region.
-- @param self Region.
-- @return First layer, last layer.
Region.get_layer_range = function(self)
	local layer = (self.point.y - Generator.layer_offset) / Generator.layer_size
	local min = math.max(1, layer)
	local max = math.max(0, math.min(Generator.layer_count, layer + 2))
	return math.floor(min), math.ceil(max) 
end

Region.get_link_point = function(self, dst)
	local ctr = self.point + self.size * 0.5
	local dir = (dst - ctr):normalize()
	if math.abs(dir.x) < math.abs(dir.z) then
		dir.y = -1
		dir.z = dir.z < 0 and -1 or 1
	else
		dir.x = dir.x < 0 and -1 or 1
		dir.y = -1
	end
	local pt = ctr + Vector(dir.x * self.size.x, dir.y * self.size.y, dir.z * self.size.z) * 0.5 + Vector(0,3)
	return pt:floor()
end

------------------------------------------------------------------------------

Generator = Class()
Generator.map_size = Vector(1000, 1000, 1000)
Generator.map_start = Vector(600, 600, 600) - Generator.map_size * 0.5
Generator.map_end = Vector(600, 600, 600) + Generator.map_size * 0.5
Generator.map_version = "5"
Generator.layer_offset = 2500
Generator.layer_count = 20
Generator.layer_size = Generator.map_size.y / Generator.layer_count
Generator.bin_size = 6
Generator.bin_stride = 512
Generator.bin_stride2 = 512*512

Generator.add_region = function(clss, region)
	-- Store by name.
	table.insert(clss.regions_dict_id, region)
	clss.regions_dict_name[region.spec.name] = region
	-- Store by layers.
	local min,max = region:get_layer_range()
	for i=min,max do
		local dict = clss.regions_dict_layer[i]
		table.insert(dict, region)
	end
	-- Add to the space partitioning table.
	clss:for_each_bin(region.point, region.size, function(i)
		clss.bins[i] = true
	end)
end

Generator.for_each_bin = function(clss, point, size, func)
	local p = (point * (1 / clss.bin_size)):floor()
	local s = (size * (1 / clss.bin_size)):ceil()
	for x = p.x,p.x+s.x do
		for y = p.y,p.y+s.y do
			for z = p.z,p.z+s.z do
				func(x + clss.bin_stride * y + clss.bin_stride2 * z)
			end
		end
	end
end

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
	local vec = dst - src
	local abs = Vector(math.abs(vec.x), math.abs(vec.y), math.abs(vec.z))
	-- Draw the tunnel line.
	if abs.x >= abs.y and abs.x >= abs.z then
		-- Walk along the X axis.
		local step = vec * (1/abs.x)
		for x=0,abs.x do
			Voxel:place_pattern{category = "corridorx", point = (src + step * x):floor()}
		end
	elseif abs.y >= abs.z then
		-- Walk along the Y axis.
		local step = vec * (1/abs.y)
		for y=0,abs.y do
			Voxel:place_pattern{category = "corridory", point = (src + step * y):floor()}
		end
	else
		-- Walk along the Z axis.
		local step = vec * (1/abs.z)
		for z=0,abs.z do
			Voxel:place_pattern{category = "corridorz", point = (src + step * z):floor()}
		end
	end
	-- Draw the endpoints.
	Voxel:place_pattern{category = "corridor", point = src}
	Voxel:place_pattern{category = "corridor", point = dst}
end

--- Places a region to the map.
-- @param clss Generator class.
-- @param reg Region spec.
-- @param pat Pattern spec.
-- @return Region or nil.
Generator.place_region = function(clss, reg, pat)
	if not pat then
		error(string.format("ERROR: No pattern was found for region `%s'.", reg.name))
	end
	local size = pat.size
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
	local aabb = Aabb{point = pos, size = size + Vector(2,2,2)}
	if dist then
		local success
		for retry=1,50 do
			pos.x = math.random(dist[2], dist[3])
			pos.y = math.random(reg.depth[1], reg.depth[2])
			pos.z = math.random(dist[2], dist[3])
			if math.random(0, 1) == 1 then pos.x = -pos.x end
			if math.random(0, 1) == 1 then pos.z = -pos.z end
			pos = pos + rel
			aabb.point = pos - Vector(1,1,1)
			if clss:validate_region_position(aabb) then
				success = true
				break
			end
		end
		if not success then return end
	else
		pos.x = rel.x
		pos.y = math.random(reg.depth[1], reg.depth[2])
		pos.z = rel.z
		aabb.point = pos - Vector(5,5,5)
	end
	-- Create the region.
	local region = Region{aabb = aabb, pattern = pat, point = pos, size = size, spec = reg}
	clss:add_region(region)
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
	local hit
	clss:for_each_bin(aabb.point, aabb.size, function(i)
		hit = hit or clss.bins[i]
	end)
	return not hit
end

--- Generates the world map.
-- @param clss Generator class.
-- @param args Arguments.
Generator.generate = function(clss, args)
	local reset_state = function()
		-- Initialize state.
		clss.bins = {}
		clss.links = {}
		clss.regions_dict_id = {}
		clss.regions_dict_name = {}
		clss.regions_dict_layer = {}
		for i=1,clss.layer_count do
			clss.regions_dict_layer[i] = {}
		end
	end
	local place_regions = function()
		local special = Regionspec:find{category = "special"}
		while true do
			local placed = 0
			local skipped = 0
			-- Try to place one or more regions.
			-- Regions are often placed relative to each other so we need to
			-- add them iteratively in the other of least dependencies.
			for name,reg in pairs(special) do
				if not clss.regions_dict_name[reg.name] then
					local pat = Pattern:random{category = reg.pattern_category, name = reg.pattern_name}
					if not clss:place_region(reg, pat) then
						skipped = skipped + 1
					else
						placed = placed + 1
					end
				end
			end
			-- The generator may sometimes run into a dead end where a region
			-- can't be placed without violating the constraints. In such a
			-- case, nil is returned and generation is restarted from scratch.
			if placed == 0 then return end
			if skipped == 0 then break end
		end
		return true
	end
	-- Remove all player characters.
	for k,v in pairs(Player.clients) do
		v:detach(true)
	end
	Player.clients = {}
	-- Reset the world.
	clss:update_status(0, "Resetting world")
	Marker:reset()
	Sectors.instance:unload_world()
	-- Place special areas.
	-- Regions have dependencies so we need to place them in several passes.
	-- The region tables are filled but the map is all empty after this.
	repeat
		reset_state()
		clss:update_status(0, "Placing regions")
	until place_regions()
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
	for layer = 1,clss.layer_count do
		local dict = clss.regions_dict_layer[layer]
		local count = #dict
		if count > 1 then
			local retry = 0
			local created = 0
			while retry < 2 and created < 4 do
				local reg1 = dict[math.random(1, count)]
				local reg2 = dict[math.random(1, count)]
				if reglink(reg1, reg2) then
					created = created + 1
				else
					retry = retry + 1
				end
			end
		end
		clss:update_status(layer / clss.layer_count)
	end
	linkn = #clss.links
	-- Paint regions.
	clss:update_status(0, "Creating regions")
	for _,reg in pairs(clss.regions_dict_id) do
		Voxel:fill_region{point = reg.point, size = reg.pattern.size}
		Voxel:place_pattern{point = reg.point, name = reg.pattern.name}
	end
	-- Paint corridors.
	clss:update_status(0, "Creating corridors")
	for i,link in ipairs(clss.links) do
		local src = link[1]:get_link_point(link[2].point)
		local dst = link[2]:get_link_point(link[1].point)
		clss:paint_corridor(src, dst)
		clss:update_status(i / linkn)
	end
	-- Find used sectors.
	clss:update_status(0, "Counting sectors")
	local sectorn = 0
	local sectors = Program.sectors
	for k in pairs(sectors) do
		sectorn = sectorn + 1
	end
	-- Create fractal terrain.
	clss:update_status(0, "Randomizing terrain")
	local index = 0
	for k in pairs(sectors) do
		Sectors.instance:create_fractal_regions(k,
			function(aabb) return clss:validate_region_position(aabb) end)
		clss:update_status(index / sectorn)
		index = index + 1
	end
	-- Randomize tiles.
	clss:update_status(0, "Creating resource deposits")
	index = 0
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
	clss:update_status(0, "Saving quests")
	Serialize:save_markers(true)
	Serialize:save_quests(true)
	Serialize:save_accounts(true)
	-- Discard events emitted during map generation so that they
	-- don't trigger when the game starts.
	clss:update_status(0, "Finishing")
	Program:update()
	repeat until not Program:pop_event()
	Program:update()
	repeat until not Program:pop_event()
	-- Inform players of the generation being complete.
	-- All accounts were erased so clients need to re-authenticate.
	local status = Packet(packets.CLIENT_AUTHENTICATE)
	for k,v in pairs(Network.clients) do
		Network:send{client = v, packet = status}
	end
end

--- Subdivides the link between the region and another region.
-- @param self Generator class.
-- @param link Link to subdivide.
-- @return Link or nil.
Generator.subdivide_link = function(clss, link)
	-- Check if the path is long enough.
	local src = link[1].point + link[1].size * 0.5
	local dst = link[2].point + link[2].size * 0.5
	local len = (src - dst).length - (link[1].size.length + link[2].size.length) / 2
	if len < 50 then return end
	-- Try to subdivide the link.
	for i = 1,20 do
		-- Select the type for the new region.
		local spec = Regionspec:random{category = "random"}
		if not spec then return end
		local pattern = Pattern:random{category = spec.pattern_category, name = spec.pattern_name}
		if not pattern then return end
		-- Check if the pattern fits.
		local region = clss:subdivide_link_test(link, spec, pattern)
		if region then
			-- Link the new region to the path.
			clss:add_region(region)
			local link1 = region:create_link(link[2])
			link[2] = region
			table.insert(clss.links, link1)
			-- Recursively subdivide the new paths.
			clss:subdivide_link(link)
			clss:subdivide_link(link1)
			return link1
		end
	end
end

Generator.subdivide_link_test = function(clss, link, spec, pattern)
	-- Calculate the subdivision center.
	local src = link[1].point
	local dst = link[2].point
	local rel = src + (dst - src) * (0.3 + 0.4 * math.random())
	-- Calculate the direction vector.
	local forward = (dst - src):normalize()
	local side = forward:cross(Vector(0,1)):normalize()
	-- Find the position for the new region.
	local ok = nil
	local pos = Vector()
	local size = pattern.size
	local aabb = Aabb{point = pos, size = size}
	local dist = math.ceil((dst - src).length)
	for i = 1,10 do
		pos = side * 0.5 * math.random(-dist, dist)
		pos = (pos + rel):floor()
		aabb.point = pos
		if clss:validate_region_position(aabb) then
			return Region{aabb = aabb, pattern = pattern, point = pos, size = size, spec = spec}
		end
	end
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
