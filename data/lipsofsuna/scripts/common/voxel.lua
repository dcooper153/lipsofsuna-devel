Voxel.tile_size = 32 / Voxel.tiles_per_line
Voxel.tile_scale = 1 / Voxel.tile_size

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

--- Places a monster to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>category: Species category.</li>
--   <li>name: Species name.</li>
--   <li>point: Position vector, in tiles.</li></ul>
Voxel.place_creature = function(clss, args)
	local spec = Species:random(args)
	if not spec then return end
	local clss_ = Creature or Object
	clss_.new(clss_, {
		spec = spec,
		position = args.point * clss.tile_size,
		random = true,
		realized = true})
end

--- Places an item to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>category: Item category.</li>
--   <li>name: Item name.</li>
--   <li>point: Position vector, in tiles.</li></ul>
Voxel.place_item = function(clss, args)
	local spec = Itemspec:random(args)
	if not spec then return end
	local clss_ = Item or Object
	clss_.new(clss_, {
		spec = spec,
		position = args.point * clss.tile_size,
		random = true,
		realized = true})
end

--- Places an obstacle to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>name: Obstacle name.</li>
--   <li>point: Position vector, in tiles.</li></ul>
Voxel.place_obstacle = function(clss, args)
	local spec = Obstaclespec:random(args)
	if not spec then return end
	local clss_ = Obstacle or Object
	clss_.new(clss_, {
		spec = spec,
		position = args.point * clss.tile_size,
		realized = true})
end

--- Places a predefined map pattern to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>name: Pattern name.</li>
--   <li>point: Position vector, in tiles.</li></ul>
Voxel.place_pattern = function(clss, args)
	local pat = Pattern:random(args)
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
		local point = args.point + Vector(v[1], v[2], v[3])
		clss:place_obstacle{name = v[4], point = point}
	end
	-- Create items.
	for k,v in pairs(pat.items) do
		local point = args.point + Vector(v[1], v[2], v[3])
		clss:place_item{name = v[4], point = point}
	end
	-- Create creatures.
	for k,v in pairs(pat.creatures) do
		local point = args.point + Vector(v[1], v[2], v[3])
		clss:place_creature{name = v[4], point = point}
	end
end
