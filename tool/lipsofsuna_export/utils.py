import re
import bpy, mathutils
from bpy_extras.object_utils import object_data_add

class LIUtils:

	@classmethod
	def get_files_for_material(cls, material):
		def getprop(mat, prop):
			try:
				return mat[prop]
			except:
				return ""
		files = getprop(material, 'file').split(',')
		if getprop(material, 'lod') == 'true':
			return [f + 'l.lmdl' for f in files if len(f)]
		else:
			return [f + '.lmdl' for f in files if len(f)]

	@classmethod
	def get_files_for_object(cls, object):
		def getprop(obj, prop):
			try:
				return obj[prop]
			except:
				return ""
		# No files for explicitly disabled objects.
		if getprop(object, 'export') == 'false':
			return []
		# Don't export links or proxies.
		try:
			if object.proxy or object.library:
				return []
			if object.data.library:
				return []
		except:
			pass
		# Otherwise get the list from the file property.
		files = getprop(object, 'file').split(',')
		if getprop(object, 'lod') == 'true':
			return [f + 'l.lmdl' for f in files if len(f)]
		else:
			return [f + '.lmdl' for f in files if len(f)]

	@classmethod
	def object_check_export(cls, object, file, type='NODE'):
		def getprop(object, prop):
			try:
				return object[prop]
			except:
				return ""
		# Don't export links or proxies.
		try:
			if object.proxy or object.library:
				return False
			if object.data.library:
				return False
		except:
			pass
		# Make sure that the filename matches.
		if file not in cls.get_files_for_object(object):
			return False
		# Make sure that the type is a requested one.
		if type == 'NODE':
			return True
		elif type == 'PARTICLE':
			if object.type != 'MESH':
				return False
			if getprop(object, 'shape') != "":
				return False
			return True
		elif type == 'RENDER':
			if object.type != 'MESH':
				return False
			if getprop(object, 'shape') != "" or getprop(object, 'render') == "false":
				return False
			return True
		elif type == 'SHAPE':
			if object.type != 'MESH':
				return False
			if getprop(object, 'shape') == "":
				return False
			return True
		else:
			return False

	# \brief Mirrors the object and its shape keys.
	# \param obj Object to mirror.
	# \param mod Mirror modifier.
	@classmethod
	def apply_shape_key_mirror(cls, obj, mod):
		mirror_g = mod.use_mirror_vertex_groups
		mirror_u = mod.use_mirror_u
		mirror_v = mod.use_mirror_v
		mirror_x = mod.use_x
		faces = []
		face_newtoold = {}
		face_oldtonew = {}
		verts = []
		vert_merged = {}
		vert_newtoold = {}
		vert_oldtonew = {}
		# Store original counts.
		num_verts = len(obj.data.vertices)
		num_edges = len(obj.data.edges)
		num_faces = len(obj.data.polygons)
		num_groups = len(obj.vertex_groups)
		num_shapes = len(obj.data.shape_keys.key_blocks)
		num_layers = 0
		if obj.data.uv_textures:
			num_layers = len(obj.data.uv_textures)
		# Add original vertices.
		for i in range(0,num_verts):
			verts.append(obj.data.vertices[i].co)
			vert_newtoold[i] = i
			vert_oldtonew[i] = i
		# Add mirrored vertices.
		for i in range(0,num_verts):
			vert = verts[i]
			if vert.x > -0.001 and vert.x < 0.001:
				vert_newtoold[i] = i
				vert_oldtonew[i] = i
				vert_merged[i] = True
			else:
				l = len(verts)
				vert_newtoold[l] = i
				vert_oldtonew[i] = l
				verts.append(mathutils.Vector((-vert.x, vert.y, vert.z)))
		# Add original faces.
		for i in range(0,num_faces):
			face = obj.data.polygons[i]
			if len(face.vertices) == 3:
				indices = [face.vertices[0], face.vertices[1], face.vertices[2]]
			else:
				indices = [face.vertices[0], face.vertices[1], face.vertices[2], face.vertices[3]]
			face_newtoold[i] = i
			face_oldtonew[i] = i
			faces.append(indices)
		# Add mirrored faces.
		for i in range(0,num_faces):
			face = faces[i]
			if len(face) == 3:
				indices = [face[2], face[1], face[0]]
			else:
				indices = [face[3], face[2], face[1], face[0]]
			for j in range(0,len(face)):
				indices[j] = vert_oldtonew[indices[j]]
			l = len(faces)
			face_newtoold[l] = i
			face_oldtonew[i] = l
			faces.append(indices)
		# Create the mesh.
		mesh = bpy.data.meshes.new("LosTmp")
		mesh.from_pydata(verts, [], faces)
		mesh.update()
		# Create materials.
		for mat in obj.data.materials:
			mesh.materials.append(mat)
		# Create vertices.
		for i in range(0,len(verts)):
			old_vert = obj.data.vertices[vert_newtoold[i]]
			new_vert = mesh.vertices[i]
			if i in vert_merged:
				n = old_vert.normal.copy()
				n.x = 0
				new_vert.normal = n.normalized()
			else:
				new_vert.normal = old_vert.normal.copy()
		# Create faces.
		for i in range(0,len(faces)):
			old_face = obj.data.polygons[face_newtoold[i]]
			new_face = mesh.polygons[i]
			new_face.hide = old_face.hide
			new_face.material_index = old_face.material_index
			new_face.select = old_face.select
			new_face.use_smooth = old_face.use_smooth
		# Create UV textures.
		for i in range(0,num_layers):
			old_texture = obj.data.uv_textures[i]
			new_texture = mesh.uv_textures.new(old_texture.name)
			for i in range(0,len(faces)):
				old_uv_face = old_texture.data[face_newtoold[i]]
				new_uv_face = new_texture.data[i]
				new_uv_face.image = old_uv_face.image
		# Create UV layers.
		for i in range(0,len(faces)):
			old_face = obj.data.polygons[face_newtoold[i]]
			new_face = mesh.polygons[i]
			for j in range(0,num_layers):
				for k in range(0,old_face.loop_total):
					old_uv_face = obj.data.uv_layers[j].data[old_face.loop_start + k]
					new_uv_face = mesh.uv_layers[j].data[new_face.loop_start + k]
					new_uv_face.pin_uv = old_uv_face.pin_uv
					new_uv_face.select = old_uv_face.select
					new_uv_face.select_edge = old_uv_face.select_edge
					if not mirror_u or face_newtoold[i] == i:
						new_uv_face.uv = (old_uv_face.uv[0], old_uv_face.uv[1])
					else:
						new_uv_face.uv = (1.0-old_uv_face.uv[0], old_uv_face.uv[1])
		# Create the object.
		object_data_add(bpy.context, mesh, operator=None)
		new_obj = bpy.context.scene.objects.active
		# Create shape keys.
		for i in range(0,num_shapes):
			old_shape = obj.data.shape_keys.key_blocks[i]
			new_obj.shape_key_add(name=old_shape.name, from_mix=False)
			new_shape = new_obj.data.shape_keys.key_blocks[i]
			for j in range(0,len(verts)):
				old_vert = old_shape.data[vert_newtoold[j]]
				new_vert = new_shape.data[j]
				new_vert.co = old_vert.co.copy()
				if j != vert_newtoold[j]:
					new_vert.co.x *= -1.0
		# Create vertex groups.
		for i in range(0,num_groups):
			old_group = obj.vertex_groups[i]
			new_group = new_obj.vertex_groups.new(name=old_group.name)
		for i in range(0,len(verts)):
			old_vert = obj.data.vertices[vert_newtoold[i]]
			new_vert = mesh.vertices[i]
			for j in range(0,len(old_vert.groups)):
				old_elem = old_vert.groups[j]
				old_group = obj.vertex_groups[old_elem.group]
				if mirror_g and i != vert_newtoold[i]:
					new_group_name = re.sub(r'\.L$', '.R', old_group.name)
					if new_group_name == old_group.name:
						new_group_name = re.sub(r'\.R$', '.L', old_group.name)
					if new_group_name in new_obj.vertex_groups:
						new_group = new_obj.vertex_groups[new_group_name]
					else:
						new_group = new_obj.vertex_groups[old_group.name]
				else:
					new_group = new_obj.vertex_groups[old_elem.group]
				new_group.add([i], old_elem.weight, 'REPLACE')
		# Copy custom properties.
		def copyprop(prop):
			try:
				v = obj[prop]
				new_obj[prop] = v
			except:
				pass
		copyprop("export")
		copyprop("file")
		copyprop("render")
		copyprop("shape")
