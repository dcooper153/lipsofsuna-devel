Selection = Class()
Selection.class_name = "Selection"
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

Selection.new = function(clss, data, face)
	local self = Class.new(clss)
	if data.class_name == "Vector" then
		local p = (data + Vector(0.5,0.5,0.5)) * Voxel.tile_size
		self.key = clss:get_tile_key(data, face)
		self.tile = data
		self.face = face
		self.visual = EditorObject{model = "select1", position = p, rotation = clss.face_rot[face], realized = true}
	else
		self.face = face
		self.key = data
		self.object = data
		self.visual = EditorObject{model = "select2", position = data.position, rotation = clss.face_rot[face], realized = true}
	end
	self.visual.selection = self
	return self
end

Selection.detach = function(self)
	self.visual.realized = false
end

Selection.refresh = function(self)
	if not self.object then return end
	self.visual.position = self.object.position
	self.visual.rotation = self.object.rotation
end

Selection.rotate = function(self, drot)
	if not self.object then return end
	self.object.rotation = drot * self.object.rotation
	self:refresh()
end

Selection.transform = function(self, center, dpos, drot)
	if not self.object then return end
	self.object.position = drot * (self.object.position + dpos - center) + center
	self:refresh()
end
