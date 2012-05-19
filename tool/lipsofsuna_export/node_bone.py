import bpy
from .format import *
from .node import *

class LINodeBone(LINode):

	def __init__(self, parent, bone):
		self.type = LIFormat.NODETYPE_BONE
		self.nodes = []
		self.object = parent.object
		self.parent = parent
		self.hierarchy = self.parent.hierarchy
		self.bone = bone
		self.pose_bone = self.object.pose.bones[bone.name]
		self.name = bone.name
		self.len = mathutils.Vector((0, bone.length, 0))
		self.head = bone.head.copy()
		self.matrix = bone.matrix.copy()
		self.loc,self.rot = self.get_rest_transform()

	def add_children(self):
		# Add child bones.
		for bone in self.object.data.bones.values():
			if bone.parent == self.bone:
				self.add_node(LINodeBone(self, bone))
		# Add child objects.
		for obj in bpy.data.objects:
			if self.object == obj.parent and self.bone.name == obj.parent_bone:
				self.add_object(obj)
		# Recurse to children.
		for node in self.nodes:
			node.add_children()

	# \brief Gets the parent relative rest transformation of the node.
	def get_rest_transform(self):
		matrix = self.get_head_rest_matrix()
		if self.bone.parent:
			matrix = self.parent.get_tail_rest_matrix().inverted() * matrix
		loc = matrix.to_translation()
		rot = matrix.to_3x3().to_quaternion().normalized()
		return (loc, rot)

	# \brief Gets the parent relative pose transformation of the node.
	def get_pose_transform(self):
		rest = self.get_head_rest_matrix()
		pose = self.get_head_pose_matrix()
		if self.bone.parent:
			rest = self.parent.get_tail_rest_matrix().inverted() * rest
			pose = self.parent.get_tail_pose_matrix().inverted() * pose
		matrix = rest.inverted() * pose
		loc = matrix.to_translation()
		rot = matrix.to_3x3().to_quaternion().normalized()
		return (loc, rot)

	# \brief Gets the armature space rest matrix of the bone head.
	def get_head_rest_matrix(self):
		return self.bone.matrix_local.copy()

	# \brief Gets the armature space rest matrix of the bone tail.
	def get_tail_rest_matrix(self):
		return self.get_head_rest_matrix() * mathutils.Matrix.Translation(self.len)

	# \brief Gets the armature space pose transformation of the bone head.
	def get_head_pose_matrix(self):
		return self.pose_bone.matrix.copy()

	# \brief Gets the armature space pose transformation of the bone tail.
	def get_tail_pose_matrix(self):
		return self.get_head_pose_matrix() * mathutils.Matrix.Translation(self.len)

	def write_special(self, writer):
		writer.write_float(self.len.x)
		writer.write_float(self.len.y)
		writer.write_float(self.len.z)
