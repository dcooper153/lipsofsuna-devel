from .node import *
from .node_bone import *

class LINodeArmature(LINode):

	def __init__(self, parent, object):
		LINode.__init__(self, parent, object)
		self.name = object.name

	def add_children(self):
		# Add root bones.
		for bone in self.object.data.bones.values():
			if not bone.parent:
				self.add_node(LINodeBone(self, bone))
		# Add child objects.
		LINode.add_children(self)
