from .material import *
from .shapekey import *
from .utils import *
from .vertex import *

class LIMesh:

	def __init__(self, obj, file):
		mesh = obj.data
		self.file = file
		# Initialize storage.
		self.matdict = {}
		self.matlist = []
		self.shapekeydict = {}
		self.shapekeylist = []
		self.vertdict = {} # bl_vertex -> los_index
		self.vertlist = [] # los_index -> los_vertex
		self.vertmapping = [] # los_index -> bl_index
		self.weightgroupdict = {}
		self.weightgrouplist = []
		# Emit faces.
		self.emit_faces(obj, mesh)
		# Emit shape keys.
		if obj.data.shape_keys:
			for bl_key in obj.data.shape_keys.key_blocks:
				key = LIShapeKey(self, obj, bl_key)
				if not key.empty:
					self.shapekeydict[bl_key.name] = key
					self.shapekeylist.append(key)

	def emit_face(self, obj, mesh, face):
		# Vertices.
		verts = [mesh.vertices[x] for x in face.vertices]
		indices = [x for x in face.vertices]
		# Material attributes.
		# Materials may have a file property that limits exporting of the face
		# to a certain file. We need to check that the file is correct and skip
		# the face if it isn't.
		idx = face.material_index
		bmat = None
		if idx < len(obj.material_slots):
			bmat = obj.material_slots[idx].material
			files = LIUtils.get_files_for_material(bmat)
			if len(files) and self.file not in files:
				return
		# Texture attributes.
		bimg = None
		uvtexture = mesh.uv_textures.active
		uvlayer = mesh.uv_layers.active
		if uvtexture and uvlayer:
			bimg = uvtexture.data[face.index].image
			uvs = [uvlayer.data[face.loop_start + i].uv for i in range(0, len(verts))]
		else:
			uvs = ((0, 0), (0, 0), (0, 0), (0, 0))
		# Emit material.
		key = (bmat and idx or -1, bimg and bimg.name or '')
		if key not in self.matdict:
			mat = LIMaterial(len(self.matlist), bmat, bimg)
			self.matdict[key] = mat
			self.matlist.append(mat)
		else:
			mat = self.matdict[key]
		# Emit triangles.
		for i in range(0, len(verts)):
			# Vertex attributes.
			bvert = verts[i]
			no = face.use_smooth and bvert.normal or face.normal
			uv = uvs[i]
			# Emit vertex.
			key = (indices[i], no.x, no.y, no.z, uv[0], uv[1])
			if key not in self.vertdict:
				# Add weights.
				weights = []
				for weight in bvert.groups:
					group = obj.vertex_groups[weight.group]
					if group.name not in self.weightgroupdict:
						grpidx = len(self.weightgrouplist)
						self.weightgroupdict[group.name] = grpidx
						self.weightgrouplist.append(group.name)
					else:
						grpidx = self.weightgroupdict[group.name]
					weights.append((grpidx, weight.weight))
				# Add vertex.
				vert = LIVertex(len(self.vertlist), bvert.co, no, uv, weights)
				self.vertmapping.append(indices[i])
				self.vertdict[key] = len(self.vertlist)
				self.vertlist.append(vert)
			else:
				index = self.vertdict[key]
				vert = self.vertlist[index]
			# Emit index.
			mat.indices.append(vert.index)

	def emit_faces(self, obj, mesh):
		for face in mesh.polygons:
			self.emit_face(obj, mesh, face)

	def write_bounds(self, writer):
		if not len(self.vertlist):
			return False
		min = self.vertlist[0].co.copy()
		max = self.vertlist[0].co.copy()
		for vert in self.vertlist:
			if min.x > vert.co.x: min.x = vert.co.x
			if min.y > vert.co.y: min.y = vert.co.y
			if min.z > vert.co.z: min.z = vert.co.z
			if max.x < vert.co.x: max.x = vert.co.x
			if max.y < vert.co.y: max.y = vert.co.y
			if max.z < vert.co.z: max.z = vert.co.z
		writer.write_float(min.x)
		writer.write_float(min.y)
		writer.write_float(min.z)
		writer.write_float(max.x)
		writer.write_float(max.y)
		writer.write_float(max.z)
		writer.write_marker()
		return True

	def write_groups(self, writer):
		if not len(self.matlist):
			return False
		writer.write_int(len(self.matlist))
		writer.write_marker()
		index = 0
		for mat in self.matlist:
			writer.write_int(index)
			mat.write_indices(writer)
			index += 1
		return True

	def write_materials(self, writer):
		if not len(self.matlist):
			return False
		writer.write_int(len(self.matlist))
		writer.write_marker()
		for mat in self.matlist:
			mat.write_info(writer)
		return True

	def write_shape_keys(self, writer):
		if not len(self.shapekeylist):
			return False
		writer.write_int(len(self.shapekeylist))
		writer.write_marker()
		for key in self.shapekeylist:
			key.write(writer)
		return True

	def write_vertices(self, writer):
		if not len(self.vertlist):
			return False
		writer.write_int(len(self.vertlist))
		writer.write_marker()
		for vert in self.vertlist:
			vert.write(writer)
		return True

	def write_weights(self, writer):
		if not len(self.weightgrouplist):
			return False
		writer.write_int(len(self.weightgrouplist))
		for group in self.weightgrouplist:
			writer.write_string(group)
			writer.write_string(group)
		writer.write_marker()
		writer.write_int(len(self.vertlist))
		writer.write_marker()
		for vertex in self.vertlist:
			vertex.write_weights(writer)
		return True
