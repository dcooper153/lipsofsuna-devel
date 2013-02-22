import bpy, mathutils
from .utils_geometry import *

class LICopyShapeKeys:

	@classmethod
	def compare_shape_keys(cls, key1, key2):
		threshold = 0.01
		for i in range(len(key1.data)):
			v1 = mathutils.Vector(key1.data[i].co)
			v2 = mathutils.Vector(key2.data[i].co)
			if (v1 - v2).length >= threshold:
				return False
		return True

	@classmethod
	def transfer_key(cls, srcobj, srckey, dstobj, dstkey, closest, smoothing, clipping, vertex):
		for i in range(len(closest)):
			cls.transfer_vertex(srcobj, srckey, dstobj, dstkey, closest, i, smoothing, clipping, vertex)

	@classmethod
	def transfer_vertex(cls, srcobj, srckey, dstobj, dstkey, closest, index, smoothing, clipping, vertex):
		# Derive the vertex position in the shape key.
		mapping = closest[index]
		if mapping['type'] == 'vertex':
			# Exact vertex match.
			dstkey.data[index].co = srckey.data[mapping['vertex']].co
		elif not vertex:
			# Surface match with a potential normal component.
			face = srcobj.data.polygons[mapping['face']]
			src = [mathutils.Vector(srcobj.data.vertices[v].co) for v in face.vertices]
			dst = [mathutils.Vector(srckey.data[v].co) for v in face.vertices]
			if smoothing:
				f = mapping['face']
				p = mapping['surface_point']
				normal = LIUtilsGeometry.calculate_smooth_face_normal_shapekey(srcobj, srckey, f, p)
			else:
				normal = None
			if len(face.vertices) == 4:
				co = cls.transform_vertex_on_quad(mapping['point'], src, dst, normal)
			else:
				co = cls.transform_vertex_on_triangle(mapping['point'], src, dst, normal)
			dstkey.data[index].co = co
		else:
			return
		# Enforce X-axis clipping.
		if clipping:
			basis_co = mathutils.Vector(dstobj.data.vertices[index].co)
			shape_co = mathutils.Vector(dstkey.data[index].co)
			if abs(basis_co.x) < 0.01:
				shape_co.x = 0.0
			elif basis_co.x < 0.0:
				if shape_co.x > 0.0:
					shape_co.x = 0.0
			else:
				if shape_co.x < 0.0:
					shape_co.x = 0.0
			dstkey.data[index].co = shape_co

	@classmethod
	def transform_vertex_on_quad(cls, point, src, dst, normal):
		# Find the closest vertex to the point.
		best = 0
		best_dist = 1000000
		for i in range(len(src)):
			dist = (point - src[i]).length
			if dist < best_dist:
				best = i
				best_dist = dist
		# Calculate the transformation for the closest corners.
		src1 = [src[i % 4] for i in range(best,best+3)]
		dst1 = [dst[i % 4] for i in range(best,best+3)]
		res1 = cls.transform_vertex_on_triangle(point, src1, dst1, normal)
		return res1

	@classmethod
	def transform_vertex_on_triangle(cls, point, src, dst, normal):
		tmp1 = LIUtilsGeometry.convert_to_barycentric(src, point)
		tmp2 = LIUtilsGeometry.convert_from_barycentric(dst, tmp1, normal)
		return tmp2

	@classmethod
	def run(cls, srcobj, dstobj, smooth, clipping, vertex):
		# Disable undo.
		bpy.context.user_preferences.edit.use_global_undo = False
		toggled = False
		if dstobj.mode == "EDIT":
			toggled = True
			bpy.ops.object.editmode_toggle()
		# Copy shape keys.
		dict_added = {}
		count_added = 0
		smoothing = smooth and 0.001 or 0.0
		closest = LIUtilsGeometry.find_closest_points(srcobj, dstobj)
		if closest:
			for srckey in srcobj.data.shape_keys.key_blocks:
				if not dstobj.data.shape_keys or srckey.name not in dstobj.data.shape_keys.key_blocks:
					dict_added[srckey.name] = True
					count_added += 1
					dstobj.shape_key_add(name=srckey.name, from_mix=False)
					dstkey = dstobj.data.shape_keys.key_blocks[srckey.name]
					cls.transfer_key(srcobj, srckey, dstobj, dstkey, closest, smoothing, clipping, vertex)
		# Remove useless shape keys.
		if count_added:
			ref = None
			i = 0
			while i < len(dstobj.data.shape_keys.key_blocks):
				key = dstobj.data.shape_keys.key_blocks[i]
				if not ref:
					ref = key
				elif key.name in dict_added:
					if cls.compare_shape_keys(ref, key):
						dstobj.active_shape_key_index = i
						bpy.ops.object.shape_key_remove()
						count_added -= 1
						continue
				i += 1
		# Restore state.
		if toggled:
			bpy.ops.object.editmode_toggle()
		bpy.context.user_preferences.edit.use_global_undo = True
		return count_added

	@classmethod
	def validate(cls, srcobj, dstobj):
		# Make sure that the objects exist.
		if not srcobj:
			return "No source object was found."
		if srcobj == dstobj:
			return "The source and the destination objects are the same object."
		# Make sure that the objects are aligned.
		if (srcobj.location - dstobj.location).length > 0.01:
			return "The source and destination objects are not in the same position."
		# Make sure the source object doesn't have nasty modifiers.
		# FIXME: Disabling them would be nicer but Blender doesn't make it easy.
		for index,mod in enumerate(srcobj.modifiers):
			if mod.type == 'MIRROR':
				if mod.use_x or mod.use_y or mod.use_z:
					return "You must disable mirror modifiers from the source object."
