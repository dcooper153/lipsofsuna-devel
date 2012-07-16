Unittest:add(1, "vision", function()
	require "system/vision"
	local evt
	local update = function(vis)
		local evt = vis:update()
		assert(#evt == 0)
		Eventhandler:update()
		return vis:update()
	end
	-- Initialization.
	Eventhandler:update()
	local vis = Vision{enabled = true, position = Vector(100,200,300), radius = 2, threshold = 1}
	assert(vis.radius == 2)
	assert(vis.threshold == 1)
	-- Creating unrealized objects.
	local obj = Object{position = Vector(100,200,300)}
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
	obj.position = obj.position + Vector(1.8)
	evt = update(vis)
	assert(#evt == 0)
	-- Moving within the keep threshold.
	obj.position = obj.position + Vector(1)
	evt = update(vis)
	assert(#evt == 0)
	-- Leaving the sphere.
	obj.position = obj.position + Vector(1.2)
	evt = update(vis)
	assert(#evt == 1)
	assert(evt[1].type == "object-hidden")
	assert(evt[1].object == obj)
	assert(not vis.objects[obj])
	-- Moving outside the sphere.
	obj.position = obj.position - Vector(1.5)
	evt = update(vis)
	assert(#evt == 0)
	-- Moving into the sphere.
	obj.position = obj.position - Vector(1)
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
	local obj1 = Object{position = Vector(100,200,300), realized = true}
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
