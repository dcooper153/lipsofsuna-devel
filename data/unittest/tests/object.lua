Unittest:add(1, "object", function()
	require "system/object"
	require "system/object-physics"
	Physics.enable_simulation = true
	-- Getters and setters.
	do
		local obj = Object{position = Vector(1,2,3), realized = true}
		assert(obj.position.class == Vector)
		assert(obj.position.x == 1 and obj.position.y == 2 and obj.position.z == 3)
		assert(obj.rotation.class == Quaternion)
		assert(obj.realized)
		assert(obj.sector == 0)
		obj.realized = false
		assert(obj.sector == nil)
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
	-- Keeping realized objects.
	for i = 1,100 do Object{model = Model(), position = Vector(10*i,50,50), realized = true} end
	collectgarbage()
	local num = 0
	for k,v in pairs(__objects_realized) do num = num + 1 end
	assert(num == 100)
	-- Physics simulation.
	for i = 1,1000 do
		local o = Object{model = Model(), position = Vector(50,50,50), realized = true}
		Program:update()
		o.realized = false
	end
	-- Unloading objects.
	Program:unload_world()
	for k,v in pairs(__objects_realized) do assert(false) end
	collectgarbage()
end)
