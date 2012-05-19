from .format import *
from .shape import *

class LICollision:

	# \brief Initializes a new collision shape manager.
	# \param self Shape manager.
	def __init__(self):
		self.shapes = {}

	# \brief Builds the default collision mesh from the graphical mesh.
	# \param object Mesh object.
	def add_default(self, object):
		if 'default' not in self.shapes:
			# Calculate the bounding box of the object.
			bmin = mathutils.Vector(object.bound_box[0])
			bmax = mathutils.Vector(object.bound_box[0])
			for b in object.bound_box:
				if bmin[0] > b[0]:
					bmin[0] = b[0]
				if bmin[1] > b[1]:
					bmin[1] = b[1]
				if bmin[2] > b[2]:
					bmin[2] = b[2]
				if bmax[0] < b[0]:
					bmax[0] = b[0]
				if bmax[1] < b[1]:
					bmax[1] = b[1]
				if bmax[2] < b[2]:
					bmax[2] = b[2]
			center = 0.5 * (bmax + bmin)
			# Save the old selection.
			selection = []
			for obj in bpy.data.objects:
				if obj.select:
					selection.append(obj)
			# Create an icosphere enclosing the object.
			bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=2, size=0.7*(bmax - bmin).length)
			icohull = None
			for obj in bpy.data.objects:
				if obj.select:
					icohull = obj
					break
			# Add a shrinkwrap modifier.
			modifier = icohull.modifiers.new('shrinkwrap', 'SHRINKWRAP')
			modifier.target = object
			# Apply the shrinkwrap modifier.
			translation = mathutils.Matrix.Translation((center.x, center.y, center.z))
			icohull.matrix_local = object.matrix_local * translation
			oldmesh = icohull.data
			icohull.data = icohull.to_mesh(bpy.context.scene, True, 'PREVIEW')
			bpy.data.meshes.remove(oldmesh)
			# Fix the coordinate system.
			icohull.matrix_local = LIFormat.matrix1 * icohull.matrix_local
			bpy.ops.object.transform_apply(rotation=True)
			# Collect the resulting vertices.
			shape = LIShape('default')
			shape.add_mesh(icohull, center)
			self.shapes['default'] = shape
			# Delete the icosphere.
			bpy.ops.object.delete()
			# Restore the old selection.
			for obj in selection:
				obj.select = True

	# \brief Adds an explicit collision mesh to the list of collision shapes.
	# \param object Mesh object.
	def add_mesh(self, object):
		try:
			prop = object['shape']
			if prop not in self.shapes:
				self.shapes[prop] = LIShape(prop)
			self.shapes[prop].add_mesh(object, mathutils.Vector((0.0, 0.0, 0.0)))
		except:
			pass

	# \brief Saves all shapes.
	# \param self Shape manager.
	# \param writer Writer.
	def write(self, writer):
		if not len(self.shapes):
			return False
		writer.write_int(len(self.shapes))
		writer.write_marker()
		for name in self.shapes:
			self.shapes[name].write(writer)
		return True
