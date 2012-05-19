import bpy
from .format import *
from .node import *
from .node_armature import *
from .node_light import *
from .utils import *

class LINodeRoot(LINode):

	def __init__(self, hierarchy):
		self.type = LIFormat.NODETYPE_EMPTY
		self.nodes = []
		self.object = None
		self.parent = None
		self.hierarchy = hierarchy
		self.name = '#'
		self.rot = LIFormat.quat
		self.loc = mathutils.Vector((0, 0, 0))

	# \brief Recursively adds children to the node.
	def add_children(self):
		def add_object(object):
			if LIUtils.object_check_export(object, self.hierarchy.file, 'NODE'):
				if object.type == 'ARMATURE':
					self.add_node(LINodeArmature(self, object))
				if object.type == 'EMPTY':
					self.add_node(LINode(self, object))
				if object.type == 'LAMP' and (object.data.type == 'SPOT' or object.data.type == 'POINT'):
					self.add_node(LINodeLight(self, object))
		# Add child objects.
		for object in bpy.data.objects:
			if object.parent == None:
				# Add unparented objects.
				add_object(object)
			elif object.parent.type == "MESH":
				# Add objects parented to meshes that belong to the same file.
				if LIUtils.object_check_export(object.parent, self.hierarchy.file, 'NODE'):
					add_object(object)
		# Recurse to children.
		for node in self.nodes:
			node.add_children()
