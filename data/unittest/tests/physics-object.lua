Unittest:add(2, "system", "physics object", function()
	local Model = require("system/model")
	local Physics = require("system/physics")
	local PhysicsObject = require("system/physics-object")
	local Program = require("system/core")
	local Vector = require("system/math/vector")
	-- Creation and basic setters.
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
	-- Simulation.
	Physics:set_enable_simulation(true)
	Program:discard_events()
	local start = Program:get_time()
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
		curr = Program:get_time()
	end
	assert(o:get_position().x == 1000)
	assert(o:get_position().y < 1100)
	assert(o:get_position().z == 1200)
	Physics:set_enable_simulation(false)
	-- TODO: Collisions.
end)
