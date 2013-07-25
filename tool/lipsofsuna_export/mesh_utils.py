import bpy, mathutils
from bpy_extras.object_utils import object_data_add

class LIMeshUtils:

	@classmethod
	def create_mesh(self, verts, edges, faces):
		"""Creates a new mesh from vertices, edges and faces.
		
		Parameters:
			verts: List of vertex coordinates.
			edges: List of vertex indices of edges.
			edges: List of vertex indices of faces.
		
		Returns:
			Mesh.
		"""

		# Create the mesh from the generated data.
		new_mesh = bpy.data.meshes.new("LosTmp")
		new_mesh.from_pydata(verts, edges, faces)

		# Recalculate normals.
		new_mesh.update()

		# Fix the edge pointers of the faces.
		# Blender will crash if this is not done!
		new_mesh.validate()

		return new_mesh

	@classmethod
	def copy_mesh(self, mesh):
		"""Creates a deep copy of the mesh.
		
		Parameters:
			mesh: Blender mesh.
		
		Returns:
			Blender mesh.
		"""

		# Generate the primitive lists.
		verts = [v.co.copy() for v in mesh.vertices]
		edges = [[v for v in e.vertices] for e in mesh.edges]
		faces = [[v for v in f.vertices] for f in mesh.polygons]

		# Create the mesh.
		new_mesh = bpy.data.meshes.new("LosTmp")
		new_mesh.from_pydata(verts, edges, faces)

		# Recalculate normals.
		new_mesh.update()

		# Fix the edge pointers of the faces.
		# Blender will crash if this is not done!
		new_mesh.validate()

		# Copy the materials.
		for mat in mesh.materials:
			new_mesh.materials.append(mat)

		# Copy the vertex attributes.
		for i,new_vert in enumerate(new_mesh.vertices):
			old_vert = mesh.vertices[i]
			new_vert.normal = old_vert.normal.copy()

		# Copy the edge attributes.
		for i,new_edge in enumerate(new_mesh.edges):
			old_edge = mesh.edges[i]
			new_edge.bevel_weight = old_edge.bevel_weight
			new_edge.crease = old_edge.crease
			new_edge.hide = old_edge.hide
			new_edge.is_loose = old_edge.is_loose
			new_edge.select = old_edge.select
			new_edge.use_edge_sharp = old_edge.use_edge_sharp
			new_edge.use_seam = old_edge.use_seam

		# Copy the face attributes.
		for i,new_face in enumerate(new_mesh.polygons):
			old_face = mesh.polygons[i]
			new_face.hide = old_face.hide
			new_face.loop_start = old_face.loop_start
			new_face.loop_total = old_face.loop_total
			new_face.material_index = old_face.material_index
			new_face.select = old_face.select
			new_face.use_smooth = old_face.use_smooth

		# Copy the UV textures.
		if mesh.uv_textures:
			for i,old_texture in enumerate(mesh.uv_textures):
				new_texture = new_mesh.uv_textures.new(old_texture.name)
				for j in range(len(new_mesh.polygons)):
					old_uv_face = old_texture.data[j]
					new_uv_face = new_texture.data[j]
					new_uv_face.image = old_uv_face.image

		# Copy the UV polygons.
		if mesh.uv_textures:
			for i,new_face in enumerate(new_mesh.polygons):
				old_face = mesh.polygons[i]
				for j in range(len(mesh.uv_textures)):
					for k in range(old_face.loop_total):
						old_uv_face = mesh.uv_layers[j].data[old_face.loop_start + k]
						new_uv_face = new_mesh.uv_layers[j].data[new_face.loop_start + k]
						new_uv_face.pin_uv = old_uv_face.pin_uv
						new_uv_face.select = old_uv_face.select
						new_uv_face.select_edge = old_uv_face.select_edge
						new_uv_face.uv = (old_uv_face.uv[0], old_uv_face.uv[1])

		return new_mesh

	@classmethod
	def copy_object(self, obj):
		"""Creates a deep copy of the object.
		
		Parameters:
			obj: Blender object.
		
		Returns:
			Blender object.
		"""

		# Copy the mesh.
		mesh = obj.data
		new_mesh = self.copy_mesh(mesh)

		# Create the new object.
		object_data_add(bpy.context, new_mesh, operator=None)
		new_obj = bpy.context.active_object

		# Copy the transformation.
		#new_obj.location = obj.location
		#new_obj.rotation_quaternion = obj.rotation_quaternion
		#new_obj.scale = obj.scale
		new_obj.matrix_world = obj.matrix_world

		# Copy the properties.
		def copyprop(prop):
			try:
				v = obj[prop]
				new_obj[prop] = v
			except:
				pass
		copyprop("export")
		#copyprop("file")
		copyprop("render")
		copyprop("shape")

		# Copy the vertex groups.
		for i,old_group in enumerate(obj.vertex_groups):
			new_group = new_obj.vertex_groups.new(name=old_group.name)

		# Copy the vertex weights.
		for i,old_vert in enumerate(mesh.vertices):
			new_vert = new_mesh.vertices[i]
			for j,old_elem in enumerate(old_vert.groups):
				if old_elem.group >= len(obj.vertex_groups):
					print("WARNING: Vertex group %d/%d out of bounds for vertex %d" % (old_elem.group, len(obj.vertex_groups), i))
				else:
					old_group = obj.vertex_groups[old_elem.group]
					new_group = new_obj.vertex_groups[old_elem.group]
					new_group.add([i], old_elem.weight, 'REPLACE')

		return new_obj

	@classmethod
	def copy_object_with_shape_keys(self, obj):
		"""Creates a deep copy of the object and its shape keys.
		
		Parameters:
			obj: Blender object.
		
		Returns:
			Blender object.
		"""

		# Copy the object.
		mesh = obj.data
		new_obj = self.copy_object(obj)
		new_mesh = new_obj.data

		# Copy the shape keys.
		for i,old_shape in enumerate(mesh.shape_keys.key_blocks):
			new_obj.shape_key_add(name=old_shape.name, from_mix=False)
			new_shape = new_mesh.shape_keys.key_blocks[i]
			for j in range(len(mesh.vertices)):
				old_vert = old_shape.data[j]
				new_vert = new_shape.data[j]
				new_vert.co = old_vert.co.copy()

		return new_obj

	@classmethod
	def apply_modifiers(self, obj, mods):
		"""Applies modifiers to the object.
		
		Parameters:
			obj: Blender object.
			mods: List of blender modifier.
		"""

		for mod in mods:
			if mod.type == 'EDGE_SPLIT':
				self.apply_modifier_edge_split(obj, mod)
			elif mod.type == 'MIRROR':
				self.apply_modifier_mirror(obj, mod)

	@classmethod
	def apply_modifier_edge_split(self, obj, mod):
		"""Applies a mirror modifier to the object.
		
		Parameters:
			obj: Blender object.
			mod: Blender modifier.
		"""

		# Activate the object.
		bpy.context.scene.objects.active = obj
		assert(bpy.context.active_object == obj)

		# Create the modifier.
		bpy.ops.object.modifier_add(type='EDGE_SPLIT')
		assert(len(obj.modifiers) == 1)
		new_mod = obj.modifiers[0]
		new_mod.split_angle = mod.split_angle
		new_mod.use_edge_angle = mod.use_edge_angle
		new_mod.use_edge_sharp = mod.use_edge_sharp

		# Apply the modifier.
		old_mesh = obj.data
		obj.data = obj.to_mesh(bpy.context.scene, True, 'PREVIEW')
		bpy.data.meshes.remove(old_mesh)
		obj.modifiers.remove(new_mod)

	@classmethod
	def apply_modifier_mirror(self, obj, mod):
		"""Applies a mirror modifier to the object.
		
		Parameters:
			obj: Blender object.
			mod: Blender modifier.
		"""

		# Activate the object.
		bpy.context.scene.objects.active = obj
		assert(bpy.context.active_object == obj)

		# Create the modifier.
		bpy.ops.object.modifier_add(type='MIRROR')
		assert(len(obj.modifiers) == 1)
		new_mod = obj.modifiers[0]
		new_mod.merge_threshold = mod.merge_threshold
		new_mod.mirror_object = mod.mirror_object
		new_mod.use_clip = mod.use_clip
		new_mod.use_mirror_merge = mod.use_mirror_merge
		new_mod.use_mirror_u = mod.use_mirror_u
		new_mod.use_mirror_v = mod.use_mirror_v
		new_mod.use_mirror_vertex_groups = mod.use_mirror_vertex_groups
		new_mod.use_x = mod.use_x
		new_mod.use_y = mod.use_y
		new_mod.use_z = mod.use_z

		# Apply the modifier.
		old_mesh = obj.data
		obj.data = obj.to_mesh(bpy.context.scene, True, 'PREVIEW')
		bpy.data.meshes.remove(old_mesh)
		obj.modifiers.remove(new_mod)

	@classmethod
	def apply_shape_key(self, obj, index):
		"""Applies the given shape key and removes others.
		
		Parameters:
			obj: Blender object.
			index: Shape key index.
		"""

		# Activate the object.
		bpy.context.scene.objects.active = obj
		assert(bpy.context.active_object == obj)

		# Remove other shape keys.
		keys = obj.data.shape_keys
		assert(len(keys.key_blocks) >= 1)
		for i in range(len(keys.key_blocks),-1,-1):
			if i != index:
				obj.active_shape_key_index = i
				bpy.ops.object.shape_key_remove()

		# Remove the applied shape key last.
		assert(len(keys.key_blocks) == 1)
		obj.active_shape_key_index = 0
		bpy.ops.object.shape_key_remove()
