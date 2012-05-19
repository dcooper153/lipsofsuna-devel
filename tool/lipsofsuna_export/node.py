import re
import bpy
from .format import *
from .utils import *

class LINode:

	# \brief Creates a new generic object node.
	# \param object Blender object.
	# \param parent Parent node.
	def __init__(self, parent, object):
		self.type = LIFormat.NODETYPE_EMPTY
		self.nodes = []
		self.object = object
		self.parent = parent
		self.hierarchy = self.parent.hierarchy
		self.loc,self.rot = self.get_rest_transform()
		# Remove the number part so that it's possible to have multiple
		# objects that have anchors with the same names.
		self.name = '#' + re.sub("\\.[0-9]+", "", object.name)

	def add_object(self, object):
		if LIUtils.object_check_export(object, self.hierarchy.file, 'NODE'):
			if object.type == 'ARMATURE':
				self.add_node(node_armature.LINodeArmature(self, object))
			if object.type == 'EMPTY':
				self.add_node(LINode(self, object))
			if object.type == 'LAMP' and (object.data.type == 'SPOT' or object.data.type == 'POINT'):
				self.add_node(node_light.LINodeLight(self, object))

	def add_node(self, node):
		self.nodes.append(node)
		self.hierarchy.add_node(node)

	# \brief Recursively adds children to the node.
	def add_children(self):
		# Add child objects.
		for obj in bpy.data.objects:
			if self.object == obj.parent and not obj.parent_bone:
				self.add_object(obj)
		# Recurse to children.
		for node in self.nodes:
			node.add_children()

	# \brief Gets the parent relative rest transformation of the node.
	def get_rest_transform(self):
		matrix = self.object.matrix_local.copy()
		if self.object.parent_bone:
			parentbone = self.object.parent.pose.bones[self.object.parent_bone]
			parentmatr = parentbone.matrix * mathutils.Matrix.Translation(self.parent.len)
			matrix = parentmatr.inverted() * matrix
		elif self.object.parent:
			parentmatr = self.object.parent.matrix_local
			matrix = parentmatr.inverted() * matrix
		loc = matrix.to_translation()
		rot = matrix.to_3x3().to_quaternion().normalized()
		return (loc, rot)

	# \brief Writes the node to a stream.
	# \param self Node.
	# \param writer Writer.
	def write(self, writer):
		writer.write_int(self.type)
		writer.write_int(len(self.nodes))
		writer.write_string(self.name)
		writer.write_float(self.loc.x)
		writer.write_float(self.loc.y)
		writer.write_float(self.loc.z)
		writer.write_float(self.rot.x)
		writer.write_float(self.rot.y)
		writer.write_float(self.rot.z)
		writer.write_float(self.rot.w)
		self.write_special(writer)
		writer.write_marker()
		for node in self.nodes:
			node.write(writer)

	def write_special(self, writer):
		pass
