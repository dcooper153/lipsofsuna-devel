Unittest:add(1, "system", "vision", function()
	local Class = require("system/class")
	local Eventhandler = require("system/eventhandler")
	local Object = require("system/object")
	local Vector = require("system/math/vector")
	local Vision = require("system/vision")
	local evt
	local update = function(vis)
		local evt = vis:update()
		assert(#evt == 0)
		Eventhandler:update()
		return vis:update()
	end
	local objects = Class("ObjectManager")
	objects.dict = setmetatable({}, {__mode = "kv"})
	objects.add = function(self, obj) self.dict[obj:get_id()] = obj end
	objects.find_by_id = function(self, id) return self.dict[id] end
	-- Initialization.
	Eventhandler:update()
	local vis = Vision(0, objects)
	vis:set_enabled(true)
	vis:set_position(Vector(100,200,300))
	vis:set_radius(2)
	vis:set_threshold(1)
	assert(vis:get_radius() == 2)
	assert(vis:get_threshold() == 1)
	-- Creating unrealized objects.
	local obj = Object()
	obj:set_id(1)
	obj:set_position(Vector(100,200,300))
	objects:add(obj)
	evt = update(vis)
	assert(#evt == 0)
	-- Realizing within the sphere.
	obj:set_visible(true)
	evt = update(vis)
	assert(#evt == 1)
	assert(evt[1].type == "object-shown")
	assert(evt[1].object == obj)
	assert(vis.objects[obj])
	-- Moving within the sphere.
	obj:set_position(obj:get_position() + Vector(1.8))
	evt = update(vis)
	assert(#evt == 0)
	-- Moving within the keep threshold.
	obj:set_position(obj:get_position() + Vector(1))
	evt = update(vis)
	assert(#evt == 0)
	-- Leaving the sphere.
	obj:set_position(obj:get_position() + Vector(1.2))
	evt = update(vis)
	assert(#evt == 1)
	assert(evt[1].type == "object-hidden")
	assert(evt[1].object == obj)
	assert(not vis.objects[obj])
	-- Moving outside the sphere.
	obj:set_position(obj:get_position() - Vector(1.5))
	evt = update(vis)
	assert(#evt == 0)
	-- Moving into the sphere.
	obj:set_position(obj:get_position() - Vector(1))
	evt = update(vis)
	assert(#evt == 1)
	assert(evt[1].type == "object-shown")
	assert(evt[1].object == obj)
	-- Unrealizing within the sphere.
	obj:set_visible(false)
	evt = update(vis)
	assert(#evt == 1)
	assert(evt[1].type == "object-hidden")
	assert(evt[1].object == obj)
	-- Multiple events.
	obj:set_visible(true)
	evt = update(vis)
	local obj1 = Object()
	obj1:set_id(2)
	obj1:set_position(Vector(100,200,300))
	obj1:set_visible(true)
	objects:add(obj1)
	obj:set_visible(false)
	evt = update(vis)
	assert(#evt == 2)
	if evt[2].type == "object-hidden" then
		assert(evt[1].type == "object-shown")
		assert(evt[1].object == obj1)
		assert(evt[2].type == "object-hidden")
		assert(evt[2].object == obj)
	else
		assert(evt[1].type == "object-hidden")
		assert(evt[1].object == obj)
		assert(evt[2].type == "object-shown")
		assert(evt[2].object == obj1)
	end
	-- Garbage collection.
	obj1:set_visible(false)
	evt = update(vis)
	obj = nil
	obj1 = nil
	collectgarbage()
	for k,v in pairs(vis.objects) do assert(false) end
end)
