import bpy, mathutils
from bpy_extras.object_utils import object_data_add

class LIModifier:

	def __init__(self, obj):
		# Store the original object.
		self.old_obj = obj
		self.old_mesh = obj.data
		# Store the original counts.
		self.old_num_verts = len(obj.data.vertices)
		self.old_num_edges = len(obj.data.edges)
		self.old_num_faces = len(obj.data.polygons)
		self.old_num_groups = len(obj.vertex_groups)
		self.old_num_shapes = len(obj.data.shape_keys.key_blocks)
		self.old_num_layers = 0
		if obj.data.uv_textures:
			self.old_num_layers = len(obj.data.uv_textures)
		# Initialize the created data.
		self.new_obj = None
		self.new_mesh = None
		self.faces = []
		self.face_newtoold = {}
		self.face_oldtonew = {}
		self.verts = []
		self.vert_newtoold = {}
		self.vert_oldtonew = {}

	def apply(self):
		# Prepare the geometry.
		self._add_vertices()
		self._add_faces()
		# Create the new object.
		self._create_mesh()
		self._create_materials()
		self._create_vertices()
		self._create_faces()
		self._create_uv_textures()
		self._create_uv_polygons()
		self._create_object()
		self._create_shape_keys()
		self._create_vertex_groups()
		self._create_vertex_group_weights()
		self._create_edges()
		self._finish()
		# Return the built object.
		return self.new_obj

	def _add_vertices(self):
		for i in range(0,self.old_num_verts):
			self.verts.append(self.old_mesh.vertices[i].co)
			self.vert_newtoold[i] = i
			self.vert_oldtonew[i] = i

	def _add_faces(self):
		for i in range(0,self.old_num_faces):
			face = self.old_mesh.polygons[i]
			if len(face.vertices) == 3:
				indices = [face.vertices[0], face.vertices[1], face.vertices[2]]
			else:
				indices = [face.vertices[0], face.vertices[1], face.vertices[2], face.vertices[3]]
			self.face_newtoold[i] = i
			self.face_oldtonew[i] = i
			self.faces.append(indices)

	def _create_mesh(self):
		self.new_mesh = bpy.data.meshes.new("LosTmp")
		self.new_mesh.from_pydata(self.verts, [], self.faces)
		self.new_mesh.update()

	def _create_edges(self):
		old_edge_dict = {}
		for old_edge in self.old_mesh.edges:
			old_edge_dict[old_edge.key] = old_edge
		for new_edge in self.new_mesh.edges:
			old_key = (self.vert_newtoold[new_edge.key[0]], self.vert_newtoold[new_edge.key[1]])
			if old_key[1] < old_key[0]:
				old_key = (old_key[1], old_key[0])
			old_edge = old_edge_dict.get(old_key)
			if old_edge:
				new_edge.bevel_weight = old_edge.bevel_weight
				new_edge.crease = old_edge.crease
				new_edge.hide = old_edge.hide
				new_edge.is_loose = old_edge.is_loose
				new_edge.select = old_edge.select
				new_edge.use_edge_sharp = old_edge.use_edge_sharp
				new_edge.use_seam = old_edge.use_seam

	def _create_materials(self):
		for mat in self.old_obj.data.materials:
			self.new_mesh.materials.append(mat)

	def _create_vertices(self):
		for i in range(0,len(self.verts)):
			old_vert = self.old_mesh.vertices[self.vert_newtoold[i]]
			new_vert = self.new_mesh.vertices[i]
			new_vert.normal = old_vert.normal.copy()

	def _create_faces(self):
		for i in range(0,len(self.faces)):
			old_face = self.old_mesh.polygons[self.face_newtoold[i]]
			new_face = self.new_mesh.polygons[i]
			new_face.hide = old_face.hide
			new_face.material_index = old_face.material_index
			new_face.select = old_face.select
			new_face.use_smooth = old_face.use_smooth

	def _create_uv_textures(self):
		for i in range(0,self.old_num_layers):
			old_texture = self.old_mesh.uv_textures[i]
			new_texture = self.new_mesh.uv_textures.new(old_texture.name)
			for i in range(0,len(self.faces)):
				old_uv_face = old_texture.data[self.face_newtoold[i]]
				new_uv_face = new_texture.data[i]
				new_uv_face.image = old_uv_face.image

	def _create_uv_polygons(self):
		for i in range(0,len(self.faces)):
			old_face = self.old_mesh.polygons[self.face_newtoold[i]]
			new_face = self.new_mesh.polygons[i]
			for j in range(0,self.old_num_layers):
				for k in range(0,old_face.loop_total):
					old_uv_face = self.old_mesh.uv_layers[j].data[old_face.loop_start + k]
					new_uv_face = self.new_mesh.uv_layers[j].data[new_face.loop_start + k]
					new_uv_face.pin_uv = old_uv_face.pin_uv
					new_uv_face.select = old_uv_face.select
					new_uv_face.select_edge = old_uv_face.select_edge
					new_uv_face.uv = (old_uv_face.uv[0], old_uv_face.uv[1])

	def _create_object(self):
		object_data_add(bpy.context, self.new_mesh, operator=None)
		self.new_obj = bpy.context.active_object
		def copyprop(prop):
			try:
				v = self.old_obj[prop]
				self.new_obj[prop] = v
			except:
				pass
		copyprop("export")
		copyprop("file")
		copyprop("render")
		copyprop("shape")

	def _create_shape_keys(self):
		for i in range(0,self.old_num_shapes):
			old_shape = self.old_mesh.shape_keys.key_blocks[i]
			self.new_obj.shape_key_add(name=old_shape.name, from_mix=False)
			new_shape = self.new_mesh.shape_keys.key_blocks[i]
			for j in range(0,len(self.verts)):
				old_vert = old_shape.data[self.vert_newtoold[j]]
				new_vert = new_shape.data[j]
				new_vert.co = old_vert.co.copy()
				if j != self.vert_newtoold[j]:
					new_vert.co.x *= -1.0

	def _create_vertex_groups(self):
		for i in range(0,self.old_num_groups):
			old_group = self.old_obj.vertex_groups[i]
			new_group = self.new_obj.vertex_groups.new(name=old_group.name)

	def _create_vertex_group_weights(self):
		for i in range(0,len(self.verts)):
			old_vert = self.old_mesh.vertices[self.vert_newtoold[i]]
			new_vert = self.new_mesh.vertices[i]
			for j in range(0,len(old_vert.groups)):
				old_elem = old_vert.groups[j]
				old_group = self.old_obj.vertex_groups[old_elem.group]
				new_group = self.new_obj.vertex_groups[old_elem.group]
				new_group.add([i], old_elem.weight, 'REPLACE')

	def _finish(self):
		pass
