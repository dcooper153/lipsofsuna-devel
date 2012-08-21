local Class = require("system/class")
local Model = require("system/model")
local RenderObject = require("system/object-render")

local Selection = Class("Selection")
Selection.face_dir = {
	Vector(-1,0,0), Vector(1,0,0),
	Vector(0,-1,0), Vector(0,1,0),
	Vector(0,0,-1), Vector(0,0,1)}
Selection.face_rot = {
	Quaternion{axis = Vector(0,0,1), angle = math.pi/2},
	Quaternion{axis = Vector(0,0,1), angle = -math.pi/2},
	Quaternion{axis = Vector(1,0,0), angle = math.pi},
	Quaternion{axis = Vector(1,0,0), angle = 0},
	Quaternion{axis = Vector(1,0,0), angle = -math.pi/2},
	Quaternion{axis = Vector(1,0,0), angle = math.pi/2}}

Selection.get_tile_key = function(clss, tile, face)
	return math.floor(face + 10 * tile.x + 10000 * tile.y + 10000000 * tile.z + 0.5)
end

Selection.new = function(clss, data, face, hide)
	local self = Class.new(clss)
	if data and face then
		if data.class_name == "Vector" then
			self:set_tile(data, face)
		else
			self:set_object(data, face)
		end
	end
	if not hide and self.visual then
		self.visual:set_visible(true)
	end
	return self
end

Selection.create_face_model = function(self, face, rotation, aabb)
	-- Calculate the vertices of the bounding box.
	local pad = 0.2
	local min = rotation * (aabb.point - Vector(pad,pad,pad))
	local max = rotation * (aabb.point + aabb.size + Vector(pad,pad,pad))
	min.x,max.x = math.min(min.x,max.x),math.max(min.x,max.x)
	min.y,max.y = math.min(min.y,max.y),math.max(min.y,max.y)
	min.z,max.z = math.min(min.z,max.z),math.max(min.z,max.z)
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
	local addbox = function(a,b,c,d,n)
	addface(a,{a[1]-0.1,a[2]-0.1,a[3]-0.1},c,{c[1]-0.1,c[2]-0.1,c[3]-0.1},n)
	addface(b,{b[1]-0.1,b[2]-0.1,b[3]-0.1},d,{d[1]-0.1,d[2]-0.1,d[3]-0.1},n)
	addface(a,{a[1]-0.1,a[2]-0.1,a[3]-0.1},b,{b[1]-0.1,b[2]-0.1,b[3]-0.1},n)
	addface(c,{c[1]-0.1,c[2]-0.1,c[3]-0.1},d,{d[1]-0.1,d[2]-0.1,d[3]-0.1},n)
	end
	-- Left and right.
	if face == 1 then addbox(p[1],p[3],p[5],p[7],{-1,0,0})end
	if face == 2 then addbox(p[2],p[4],p[6],p[8],{-1,0,0}) end
	-- Bottom and top.
	if face == 3 then addbox(p[1], p[2], p[5], p[6], {0,-1,0}) end
	if face == 4 then addbox(p[3], p[4], p[7], p[8], {0,1,0}) end
	-- Front and back.
	if face == 5 then addbox(p[1], p[2], p[3], p[4], {0,0,-1}) end
	if face == 6 then addbox(p[5], p[6], p[7], p[8], {0,0,1}) end
	-- Create the model.
	local model = Model()
	model:add_material{material = "bounds1"}
	--model:remove_vertices()
	model:add_triangles{material = 1, vertices = v}
	model:changed()
	return model
end

Selection.detach = function(self)
	if not self.visual then return end
	self.visual:set_visible(false)
	if self.visual_prev then
		self.visual_prev:set_visible(false)
		self.visual_prev = nil
	end
end

Selection.get_loaded = function(self)
	if not self.visual then return true end
	return self.visual:get_loaded()
end

Selection.refresh = function(self)
	if not self.visual then return end
	if not self.object then return end
	self.visual:set_position(self.object:get_position())
	self.visual:set_rotation(self.object:get_rotation())
end

Selection.rotate = function(self, drot)
	if not self.visual then return end
	if not self.object then return end
	self.object:set_rotation(drot * self.object:get_rotation())
	self:refresh()
end

Selection.transform = function(self, center, dpos, drot)
	if not self.object then return end
	local p = self.object:get_position():copy():add(dpos):subtract(center)
	self.object:set_position(drot * p + center)
	self:refresh()
end

Selection.update = function(self, secs)
	if not self.visual then return end
	if self.visual:get_visible() then return end
	if not self.visual:get_loaded() then return end
	self.visual:set_visible(true)
	if self.visual_prev then
		self.visual_prev:set_visible(false)
		self.visual_prev = nil
	end
end

Selection.set_empty = function(self)
	self:detach()
	self.face = nil
	self.key = nil
	self.object = nil
	self.tile = nil
	self.visual = nil
end

Selection.set_object = function(self, data, face)
	local model = self:create_face_model(face, data:get_rotation(), data:get_bounding_box_physics())
	self.face = face
	self.key = data
	self.object = data
	self.tile = nil
	self.visual_prev = self.visual
	self.visual = RenderObject()
	self.visual.model = model:get_render()
	self.visual:add_model(self.visual.model)
	self.visual:set_position(data:get_position())
end

Selection.set_tile = function(self, data, face)
	local p = (data + Vector(0.5,0.5,0.5)) * Voxel.tile_size
	self.face = face
	self.key = self:get_tile_key(data, face)
	self.object = nil
	self.tile = data
	self.visual_prev = self.visual
	self.visual = RenderObject()
	self.visual.model = Main.models:find_by_name("select1"):get_render()
	self.visual:add_model(self.visual.model)
	self.visual:set_position(p)
	self.visual:set_rotation(self.face_rot[face])
end

return Selection
