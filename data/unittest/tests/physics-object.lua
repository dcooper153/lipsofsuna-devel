Unittest:add(1, "system", "physics object: creation", function()
	local Model = require("system/model")
	local Physics = require("system/physics")
	local PhysicsObject = require("system/physics-object")
	local Vector = require("system/math/vector")
	local o = PhysicsObject()
	assert(o:get_physics() == "static")
	o:set_id(1)
	o:set_mass(100)
	o:set_model(Model())
	o:set_physics("rigid")
	o:set_position(Vector(1000,1100,1200))
	o:set_visible(true)
	assert(o:get_id() == 1)
	assert(o:get_mass() == 100)
	assert(o:get_physics() == "rigid")
	assert(o:get_position().x == 1000)
	assert(o:get_position().y == 1100)
	assert(o:get_position().z == 1200)
	assert(o:get_visible() == true)
end)

Unittest:add(1, "system", "physics object: collision", function()
	local Model = require("system/model-editing")
	local Physics = require("system/physics")
	local PhysicsObject = require("system/physics-object")
	local Program = require("system/core")
	local Time = require("system/time")
	local Vector = require("system/math/vector")
	-- Create the collision shape.
	local model = Model()
	model:add_material{material = "diff1"}
	model:add_aabb(1, Vector(-1,-1,-1), Vector(1,1,1))
	model:changed()
	-- Create the first object.
	local o1 = PhysicsObject()
	o1:set_id(1)
	o1:set_mass(100)
	o1:set_model(model)
	o1:set_physics("rigid")
	o1:set_position(Vector(1000,1100,1200))
	o1:set_visible(true)
	local o2 = PhysicsObject()
	o2:set_id(1)
	o2:set_mass(100)
	o2:set_model(model)
	o2:set_physics("rigid")
	o2:set_position(Vector(1000,1100,1200))
	o2:set_visible(true)
	-- Simulation.
	Physics:set_enable_simulation(true)
	Program:discard_events()
	local start = Time:get_secs()
	local curr = start
	local prev = start - 0.01
	while curr < start + 0.2 do
		Physics:update(curr - prev)
		Program:update()
		local e = Program:pop_event()
		while e do
			e = Program:pop_event()
		end
		prev = curr
		curr = Time:get_secs()
	end
	Physics:set_enable_simulation(false)
end)

Unittest:add(1, "system", "physics object: motion", function()
	local Model = require("system/model")
	local Physics = require("system/physics")
	local PhysicsObject = require("system/physics-object")
	local Program = require("system/core")
	local Time = require("system/time")
	local Vector = require("system/math/vector")
	-- Create the object.
	local o = PhysicsObject()
	assert(o:get_physics() == "static")
	o:set_id(1)
	o:set_mass(100)
	o:set_model(Model())
	o:set_physics("rigid")
	o:set_position(Vector(1000,1100,1200))
	o:set_visible(true)
	-- Simulation.
	Physics:set_enable_simulation(true)
	Program:discard_events()
	local start = Time:get_secs()
	local curr = start
	local prev = start - 0.01
	while curr < start + 0.2 do
		Physics:update(curr - prev)
		Program:update()
		local e = Program:pop_event()
		while e do
			if e.type ~= "tick" then
				assert(e.type == "object-motion")
				assert(e.id == o:get_id())
				assert(o:get_position().x == 1000)
				assert(o:get_position().y < 1100)
				assert(o:get_position().z == 1200)
			end
			e = Program:pop_event()
		end
		prev = curr
		curr = Time:get_secs()
	end
	assert(o:get_position().x == 1000)
	assert(o:get_position().y < 1100)
	assert(o:get_position().z == 1200)
	Physics:set_enable_simulation(false)
end)
