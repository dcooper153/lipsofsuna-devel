local Actor = require("core/objects/actor")
local Class = require("system/class")
local Item = require("core/objects/item")
local Obstacle = require("core/objects/obstacle")
local Staticobject = require("core/objects/static")

--- Various helpers for populating the map.
-- @type MapUtils
local MapUtils = Class("MapUtils")

--- Places a monster to the map.
-- @param clss MapUtils class.
-- @param category Spec category.
-- @param point Position vector, in tiles.
-- @param rotation Rotation around Y axis. Nil for default.
-- @param difficulty Maximum difficulty of the actor. Nil for no preference.
MapUtils.place_actor_by_category = function(clss, category, point, rotation, difficulty)
	-- Find the actors in the category.
	local cat = Main.specs:find_by_category("ActorSpec", category)
	if not cat then return end
	-- Choose the actor.
	local num = 0
	local opt = {}
	local dif = difficulty or 1000000
	for k,v in pairs(cat) do
		if v.difficulty <= dif then
			num = num + 1
			opt[num] = v
		end
	end
	if num == 0 then return end
	local spec = opt[math.random(1, num)]
	-- Spawn the actor.
	local obj = Actor(Main.objects)
	obj:set_spec(spec)
	if obj.randomize then
		obj:randomize()
	end
	obj:set_position(point)
	if rotation then obj:set_rotation(Quaternion:new_from_euler(rotation * 2 * math.pi, 0, 0)) end
	obj:set_visible(true)
	return obj
end

--- Places a monster to the map.
-- @param clss MapUtils class.
-- @param name Spec name.
-- @param point Position vector, in tiles.
-- @param rotation Rotation around Y axis. Nil for default.
MapUtils.place_actor_by_name = function(clss, name, point, rotation)
	-- Choose the actor.
	local spec = Main.specs:find_by_name("ActorSpec", name)
	if not spec then return end
	-- Spawn the actor.
	local obj = Actor(Main.objects)
	obj:set_spec(spec)
	if obj.randomize then
		obj:randomize()
	end
	obj:set_position(point)
	if rotation then obj:set_rotation(Quaternion:new_from_euler(rotation * 2 * math.pi, 0, 0)) end
	obj:set_visible(true)
	return obj
end

--- Places an item to the map.
-- @param clss MapUtils class.
-- @param category Spec category.
-- @param point Position vector, in tiles.
-- @param rotation Rotation around Y axis. Nil for default.
MapUtils.place_item_by_category = function(clss, category, point, rotation)
	local spec = Main.specs:find_random_by_category("ItemSpec", category)
	if not spec then return end
	local obj = Item(Main.objects)
	obj:set_spec(spec)
	if obj.randomize then
		obj:randomize()
	end
	obj:set_position(args.point)
	if rotation then obj:set_rotation(Quaternion:new_from_euler(rotation * 2 * math.pi, 0, 0)) end
	obj:set_visible(true)
	return obj
end

--- Places an item to the map.
-- @param clss MapUtils class.
-- @param name Spec name.
-- @param point Position vector, in tiles.
-- @param rotation Rotation around Y axis. Nil for default.
MapUtils.place_item_by_name = function(clss, name, point, rotation)
	local spec = Main.specs:find_by_name(name)
	if not spec then return end
	local obj = Item(Main.objects)
	obj:set_spec(spec)
	if obj.randomize then
		obj:randomize()
	end
	obj:set_position(point)
	if rotation then obj:set_rotation(Quaternion:new_from_euler(rotation * 2 * math.pi, 0, 0)) end
	obj:set_visible(true)
	return obj
end

--- Places an obstacle to the map.
-- @param clss MapUtils class.
-- @param category Spec category.
-- @param point Position vector, in tiles.
-- @param rotation Rotation around Y axis. Nil for default.
MapUtils.place_obstacle_by_category = function(clss, category, point, rotation)
	local spec = Main.specs:find_random_by_category(category)
	if not spec then return end
	local obj = Obstacle(Main.objects)
	obj:set_spec(spec)
	obj:set_position(point)
	if rotation then obj:set_rotation(Quaternion:new_from_euler(rotation * 2 * math.pi, 0, 0)) end
	obj:set_visible(true)
	return obj
end

--- Places an obstacle to the map.
-- @param clss MapUtils class.
-- @param name Spec name.
-- @param point Position vector, in tiles.
-- @param rotation Rotation around Y axis. Nil for default.
MapUtils.place_obstacle_by_name = function(clss, name, point, rotation)
	local spec = Main.specs:find_by_name(name)
	if not spec then return end
	local obj = Staticobject(Main.objects)
	obj:set_spec(spec)
	obj:set_position(point)
	if rotation then obj:set_rotation(Quaternion:new_from_euler(rotation * 2 * math.pi, 0, 0)) end
	obj:set_visible(true)
	return obj
end

--- Places a static object to the map.
-- @param clss MapUtils class.
-- @param category Spec category.
-- @param point Position vector, in tiles.
-- @param rotation Rotation around Y axis. Nil for default.
MapUtils.place_static_by_category = function(clss, category, point, rotation)
	local spec = Main.specs:find_random_by_category(category)
	if not spec then return end
	local obj = Staticobject(Main.objects)
	obj:set_spec(spec)
	obj:set_position(point)
	if rotation then obj:set_rotation(Quaternion:new_from_euler(rotation * 2 * math.pi, 0, 0)) end
	obj:set_visible(true)
	return obj
end

--- Places a static object to the map.
-- @param clss MapUtils class.
-- @param name Spec name.
-- @param point Position vector, in tiles.
-- @param rotation Rotation around Y axis. Nil for default.
MapUtils.place_static_by_name = function(clss, name, point, rotation)
	local spec = Main.specs:find_by_name(name)
	if not spec then return end
	local obj = Staticobject(Main.objects)
	obj:set_spec(spec)
	obj:set_position(point)
	if rotation then obj:set_rotation(Quaternion:new_from_euler(rotation * 2 * math.pi, 0, 0)) end
	obj:set_visible(true)
	return obj
end

return MapUtils
