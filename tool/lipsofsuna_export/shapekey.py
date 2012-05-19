import mathutils

class LIShapeKey:

	def __init__(self, mesh, obj, bl_key):
		self.name = bl_key.name
		self.vertices = []
		self.empty = True
		# Calculate shape key vertex normals.
		dict_index = {}
		for face in obj.data.polygons:
			coords = [bl_key.data[face.vertices[i]].co for i in range(len(face.vertices))]
			if len(face.vertices) == 3:
				nml = mathutils.geometry.normal(coords[0], coords[1], coords[2])
			else:
				nml = mathutils.geometry.normal(coords[0], coords[1], coords[2], coord[3])
			for i in face.vertices:
				if i not in dict_index:
					dict_index[i] = [nml]
				else:
					dict_index[i].append(nml)
		dict_normal = {}
		for i in dict_index:
			arr = dict_index[i]
			nml = mathutils.Vector((0, 0, 0))
			for n in arr:
				nml += n
			nml.normalize()
			dict_normal[i] = nml
		# Create shape key vertices.
		for li_idx in range(len(mesh.vertmapping)):
			bl_idx = mesh.vertmapping[li_idx]
			key = bl_key.data[bl_idx]
			vtx = obj.data.vertices[bl_idx]
			if bl_idx in dict_normal:
				nml = dict_normal[bl_idx]
			else:
				nml = vtx.normal
			self.vertices.append((key.co, nml))
			if key.co != vtx.co:
				self.empty = False

	def write(self, writer):
		writer.write_string(self.name)
		writer.write_int(len(self.vertices))
		writer.write_marker()
		for v in self.vertices:
			writer.write_float(v[0].x)
			writer.write_float(v[0].y)
			writer.write_float(v[0].z)
			writer.write_float(v[1].x)
			writer.write_float(v[1].y)
			writer.write_float(v[1].z)
			writer.write_marker()
