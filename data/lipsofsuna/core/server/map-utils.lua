local Actor = require("core/objects/actor")
local Class = require("system/class")
local Item = require("core/objects/item")
local Obstacle = require("core/objects/obstacle")
local Sectors = require("system/sectors")
local Staticobject = require("core/objects/static")

--- Various helpers for populating the map.
-- @type MapUtils
MapUtils = Class("MapUtils")

--- Places a monster to the map.
-- @param clss MapUtils class.
-- @param args Arguments.<ul>
--   <li>category: Actorspec category.</li>
--   <li>class: Forces all objects to use the given class.</li>
--   <li>diffuculty: Maximum difficulty of the actor.</li>
--   <li>name: Actorspec name.</li>
--   <li>point: Position vector, in tiles.</li>
--   <li>rotation: Rotation around Y axis.</li></ul>
MapUtils.place_actor = function(clss, args)
	-- Choose the actor.
	-- The actor can by explicitly named or randomly selected from a specific
	-- category. Selection from a category can also optionally be limited by the
	-- maximum difficulty of the actor.
	local spec
	if args.category and args.difficulty then
		local cat = Actorspec:find(args)
		if not cat then return end
		local num = 0
		local opt = {}
		local dif = args.difficulty
		for k,v in pairs(cat) do
			if v.difficulty <= dif then
				num = num + 1
				opt[num] = v
			end
		end
		if num == 0 then return end
		spec = opt[math.random(1, num)]
	else
		spec = Actorspec:random(args)
		if not spec then return end
	end
	-- Spawn the actor.
	-- This needs to support both the client and the server so the class
	-- used varies depending on what's available.
	local clss_ = args.class or Actor or Object
	local obj = clss_(Main.objects)
	obj:set_spec(spec)
	if obj.randomize then
		obj:randomize()
	end
	obj:set_position(args.point)
	if args.rotation then obj:set_rotation(Quaternion{euler = {args.rotation * 2 * math.pi, 0, 0}}) end
	obj:set_visible(true)
end

--- Places an item to the map.
-- @param clss MapUtils class.
-- @param args Arguments.<ul>
--   <li>category: Item category.</li>
--   <li>class: Forces all objects to use the given class.</li>
--   <li>name: Item name.</li>
--   <li>point: Position vector, in tiles.</li>
--   <li>rotation: Rotation around Y axis.</li></ul>
MapUtils.place_item = function(clss, args)
	local spec = Itemspec:random(args)
	if not spec then return end
	local clss_ = args.class or Item or Object
	local obj = clss_(Main.objects)
	obj.random = true
	obj:set_spec(spec)
	obj:set_position(args.point)
	if args.rotation then obj:set_rotation(Quaternion{euler = {args.rotation * 2 * math.pi, 0, 0}}) end
	obj:set_visible(true)
end

--- Places an obstacle to the map.
-- @param clss MapUtils class.
-- @param args Arguments.<ul>
--   <li>class: Forces all objects to use the given class.</li>
--   <li>name: Obstacle name.</li>
--   <li>point: Position vector, in tiles.</li>
--   <li>rotation: Rotation around Y axis.</li></ul>
MapUtils.place_obstacle = function(clss, args)
	local spec = Obstaclespec:random(args)
	if not spec then return end
	local clss_ = args.class or Obstacle or Object
	local obj = clss_(Main.objects)
	obj:set_spec(spec)
	obj:set_position(args.point)
	if args.rotation then obj:set_rotation(Quaternion{euler = {args.rotation * 2 * math.pi, 0, 0}}) end
	obj:set_visible(true)
end

--- Places a static object to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>class: Forces all objects to use the given class.</li>
--   <li>name: Static object name.</li>
--   <li>point: Position vector, in tiles.</li>
--   <li>rotation: Rotation around Y axis.</li></ul>
MapUtils.place_static = function(clss, args)
	local spec = Staticspec:random(args)
	if not spec then return end
	local clss_ = args.class or Staticobject or Object
	local obj = clss_(Main.objects)
	obj:set_spec(spec)
	obj:set_position(args.point)
	if args.rotation then obj:set_rotation(Quaternion{euler = {args.rotation * 2 * math.pi, 0, 0}}) end
	obj:set_visible(true)
end

--- Places a predefined map pattern to the map.
-- @param clss Voxel class.
-- @param args Arguments.<ul>
--   <li>class: Forces all objects to use the given class.</li>
--   <li>name: Pattern name.</li>
--   <li>point: Position vector, in tiles.</li>
--   <li>rotation: Counter-clockwise rotation in 90 degree steps.</li></ul>
MapUtils.place_pattern = function(clss, args)
	local pat = Patternspec:random(args)
	if not pat then return end
	-- Initialize rotation
	local coord
	if args.rotation == 1 then
		coord = function(x,y,z) return Vector(z,y,x) end
	elseif args.rotation == 2 then
		coord = function(x,y,z) return Vector(pat.size.x-1-x,y,pat.size.z-1-z) end
	elseif args.rotation == 3 then
		coord = function(x,y,z) return Vector(pat.size.z-1-z,y,pat.size.x-1-x) end
	else
		coord = function(x,y,z) return Vector(x,y,z) end
	end
	-- Create tiles.
	for k,v in pairs(pat.tiles) do
		-- Get tile type.
		local tile = 0
		if v[4] then
			local mat = Material:find{name = v[4]}
			tile = mat and mat:get_id() or 0
		end
		if v[5] then
			-- Fill volume of tiles.
			for x = v[1],v[1]+v[5] do
				for y = v[2],v[2]+v[6] do
					for z = v[3],v[3]+v[7] do
						Voxel:set_tile(args.point + coord(x, y, z), tile)
					end
				end
			end
		else
			-- Fill individual tile.
			Voxel:set_tile(args.point + coord(v[1], v[2], v[3]), tile)
		end
	end
	-- Create obstacles.
	for k,v in pairs(pat.obstacles) do
		local point = args.point + coord(v[1], v[2], v[3])
		clss:place_obstacle{class = args.class, name = v[4], point = point, rotation = v[5]}
	end
	-- Create static objects.
	for k,v in pairs(pat.statics) do
		local point = args.point + coord(v[1], v[2], v[3])
		clss:place_static{class = args.class, name = v[4], point = point, rotation = v[5]}
	end
	-- Create items.
	for k,v in pairs(pat.items) do
		local point = args.point + coord(v[1], v[2], v[3])
		clss:place_item{class = args.class, name = v[4], point = point, rotation = v[5]}
	end
	-- Create actors.
	for k,v in pairs(pat.actors) do
		local point = args.point + coord(v[1], v[2], v[3])
		clss:place_actor{class = args.class, name = v[4], point = point, rotation = v[5]}
	end
end

return MapUtils
