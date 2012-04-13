require(Mod.path .. "spec")

Pattern = Class(Spec)
Pattern.type = "pattern"
Pattern.dict_id = {}
Pattern.dict_cat = {}
Pattern.dict_name = {}

Patternspec = Pattern --FIXME

--- Creates a new map pattern.
-- @param clss Pattern class.
-- @param args Arguments.<ul>
--   <li>creatures: Array of creatures to create.</li>
--   <li>distance: Distance to another pattern.</li>
--   <li>file: Path to the file where the pattern was specified. Nil for autodetect.</li>
--   <li>items: Array of items to create.</li>
--   <li>position: World map position, in tiles.</li>
--   <li>position_random: Position randomization, in tiles.</li>
--   <li>obstacles: Array of obstacles to create.</li>
--   <li>size: Pattern size in tiles.</li>
--   <li>spawn_point: Spawn point position, in tiles.</li>
--   <li>statics: Array of static objects to create.</li>
--   <li>tiles: Array of terrain tiles to create.</li></ul>
-- @return New map pattern.
Pattern.new = function(clss, args)
	local self = Spec.new(clss, args)
	if not self.file then
		self.file = Program:get_calling_file(4)
	end
	self.creatures = self.creatures or {}
	self.items = self.items or {}
	self.obstacles = self.obstacles or {}
	self.statics = self.statics or {}
	self.size = self.size or Vector(4,4,4)
	self.position = self.position or Vector(700,700,700)
	self.position_random = self.position_random or Vector()
	self.tiles = self.tiles or {}
	return self
end

--- Finds patterns that have a spawn point.
-- @param self Patternspec class.
-- @return Table of patterns.
Pattern.find_spawn_points = function(self)
	local list = {}
	for k,v in pairs(self.dict_id) do
		if v.spawn_point then
			table.insert(list, v)
		end
	end
	return list
end

--- Writes the pattern to a string.
-- @param self Pattern.
-- @return String.
Pattern.write = function(self)
	-- Format categories.
	local t = "Pattern{\n\tname = \"" .. self.name .. "\",\n\tsize = " .. tostring(self.size) .. ",\n"
	if self.distance then
		t = t .. string.format("\tdistance = {%q,%d,%d},\n", self.distance[1], self.distance[2], self.distance[3])
	end
	if self.position then
		t = t .. "\tposition = " .. tostring(self.position) .. ",\n"
	end
	if self.position_random then
		t = t .. "\tposition_random = " .. tostring(self.position) .. ",\n"
	end
	if self.spawn_point then
		t = t .. "\tspawn_point = " .. tostring(self.spawn_point) .. ",\n"
	end
	if self.categories then
		local categories = {}
		for k,v in pairs(self.categories) do
			table.insert(categories, k)
		end
		table.sort(categories)
		if #categories then
			t = t .. "\tcategories = {"
			local comma
			for k,v in ipairs(categories) do
				if comma then t = t .. "," end
				t = t .. "\"" .. v .. "\""
				comma = true
			end
			t = t .. "}"
		end
	end
	-- Format objects.
	local addobj = function(t, k, v)
		if v[5] then
			return string.format("%s%s\t\t{%f,%f,%f,%q,%.2f}",
				t, k > 1 and ",\n" or "", v[1], v[2], v[3], v[4], v[5])
		else
			return string.format("%s%s\t\t{%f,%f,%f,%q}",
				t, k > 1 and ",\n" or "", v[1], v[2], v[3], v[4])
		end
	end
	if #self.items > 0 then
		t = t .. ",\n\titems = {\n"
		for k,v in ipairs(self.items) do t = addobj(t, k, v) end
		t = t .. "}"
	end
	if #self.obstacles > 0 then
		t = t .. ",\n\tobstacles = {\n"
		for k,v in ipairs(self.obstacles) do t = addobj(t, k, v) end
		t = t .. "}"
	end
	if #self.statics > 0 then
		t = t .. ",\n\tstatics = {\n"
		for k,v in ipairs(self.statics) do t = addobj(t, k, v) end
		t = t .. "}"
	end
	if #self.creatures > 0 then
		t = t .. ",\n\tcreatures = {\n"
		for k,v in ipairs(self.creatures) do t = addobj(t, k, v) end
		t = t .. "}"
	end
	if #self.tiles > 0 then
		local y = -1
		local z = -1
		local comma
		t = t .. ",\n\ttiles = {\n"
		for k,v in ipairs(self.tiles) do
			local d
			if comma then
				d = (y ~= v[2] or z ~= v[3]) and ",\n\t\t" or ", "
			else
				d = "\t\t"
			end
			comma = true
			y,z = v[2],v[3]
			t = string.format("%s%s{%d,%d,%d,%q}", t, d, v[1], v[2], v[3], v[4])
		end
		t = t .. "}"
	end
	-- Finish the string.
	t = t .. "}\n"
	return t
end

Patternspec:add_getters{
	spawn_point_world = function(self)
		if not self.spawn_point then return end
		return (self.position + self.spawn_point) * Voxel.tile_size
	end}
