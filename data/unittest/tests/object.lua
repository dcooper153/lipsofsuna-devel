Unittest:add(1, "system", "object", function()
	local Object = require("system/object")
	local Quaternion = require("system/math/quaternion")
	local Vector = require("system/math/vector")
	-- Getters and setters.
	do
		local obj = Object()
		obj:set_position(Vector(1,2,3))
		obj:set_visible(true)
		assert(obj:get_position().class == Vector)
		assert(obj:get_position().x == 1 and obj:get_position().y == 2 and obj:get_position().z == 3)
		assert(obj:get_rotation().class == Quaternion)
		assert(obj:get_visible())
		obj:set_visible(false)
		assert(not obj:get_visible())
		collectgarbage()
	end
	-- Name field.
	do
		local obj = Object()
		assert(obj.name == nil)
		assert(obj.class_name == "Object")
		obj.name = "Yui"
		assert(obj.name == "Yui")
		obj.name = nil
		assert(obj.name == nil)
		collectgarbage()
	end
	-- Garbage collection.
	local objs = setmetatable({}, {__mode = "v"})
	for i = 1,100 do
		local o = Object()
		o:set_position(Vector(10*i,50,50))
		o:set_visible(true)
		objs[i] = o
	end
	collectgarbage()
	local num = 0
	for k,v in pairs(objs) do num = num + 1 end
	assert(num == 0)
end)
