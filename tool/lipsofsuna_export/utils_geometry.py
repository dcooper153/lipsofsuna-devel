import bpy, mathutils

class LIUtilsGeometry:

	@classmethod
	def calculate_smooth_area_normal(cls, object, face, point, radius):
		"""Calculates a smooth normal for a point on a face and an area around it.

		Parameters:
			object: Object.
			face: Face index.
			point: Point on the face.
			radius: Area radius.

		Returns:
			Normal vector.
		"""

		# Calculate the unsmoothed normal.
		nml = cls.calculate_smooth_face_normal(object, face, point)
		# Find the connected vertices within the given radius.
		vtx = {}
		for v in object.data.polygons[face].vertices:
			p = mathutils.Vector(object.data.vertices[v].co)
			r = radius - (p - point).length
			if r > 0:
				for x in cls.find_connected_vertices_within_radius(object, v, r):
					dist = vtx.get(x[0])
					if not dist or dist > x[1]:
						vtx[x[0]] = x[1]
		# Blend in the normals of the vertices.
		for idx,dist in vtx.items():
			weight = dist / radius
			nml += cls.calculate_smooth_vertex_normal(object, idx) * weight
		nml.normalize()
		return nml

	@classmethod
	def calculate_smooth_area_normal_shapekey(cls, object, key, face, point, radius):
		"""Calculates a smooth normal for a point on a face and an area around it.

		Parameters:
			object: Object.
			key: Shape key.
			face: Face index.
			point: Point on the face.
			radius: Area radius.

		Returns:
			Normal vector.
		"""

		# Calculate the unsmoothed normal.
		nml = cls.calculate_smooth_face_normal_shapekey(object, key, face, point)
		# Find the connected vertices within the given radius.
		vtx = {}
		for v in object.data.polygons[face].vertices:
			p = mathutils.Vector(object.data.vertices[v].co)
			r = radius - (p - point).length
			if r > 0:
				# TODO: Should this use the shape key too?
				for x in cls.find_connected_vertices_within_radius(object, v, r):
					dist = vtx.get(x[0])
					if not dist or dist > x[1]:
						vtx[x[0]] = x[1]
		# Blend in the normals of the vertices.
		for idx,dist in vtx.items():
			weight = dist / radius
			nml += cls.calculate_smooth_vertex_normal_shapekey(object, key, idx) * weight
		nml.normalize()
		return nml

	@classmethod
	def calculate_smooth_face_normal(cls, object, face, point):
		"""Calculates a smooth normal for a point on a face.

		Parameters:
			object: Object.
			face: Face index.
			point: Point on the face.

		Returns:
			Normal vector.
		"""

		face = object.data.polygons[face]
		vtx = [mathutils.Vector(object.data.vertices[v].co) for v in face.vertices]
		nml = [cls.calculate_smooth_vertex_normal(object, v) for v in face.vertices]
		return cls.interpolate_face(vtx, point, nml)

	@classmethod
	def calculate_smooth_face_normal_shapekey(cls, object, key, face, point):
		"""Calculates a smooth normal for a point on a face.

		Parameters:
			object: Object.
			key: Shape key.
			face: Face index.
			point: Point on the face.

		Returns:
			Normal vector.
		"""

		face = object.data.polygons[face]
		vtx = [mathutils.Vector(key.data[v].co) for v in face.vertices]
		nml = [cls.calculate_smooth_vertex_normal_shapekey(object, key, v) for v in face.vertices]
		return cls.interpolate_face(vtx, point, nml)

	@classmethod
	def calculate_smooth_vertex_normal(cls, object, vertex):
		"""Calculates a smooth normal for a vertex.

		Parameters:
			object: Object.
			vertex: Vertex index.

		Returns:
			Normal vector.
		"""

		faces = cls.find_faces_using_vertex(object, vertex)
		if len(faces):
			normal = mathutils.Vector((0.0, 0.0, 0.0))
			for f in faces:
				face = object.data.polygons[f]
				src = [mathutils.Vector(object.data.vertices[v].co) for v in face.vertices]
				n1 = mathutils.geometry.normal(src[0], src[1], src[2])
				normal += n1
			normal.normalize()
		else:
			normal = mathutils.Vector((0.0, 0.0, 1.0))
		return normal

	@classmethod
	def calculate_smooth_vertex_normal_shapekey(cls, object, key, vertex):
		"""Calculates a smooth normal for a vertex of a shape key.

		Parameters:
			object: Object.
			key: Shape key.
			vertex: Vertex index.

		Returns:
			Normal vector.
		"""

		faces = cls.find_faces_using_vertex(object, vertex)
		if len(faces):
			normal = mathutils.Vector((0.0, 0.0, 0.0))
			for f in faces:
				face = object.data.polygons[f]
				src = [mathutils.Vector(key.data[v].co) for v in face.vertices]
				n1 = mathutils.geometry.normal(src[0], src[1], src[2])
				normal += n1
			normal.normalize()
		else:
			normal = mathutils.Vector((0.0, 0.0, 1.0))
		return normal

	@classmethod
	def calculate_barycentric_matrix(cls, tri, nml=None):
		if nml:
			n = nml
		else:
			n = mathutils.geometry.normal(tri[0], tri[1], tri[2])
		u = tri[0] + n
		mat = mathutils.Matrix()
		mat[0][0] = tri[0].x - u.x
		mat[1][0] = tri[1].x - u.x
		mat[2][0] = tri[2].x - u.x
		mat[0][1] = tri[0].y - u.y
		mat[1][1] = tri[1].y - u.y
		mat[2][1] = tri[2].y - u.y
		mat[0][2] = tri[0].z - u.z
		mat[1][2] = tri[1].z - u.z
		mat[2][2] = tri[2].z - u.z
		return (mat, u)

	@classmethod
	def convert_to_barycentric(cls, tri, point, nml=None):
		(mat,ref) = cls.calculate_barycentric_matrix(tri, nml)
		mat.invert()
		return (point - ref) * mat

	@classmethod
	def convert_from_barycentric(cls, tri, bary, nml=None):
		(mat,ref) = cls.calculate_barycentric_matrix(tri, nml)
		return (bary * mat) + ref

	@classmethod
	def interpolate_barycentric(cls, tri, point, values):
		"""Performs barycentric interpolation of values on the triangle.

		Parameters:
			tri: Triangle vertex coordinates.
			point: Point in object space.
			values: Values assigned to triangle vertices.

		Returns:
			Interpolated value.
		"""

		bary = cls.convert_from_barycentric(tri, point)
		val = bary.x * values[0] + bary.y * values[1] + bary.z * values[2]
		return val * (1.0 / (bary.x + bary.y + bary.z))

	@classmethod
	def interpolate_face(cls, vertices, point, values):
		"""Performs interpolation of values on the triangle or quad.

		Parameters:
			tri: Triangle or quad vertex coordinates.
			point: Point in object space.
			values: Values assigned to triangle or quad vertices.

		Returns:
			Interpolated value.
		"""

		if len(vertices) == 3:
			return cls.interpolate_barycentric(vertices, point, values)
		else:
			vtx1 = [vertices[0], vertices[1], vertices[2]]
			vtx2 = [vertices[0], vertices[2], vertices[3]]
			vals1 = [values[0], values[1], values[2]]
			vals2 = [values[0], values[2], values[3]]
			val1 = cls.interpolate_barycentric(vtx1, point, vals1)
			val2 = cls.interpolate_barycentric(vtx2, point, vals2)
			return (val1 + val2) * 0.5

	@classmethod
	def find_closest_point(cls, object, point):
		# Find the closest vertex.
		best = None
		best_diff = None
		best_dist = None
		for i in range(len(object.data.vertices)):
			diff = object.data.vertices[i].co - point
			dist = diff.length
			if best == None or dist < best_dist:
				best = i
				best_diff = diff
				best_dist = dist
		# Return the vertex if the distance to it is very small.
		if best and best_dist <= 0.01:
			return {'type': 'vertex', 'vertex': best, 'point': point, 'offset': best_diff}
		# Return the closest face.
		(loc,nml,idx) = object.closest_point_on_mesh(point)
		if idx != -1:
			return {'type': 'face', 'face': idx, 'point': point, 'surface_normal': nml, 'surface_point': loc}
		# Return the closest vertex if there were no faces.
		if best:
			return {'type': 'vertex', 'vertex': best, 'point': point, 'offset': best_diff}

	@classmethod
	def find_closest_points(cls, srcobj, dstobj):
		res = []
		for i in range(len(dstobj.data.vertices)):
			vertex = dstobj.data.vertices[i]
			closest = cls.find_closest_point(srcobj, vertex.co)
			if not closest:
				return None
			res.append(closest)
		return res

	@classmethod
	def find_connected_vertices_within_radius(cls, object, vertex, radius):
		"""Returns connected vertices within the given radius.

		Parameters:
			object: Object.
			vertex: Vertex index.
			radius: Radius in object space.

		Returns:
			List of vertex indices.
		"""

		edges = object.data.edge_keys
		infinity = 1000000.0
		def dist_between(u, v):
			u1 = mathutils.Vector(object.data.vertices[u].co)
			v1 = mathutils.Vector(object.data.vertices[v].co)
			return (u1 - v1).length
		# Find vertices that are inside the search space.
		dist = {}
		left = {}
		vert = mathutils.Vector(object.data.vertices[vertex].co)
		for i,v in enumerate(object.data.vertices):
			if (mathutils.Vector(v.co) - vert).length <= radius:
				dist[i] = infinity
				left[i] = infinity
		dist[vertex] = 0.0
		# Find the connected vertices using Dijkstra's algorithm.
		while left:
			u = min(left, key = lambda x: left.get(x))
			if dist[u] == infinity:
				break
			del left[u]
			for v in cls.find_neightbor_vertices(object, edges, u):
				if v in left:
					d = min(dist[v], dist[u] + dist_between(u, v))
					dist[v] = d
					left[v] = d
		# Return the connected vertices in the search space.
		return [(k,v) for k,v in dist.items() if v != infinity]

	@classmethod
	def find_faces_using_vertex(cls, object, vertex):
		"""Returns a list of indices for faces that contain the vertex.

		Parameters:
			object: Object.
			vertex: Vertex index.

		Returns:
			List of face indices.
		"""

		res = []
		for i,p in enumerate(object.data.polygons):
			if vertex in p.vertices:
				res.append(i)
		return res

	@classmethod
	def find_neightbor_vertices(cls, object, edges, vertex):
		"""Returns directly connected vertices.

		Parameters:
			object: Object.
			edges: Edge list for the object.
			vertex: Vertex index.

		Returns:
			List of vertex indices.
		"""

		return [x[0] != vertex and x[0] or x[1]\
			for x in edges\
			if vertex == x[0] or vertex == x[1]]
