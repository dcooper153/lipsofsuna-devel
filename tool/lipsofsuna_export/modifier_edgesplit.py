import bpy, mathutils
from .modifier import *

class LIModifierEdgeSplit(LIModifier):

	def __init__(self, obj, mod):
		LIModifier.__init__(self, obj)

	def _finish(self):
		# Unselect everything.
		bpy.ops.object.mode_set(mode='EDIT')
		bpy.context.tool_settings.mesh_select_mode = (False, True, False)
		bpy.ops.object.mode_set(mode='OBJECT')
		for e in self.new_mesh.edges:
			e.select = False
		# Force the seams and sharp edges to render.
		bpy.ops.object.mode_set(mode='EDIT')
		bpy.ops.mesh.mark_seam()
		bpy.ops.mesh.mark_sharp()
		# Select all sharp edges.
		bpy.ops.object.mode_set(mode='OBJECT')
		for e in self.new_mesh.edges:
			if e.use_edge_sharp:
				e.select = True
				break
		bpy.ops.object.mode_set(mode='EDIT')
		bpy.ops.mesh.select_similar(type='SHARP')
		# Split the selected edges.
		bpy.ops.mesh.edge_split()
		bpy.ops.object.mode_set(mode='OBJECT')
