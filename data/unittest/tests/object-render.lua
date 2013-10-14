Unittest:add(2, "system", "render object: model", function()
	local Aabb = require("system/math/aabb")
	local Model = require("system/model-editing")
	local RenderObject = require("system/render-object")
	local Vector = require("system/math/vector")
	-- Checks for valgrind.
	local create_cube_model = function(aabb)
		-- Calculate the vertices of the bounding box.
		local min = aabb.point
		local max = aabb.point + aabb.size
		local p = {
			{min.x,min.y,min.z}, {max.x,min.y,min.z}, {min.x,max.y,min.z}, {max.x,max.y,min.z},
			{min.x,min.y,max.z}, {max.x,min.y,max.z}, {min.x,max.y,max.z}, {max.x,max.y,max.z}}
		-- Face creation helpers.
		local i = 1
		local v = {}
		local addface = function(a,b,c,d,n)
			v[i] = {a[1],a[2],a[3],n[1],n[2],n[3]}
			v[i+1] = {b[1],b[2],b[3],n[1],n[2],n[3]}
			v[i+2] = {d[1],d[2],d[3],n[1],n[2],n[3]}
			v[i+3] = v[i]
			v[i+4] = v[i+2]
			v[i+5] = {c[1],c[2],c[3],n[1],n[2],n[3]}
			i = i + 6
		end
		-- Left and right.
		addface(p[1], p[3], p[5], p[7], {-1,0,0})
		addface(p[2], p[4], p[6], p[8], {1,0,0})
		-- Bottom and top.
		addface(p[1], p[2], p[5], p[6], {0,-1,0})
		addface(p[3], p[4], p[7], p[8], {0,1,0})
		-- Front and back.
		addface(p[1], p[2], p[3], p[4], {0,0,-1})
		addface(p[5], p[6], p[7], p[8], {0,0,1})
		-- Create the model.
		local model = Model()
		model:add_material{cull = false, shader = "default"}
		model:add_triangles{material = 1, vertices = v}
		model:changed()
		return model
	end
	for i=1,100 do
		local m1 = create_cube_model(Aabb{point = Vector(), size = Vector(3,3,3)})
		local m2 = create_cube_model(Aabb{point = Vector(), size = Vector(6,6,6)})
		local m = m1:copy()
		m:merge(m2)
		local o = RenderObject()
		o:add_model(m)
		o:set_visible(true)
	end
end)

Unittest:add(2, "system", "render object: custom params", function()
	local Model = require("system/model-editing")
	local RenderObject = require("system/render-object")
	local m = Model()
	local o = RenderObject()
	o:add_model(m)
	o:set_visible(true)
	o:set_custom_param(1, 0.5, 0.5, 0.5, 1)
	o:set_custom_param(5, 0.5, 0.5, 0.5, 1)
	o:set_custom_param(5, 0.5, 0.5, 0.5, 1)
	o:set_custom_param(100, 0.5, 0.5, 0.5, 1)
end)

Unittest:add(2, "system", "render object: render queue", function()
	local Model = require("system/model-editing")
	local RenderObject = require("system/render-object")
	local m = Model()
	local o = RenderObject()
	o:add_model(m)
	o:set_visible(true)
	o:set_render_queue("6")
end)
