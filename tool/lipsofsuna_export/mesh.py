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
		self.partitions = {}
		# Calculate the bounding box.
		# This may be needed by generated texture coordinates.
		if len(mesh.vertices):
			p0 = mathutils.Vector(mesh.vertices[0].co)
			p1 = mathutils.Vector(mesh.vertices[1].co)
		else:
			p0 = mathutils.Vector((-1, -1, -1))
			p1 = mathutils.Vector((1, 1, 1))
		for vert in mesh.vertices:
			v = vert.co
			if p0.x > v[0]:
				p0.x = v[0]
			if p0.y > v[1]:
				p0.y = v[1]
			if p0.z > v[2]:
				p0.z = v[2]
			if p1.x < v[0]:
				p1.x = v[0]
			if p1.y < v[1]:
				p1.y = v[1]
			if p1.z < v[2]:
				p1.z = v[2]
		self.box_center = (p1 + p0) * 0.5
		self.box_size = p1 - p0
		# Emit faces.
		self.emit_faces(obj, mesh)
		# Emit shape keys.
		if obj.data.shape_keys:
			for bl_key in obj.data.shape_keys.key_blocks:
				key = LIShapeKey(self, obj, bl_key)
				if not key.empty:
					self.shapekeydict[bl_key.name] = key
					self.shapekeylist.append(key)

	def generate_texcoord_uv(self, face, verts, layer):
		return [layer.data[face.loop_start + i].uv for i in range(0, len(verts))]

	def generate_texcoord_orco_sphere(self, face, verts):
		# Helper function taken from Blender's source code.
		# FIXME: This doesn't work correctly.
		def map_to_sphere(orco):
			l = orco.length
			if l > 0.0:
				if orco.x == 0.0 and orco.y == 0.0:
					u = 0.0
				else:
					u = (1.0 - math.atan2(orco.x,orco.y) / math.pi) / 2.0
				v = 1.0 - math.acos(orco.z / l) / math.pi
				return (u,v)
			else:
				return (0,0)
		# Generate.
		def calc(v):
			orco = 2.0 * (v - self.box_center)
			orco.x /= self.box_size.x
			orco.y /= self.box_size.y
			orco.z /= self.box_size.z
			return map_to_sphere(orco)
		return [calc(mathutils.Vector(v.co)) for v in verts]

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
		# Get the image of the face.
		uvtexture = mesh.uv_textures.active
		uvlayer = mesh.uv_layers.active
		bimg = None
		if uvtexture and uvlayer:
			bimg = uvtexture.data[face.index].image
		# Select the texture coordinate mode.
		uvmode = 'UV'
		uvmapping = 'FLAT'
		if uvtexture and uvlayer:
			if bmat and len(bmat.texture_slots):
				if bmat.texture_slots[0] and bmat.texture_slots[0].texture_coords == 'ORCO':
					uvmode = 'ORCO'
				if bmat.texture_slots[0] and bmat.texture_slots[0].mapping != 'FLAT':
					uvmapping = bmat.texture_slots[0].mapping
		else:
			uvmode = 'ORCO'
			uvmapping = 'SPHERE'
		# Generate the texture coordinates.
		if uvmapping == 'FLAT':
			uvs = self.generate_texcoord_uv(face, verts, uvlayer)
		else:
			if uvmapping != 'SPHERE' or uvmode != 'ORCO':
				print("Warning: UV mode and mapping combination not properly handled, treating as SPHERE ORCO. Mapping: %s , Mode: %s" % (uvmapping, uvmode))
			uvs = self.generate_texcoord_orco_sphere(face, verts)
		# Emit material.
		key = ((bmat is None) and -1 or idx, bimg and bimg.name or '')
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
					if not group.name.startswith('#'):
						# Bone weight.
						if group.name not in self.weightgroupdict:
							grpidx = len(self.weightgrouplist)
							self.weightgroupdict[group.name] = grpidx
							self.weightgrouplist.append(group.name)
						else:
							grpidx = self.weightgroupdict[group.name]
						weights.append((grpidx, weight.weight))
					elif group.name.startswith('#P:'):
						# Partition.
						name = group.name[3:]
						part = self.partitions.get(name)
						index = len(self.vertlist)
						if part:
							part.append((index, weight.weight))
						else:
							self.partitions[name] = [(index, weight.weight)]
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
		# Emit the faces.
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

	def write_partitions(self, writer):
		if not self.partitions:
			return False
		names = sorted([k for k in self.partitions])
		writer.write_int(len(names))
		for name in names:
			writer.write_marker()
			weights = self.partitions[name]
			writer.write_string(name)
			writer.write_int(len(weights))
			for w in weights:
				writer.write_int(w[0])
				writer.write_float(w[1])
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
