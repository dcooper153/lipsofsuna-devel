require(Mod.path .. "region")

Generator = Class()
Generator.class_name = "Generator"
Generator.map_size = Vector(1000, 1000, 1000)
Generator.map_start = Vector(600, 600, 600) - Generator.map_size * 0.5
Generator.map_end = Vector(600, 600, 600) + Generator.map_size * 0.5
Generator.map_version = "7"
Generator.layer_offset = 1600
Generator.layer_count = 20
Generator.layer_size = Generator.map_size.y / Generator.layer_count
Generator.bin_size = 6
Generator.bin_stride = 512
Generator.bin_stride2 = 512*512
Generator.sector_types = {}

--- Creates a new map generator.
-- @param clss Generator class.
-- @return Generator.
Generator.new = function(clss)
	local self = Class.new(Generator)
	-- Initialize random seeds.
	self.seed1 = math.random(10000, 60000)
	self.seed2 = math.random(10000, 60000)
	self.seed3 = math.random(10000, 60000)
	-- Initialize sector generators.
	for k,v in pairs(self.sector_types) do
		v:init()
	end
	-- Reset the status.
	self:reset()
	return self
end

Generator.add_region = function(self, region)
	-- Store by name.
	table.insert(self.regions_dict_id, region)
	self.regions_dict_name[region.spec.name] = region
	-- Store by layers.
	local min,max = region:get_layer_range()
	for i=min,max do
		local dict = self.regions_dict_layer[i]
		table.insert(dict, region)
	end
	-- Add to the space partitioning table.
	self:for_each_bin(region.point, region.size, function(i)
		self.bins[i] = true
	end)
end

Generator.for_each_bin = function(self, point, size, func)
	local p = point:copy():divide(self.bin_size):floor()
	local s = size:copy():divide(self.bin_size):ceil()
	for x = p.x,p.x+s.x do
		for y = p.y,p.y+s.y do
			for z = p.z,p.z+s.z do
				func(x + self.bin_stride * y + self.bin_stride2 * z)
			end
		end
	end
end

--- Generates the world map.
-- @param self Generator.
-- @param args Arguments.
Generator.generate = function(self, args)
	local place_regions = function()
		-- Get the list of patterns.
		-- The list is sorted so that regions with strictest constraints are
		-- placed first. This reduces the risk of running into a dead end.
		local special = {}
		for k,v in pairs(Patternspec:find{category = "init"}) do
			table.insert(special, v)
		end
		table.sort(special, function(a,b) return a.position_random.length < b.position_random.length end)
		-- Generate regions for the patterns.
		while true do
			local placed = 0
			local skipped = 0
			-- Try to place one or more regions.
			-- Regions are often placed relative to each other so we need to
			-- add them iteratively in the other of least dependencies.
			for k,pat in pairs(special) do
				if not self.regions_dict_name[pat.name] then
					if not self:place_region(pat) then
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
	self:update_status(0, "Resetting world")
	Marker:reset()
	Sectors.instance:unload_world()
	-- Place special areas.
	-- Regions have dependencies so we need to place them in several passes.
	-- The region tables are filled but the map is all empty after this.
	repeat
		self:reset()
		self:update_status(0, "Placing regions")
	until place_regions()
	-- Place dungeons.
	self:generate_dungeons()
	-- Mark roads.
	--[[self:update_status(0, "Creating roads")
	local linkn = 0
	for _,reg1 in pairs(self.regions_dict_name) do
		for _,name in ipairs(reg1.spec.links) do
			local reg2 = self.regions_dict_name[name]
			local link = reg1:create_link(reg2)
			linkn = linkn + 1
			self.links[linkn] = link
		end
	end
	for i,link in ipairs(self.links) do
		local src = link[1]:get_link_point(link[2].point)
		local dst = link[2]:get_link_point(link[1].point)
		self:mark_road(src, dst)
		self:update_status(i / linkn)
	end]]
	-- Add map markers for regions.
	-- These are used by location discovery.
	for _,reg in pairs(self.regions_dict_id) do
		local p = reg.point + reg.pattern.size * 0.5
		Marker{name = reg.spec.name, discoverable = true, position = p * Voxel.tile_size}
	end
	-- Format regions.
	-- This creates the sectors that contain special areas.
	self:update_status(0, "Formatting regions")
	for _,reg in pairs(self.regions_dict_id) do
		Voxel:fill_region{point = reg.point, size = reg.pattern.size}
	end
	-- Find used sectors.
	self:update_status(0, "Counting sectors")
	local sectorn = 0
	local sectors = Program.sectors
	for k in pairs(sectors) do
		if not self.sectors[k] then
			self.sectors[k] = "Town"
		end
		sectorn = sectorn + 1
	end
	-- Create fractal terrain.
	self:update_status(0, "Randomizing terrain")
	local index = 0
	for k in pairs(sectors) do
		self:generate_sector(k)
		self:update_status(index / sectorn)
		index = index + 1
	end
	-- Paint regions.
	self:update_status(0, "Creating regions")
	for _,reg in pairs(self.regions_dict_id) do
		Voxel:fill_region{point = reg.point, size = reg.pattern.size}
		Voxel:place_pattern{point = reg.point, name = reg.pattern.name}
	end
	-- Generate static objects.
	self:generate_overworld()
	local statics = {}
	for k,v in pairs(Staticobject.dict_id) do
		statics[k] = v
	end
	-- Save the map.
	self:update_status(0, "Saving the map")
	Serialize:clear_objects()
	Sectors.instance:save_world(true, function(p) self:update_status(p) end)
	Sectors.instance:unload_world()
	for k,v in pairs(statics) do v.realized = true end
	Serialize:save_static_objects()
	Serialize:set_value("map_version", Generator.map_version)
	-- Save map markers.
	self:update_status(0, "Saving quests")
	Serialize:save_generator(true)
	Serialize:save_markers(true)
	Serialize:save_quests(true)
	Serialize:save_accounts(true)
	Serialize:clear_world_object_decay()
	-- Discard events emitted during map generation so that they
	-- don't trigger when the game starts.
	self:update_status(0, "Finishing")
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

--- Generates the static overworld objects.
-- @param self Generator.
Generator.generate_overworld = function(self)
	local gen = function(name, count)
		-- FIXME: Generate static overworld obstacles properly.
		local spec = Staticspec:find{name = name}
		if not spec then return end
		for i = 1,count do
			for j = 1,10 do
				local point = self:find_overworld_generation_point()
				if point and self:validate_rect(point * Voxel.tile_scale - Vector(2,0,2), Vector(4,8,4), true) then
					local rot = Quaternion{axis = Vector(0,1), angle = 2.0 * math.pi * math.random()}
					Staticobject{spec = spec, position = point, rotation = rot, realized = true}
					break
				end
			end
		end
	end
	gen("statictree1", 100)
	gen("statichouse2", 20)
	gen("obelisk", 15)
end

Generator.find_overworld_generation_point = function(self)
	for i = 1,10 do
		local min = Map.aabb.point
		local max = Map.aabb.point + Map.aabb.size
		local point = Vector(math.random(min.x, max.x), 0, math.random(min.z, max.z))
		local height = Map.heightmap:get_height(point, false)
		if height then
			point.y = height
			return point
		end
	end
end

Generator.generate_dungeons = function(self)
	local cat = Patternspec:find{category = "dungeon entrance"}
	if not cat then return end
	for k,v in pairs(cat) do
		for i = 1,10 do
			if self:generate_dungeon(v) then break end
		end
	end
end

Generator.generate_dungeon = function(self, pattern)
	-- Find the counterpart pattern.
	local name2 = string.gsub(pattern.name, "entrance", "exit")
	local pattern2 = Patternspec:find{name = name2}
	if not pattern2 then return end
	-- Find the overworld point.
	local point1 = self:find_overworld_generation_point()
	point1:multiply(Voxel.tile_scale):subtract_xyz(0.5*pattern.size.x,0,0.5*pattern.size.z):ceil()
	if not self:validate_pattern_position(pattern, point1, true) then return end
	if not point1 then return end
	-- Place the underground pattern.
	local point2 = Vector(point1.x, 807, point1.z)
	if not self:validate_pattern_position(pattern2, point2) then return end
	-- Generate the patterns.
	self:create_region(pattern, point1)
	self:create_region(pattern2, point2)
	-- TODO: Choose the dungeon type.
	local dungeon = "Labyrinth"
	local dungeon_boss = dungeon .. "Boss"
	-- Mark sectors surrounding the exit.
	-- TODO: Size management.
	local offset = self:get_sector_offset_by_tile(point2)
	for z = -3,1 do
		for x = -2,2 do
			self:set_sector_type_by_offset(offset + Vector(x,-1,z), dungeon)
		end
	end
	self:set_sector_type_by_offset(offset + Vector(0,-1,-2), dungeon_boss)
	return true
end

--- Creates ore deposits and places plants.
-- @param self Sectors.
-- @param pos Position in tiles.
-- @param size Size in tiles.
-- @param amount How many resources to create at most.
Generator.generate_resources = function(self, pos, size, amount)
	-- /FIXME
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
	local points = {
		Vector(-1,-1,-1), Vector(0,-1,-1), Vector(1,-1,-1),
		Vector(-1,0,-1), Vector(0,0,-1), Vector(1,0,-1),
		Vector(-1,1,-1), Vector(0,1,-1), Vector(1,1,-1),
		Vector(-1,-1,0), Vector(0,-1,0), Vector(1,-1,0),
		Vector(-1,0,0), Vector(0,0,0), Vector(1,0,0),
		Vector(-1,1,0), Vector(0,1,0), Vector(1,1,0),
		Vector(-1,-1,1), Vector(0,-1,1), Vector(1,-1,1),
		Vector(-1,0,1), Vector(0,0,1), Vector(1,0,1),
		Vector(-1,1,1), Vector(0,1,1), Vector(1,1,1)}
	local create_plant_or_item = function(ctr)
		local i = 0
		repeat
			i = i + 1
			ctr.y = ctr.y - 1
			if i > 10 then return end
		until Voxel:get_tile(ctr) ~= 0
		ctr.y = ctr.y + 1
		local src = ctr + Vector(-1,1,-1)
		local dst = ctr + Vector(1,3,1)
		ctr = ctr + Vector (0.5, 0, 0.5)
		if math.random() < 0.1 then
			Voxel:place_item{point = ctr, category = "generate"}
		elseif math.random() < 0.4 and Voxel:check_range(src, dst).solid == 0 then
			Voxel:place_obstacle{point = ctr, category = "tree"}
		else
			Voxel:place_obstacle{point = ctr, category = "small-plant"}
		end
	end
	local create_vein = function(ctr, mat)
		for k,v in pairs(points) do
			if math.random() < 0.3 then
				local t = Voxel:get_tile(ctr + v)
				if t ~= 0 then
					Voxel:set_tile(ctr + v, mat)
				end
			end
		end
	end
	-- Generate resources.
	local p = Vector()
	local n = amount or 10
	for i=1,n do
		p.x = pos.x + math.random(1, size.x - 2)
		p.y = pos.y + math.random(1, size.y - 2)
		p.z = pos.z + math.random(1, size.z - 2)
		local t = Voxel:get_tile(p)
		if t ~= 0 then
			local m = mats[math.random(1,#mats)]
			if m then create_vein(p, m.id, 1) end
		else
			create_plant_or_item(p)
		end
	end
end

Generator.generate_sector = function(self, id)
	self.sector_types.Main:generate(id)
end

--- Gets the ID of the sector.
-- @param self Generator.
-- @param sector Sector offset in tiles.
-- @return Vector.
Generator.get_sector_id = function(self, tile)
	local w = 128
	local s = tile:copy():round():divide(Voxel.tiles_per_line):floor()
	return s.x + s.y * w + s.z * w^2
end

--- Gets the offset of the sector in tiles.
-- @param self Generator.
-- @param sector Sector index.
-- @return Vector.
Generator.get_sector_offset = function(self, sector)
	local w = 128
	local sx = sector % w
	local sy = math.floor(sector / w) % w
	local sz = math.floor(sector / w / w) % w
	return Vector(sx, sy, sz) * Voxel.tiles_per_line
end

--- Informs clients of the generator status.
-- @param self Generator.
-- @param client Specific client to inform or nil to inform all.
-- @return Network packet.
Generator.inform_clients = function(self, client)
	local p = Packet(packets.GENERATOR_STATUS,
		"string", self.prev_message or "",
		"float", self.prev_fraction or 0)
	if client then
		Network:send{client = client, packet = p}
	else
		for k,v in pairs(Network.clients) do
			Network:send{client = v, packet = p}
		end
	end
end

--- Marks road sectors.
-- @param self Generator.
-- @param src Source point in tiles.
-- @param dst Destination point in tiles.
Generator.mark_road = function(self, src, dst)
	local dist
	local ssec = src:copy():divide(Voxel.tiles_per_line):round()
	local dsec = dst:copy():divide(Voxel.tiles_per_line):round()
	local psec = ssec:copy()
	repeat
		-- Mark as road.
		local sec = self:get_sector_id(psec * Voxel.tiles_per_line)
		self.sectors[sec] = "Road"
		dist = (dsec - psec):round()
		if dist.length < 1 then break end
		-- Move horizontally.
		if math.abs(dist.x) > math.abs(dist.z) then
			if dist.x > 0 then
				psec.x = psec.x + 1
			else
				psec.x = psec.x - 1
			end
		else
			if dist.z > 0 then
				psec.z = psec.z + 1
			else
				psec.z = psec.z - 1
			end
		end
		-- Move vertically.
		-- FIXME: All in the beginning.
		if dist.y < -0.5 then
			psec.y = psec.y - 1
		elseif dist.y > 0.5 then
			psec.y = psec.y + 1
		end
	until false
	-- Mark the endpoints.
	self.sectors[self:get_sector_id(src)] = "Road"
	self.sectors[self:get_sector_id(dst)] = "Road"
end

--- Draws a corridor in the map.
-- @param self Generator.
-- @param src Source point in tiles.
-- @param dst Destination point in tiles.
Generator.paint_corridor = function(self, src, dst)
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

Generator.create_region = function(self, pattern, point)
	local aabb = Aabb{point = point, size = pattern.size}
	local region = Region{aabb = aabb, pattern = pattern, point = point, size = pattern.size, spec = pattern}
	self:add_region(region)
	return region
end

--- Places a region to the map.
-- @param self Generator.
-- @param pat Pattern spec.
-- @return Region or nil.
Generator.place_region = function(self, pat)
	local size = pat.size
 	-- Determine the approximate position.
	-- Regions can be placed randomly or relative to each other. Here we
	-- decide the range of positions that are valid for the region.
	local rel = nil
	local dist = nil
	if pat.position then
		rel = Vector(pat.position.x, 0, pat.position.z)
	elseif not pat.distance_pattern then
		rel = Vector(math.random(self.map_start.x, self.map_end.x), 0, math.random(self.map_start.z, self.map_end.z))
		dist = {nil, 0.1 * self.map_size.x, 0.1 * self.map_size.x}
	elseif self.regions_dict_name[pat.distance_pattern] then
		rel = self.regions_dict_name[pat.distance_pattern].point
		rel = Vector(rel.x, 0, rel.z)
		dist = {pat.distance_pattern, pat.distance_min, pat.distance_max}
	else return end
	-- Determine the final position.
	-- Regions with a random position range are placed at different
	-- positions until one is found where they fit. Regions with fixed
	-- positions are simply placed there without any checks.
	local pos = Vector()
	local var = pat.position_random.y
	if dist then
		local success
		for retry=1,50 do
			pos.x = math.random(dist[2], dist[3])
			pos.y = pat.position.y + math.random(-var, var)
			pos.z = math.random(dist[2], dist[3])
			if math.random(0, 1) == 1 then pos.x = -pos.x end
			if math.random(0, 1) == 1 then pos.z = -pos.z end
			pos = pos + rel
			if self:validate_pattern_position(pat, pos) then
				success = true
				break
			end
		end
		if not success then return end
	else
		local success
		local varx = pat.position_random.x
		local varz = pat.position_random.z
		for retry=1,50 do
			pos.x = rel.x + math.random(-varx, varx)
			pos.y = pat.position.y + math.random(-var, var)
			pos.z = rel.z + math.random(-varz, varz)
			if self:validate_pattern_position(pat, pos, pat.overworld) then
				success = true
				break
			end
		end
		if not success then return end
	end
	-- Create the region.
	return self:create_region(pat, pos)
end

Generator.reset = function(self)
	self.bins = {}
	self.links = {}
	self.regions_dict_id = {}
	self.regions_dict_name = {}
	self.regions_dict_layer = {}
	for i=1,self.layer_count do
		self.regions_dict_layer[i] = {}
	end
	self.sectors = {}
end

--- Updates the network status while the generator is active.
-- @param self Generator.
Generator.update_network = function(self)
	Network:update()
	while true do
		local event = Program:pop_event()
		if not event then break end
		if event.type == "login" then
			self:inform_clients(event.client)
		end
	end
end

--- Updates the status message of the generator.
-- @param self Generator.
-- @param frac Fraction of the task completed.
-- @param msg Message string.
Generator.update_status = function(self, frac, msg)
	if msg then
		print(math.ceil(frac * 100) .. "% " .. msg)
		self.prev_message = msg
		self.prev_fraction = frac
		self:inform_clients()
		self:update_network()
	elseif frac == 1 or frac > self.prev_fraction + 0.05 then
		print(math.ceil(frac * 100) .. "% " .. self.prev_message)
		self.prev_fraction = frac
		self:inform_clients()
		self:update_network()
	end
end

--- Checks if the pattern can be placed in the given position.
-- @param self Generator.
-- @param pattern Pattern spec.
-- @param point Position in tile space.
-- @param overworld True if overworld placement is allowed.
-- @return True if the position is valid.
Generator.validate_pattern_position = function(self, pattern, point, overworld)
	local point1 = Vector(point.x - 1, point.y - 1, point.z - 1)
	local size = pattern.size + Vector(2,2,2)
	return self:validate_rect(point1, size, overworld)
end

Generator.validate_rect = function(self, pos, size, overworld)
	if pos.x < self.map_start.x then return end
	if pos.y < self.map_start.y then return end
	if pos.z < self.map_start.z then return end
	if pos.x + size.x > self.map_end.x then return end
	if pos.z + size.z > self.map_end.z then return end
	if not overworld then
		if pos.y + size.y > self.map_end.y then return end
	else
		if pos.y + size.y > 1400 then return end
	end
	local hit
	self:for_each_bin(pos, size, function(i)
		hit = hit or self.bins[i]
	end)
	return not hit
end

------------------------------------------------------------------------------
-- TODO: Move to utils

Generator.get_sector_center_by_id = function(self, id)
	return self:get_sector_offset_by_id(id):add_xyz(0.5,0.5,0.5):
		multiply(Voxel.tiles_per_line * Voxel.tile_size)
end

Generator.get_sector_type_by_id = function(self, id)
	return self.sectors[id]
end

Generator.get_sector_id_by_offset = function(self, offset)
	local w = 128
	local s = offset:copy():floor()
	return s.x + s.y * w + s.z * w^2
end

Generator.get_sector_id_by_point = function(self, point)
	return self:get_sector_id_by_offset(point * Voxel.tile_scale * (1 / Voxel.tiles_per_line))
end

Generator.get_sector_id_by_tile = function(self, tile)
	return self:get_sector_id_by_offset(tile * (1 / Voxel.tiles_per_line))
end

Generator.get_sector_offset_by_id = function(self, id)
	local w = 128
	local sx = id % w
	local sy = math.floor(id / w) % w
	local sz = math.floor(id / w / w) % w
	return Vector(sx, sy, sz)
end

Generator.get_sector_offset_by_point = function(self, tile)
	return tile:copy():multiply(Voxel.tile_scale):divide(Voxel.tiles_per_line):round()
end

Generator.get_sector_offset_by_tile = function(self, tile)
	return tile:copy():divide(Voxel.tiles_per_line):round()
end

Generator.get_sector_tile_by_id = function(self, id)
	return self:get_sector_offset_by_id(id):multiply(Voxel.tiles_per_line)
end

Generator.set_sector_type_by_id = function(self, id, type)
	self.sectors[id] = type
end

Generator.set_sector_type_by_offset = function(self, offset, type)
	self:set_sector_type_by_id(self:get_sector_id_by_offset(offset), type)
end

Generator.is_overworld_sector_by_id = function(self, id)
	local tile = self:get_sector_tile_by_id(id)
	return tile.y > 1000
end

Generator.is_overworld_sector_by_point = function(self, point)
	local tile_y = point.y * Voxel.tile_scale
	return tile_y > 1000
end
