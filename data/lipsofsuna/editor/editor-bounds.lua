local Class = require("system/class")
local Model = require("system/model")
local RenderModel = require("system/render-model")
local RenderObject = require("system/object-render")

local EditorBounds = Class("EditorBounds")

EditorBounds.new = function(clss)
	local self = Class.new(clss)
	self.render = RenderObject()
	self.model = Model()
	self.model:add_material{material = "bounds1"}
	return self
end

EditorBounds.detach = function(self)
	self.render:set_visible(false)
end

EditorBounds.rebuild = function(self, size)
	local i = 1
	local v = {}
	local face = function(a,b,c,d,n)
		v[i] = {a[1],a[2],a[3],n[1],n[2],n[3]}
		v[i+1] = {b[1],b[2],b[3],n[1],n[2],n[3]}
		v[i+2] = {d[1],d[2],d[3],n[1],n[2],n[3]}
		v[i+3] = v[i]
		v[i+4] = v[i+2]
		v[i+5] = {c[1],c[2],c[3],n[1],n[2],n[3]}
		i = i + 6
	end
	local o = -0.2
	local s = size * Voxel.tile_size - Vector(o,o,o)*2
	local p = {
		{o,o,o,1}, {s.x,o,o,1}, {o,s.y,o,1}, {s.x,s.y,o,1},
		{o,o,s.z,1}, {s.x,o,s.z,1}, {o,s.y,s.z,1}, {s.x,s.y,s.z,1}}
	-- Front and back.
	face(p[1], p[2], p[3], p[4], {0,0,-1})
	face(p[5], p[6], p[7], p[8], {0,0,1})
	-- Bottom and top.
	face(p[1], p[2], p[5], p[6], {0,-1,0})
	face(p[3], p[4], p[7], p[8], {0,1,0})
	-- Left and right.
	face(p[1], p[3], p[5], p[7], {-1,0,0})
	face(p[2], p[4], p[6], p[8], {1,0,0})
	-- Create the cube.
	self.model:remove_vertices()
	self.model:add_triangles{material = 1, vertices = v}
	self.model:changed()
	-- Replace the old model.
	local render_model = RenderModel(self.model)
	if self.render_model then
		self.render:replace_model(self.render_model, render_model)
	else
		self.render:add_model(render_model)
	end
	self.render_model = render_model
	self.render:set_visible(true)
end

EditorBounds.set_position = function(self, v)
	self.render:set_position(v)
end

return EditorBounds
