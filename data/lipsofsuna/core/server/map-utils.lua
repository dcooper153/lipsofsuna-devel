local Actor = require("core/objects/actor")
local Class = require("system/class")
local Item = require("core/objects/item")
local Obstacle = require("core/objects/obstacle")
local Staticobject = require("core/objects/static")

--- Various helpers for populating the map.
-- @type MapUtils
MapUtils = Class("MapUtils")

--- Places a monster to the map.
-- @param clss MapUtils class.
-- @param args Arguments.<ul>
--   <li>category: Actorspec category.</li>
--   <li>class: Forces all objects to use the given class.</li>
--   <li>difficulty: Maximum difficulty of the actor.</li>
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
		local cat = Main.specs:find_by_category("Actorspec", args.category)
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
	return obj
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
	obj:set_spec(spec)
	if obj.randomize then
		obj:randomize()
	end
	obj:set_position(args.point)
	if args.rotation then obj:set_rotation(Quaternion{euler = {args.rotation * 2 * math.pi, 0, 0}}) end
	obj:set_visible(true)
	return obj
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
	return obj
end

--- Places a static object to the map.
-- @param clss MapUtils class.
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
	return obj
end

return MapUtils
