import bpy
from .modifier_edgesplit import *

class LIApplyEdgeSplitModifierOperator(bpy.types.Operator):
	bl_idname = "wm.lipsofsuna_apply_edgesplit_modifier"
	bl_label = "Apply Edge Split Modifier"
	bl_description = 'Apply an edge split modifier even if the mesh has shape keys'

	def execute(self, context):
		oldobj = bpy.context.active_object
		for mod in oldobj.modifiers:
			if mod.type == "EDGE_SPLIT":
				oldobj.select = False
				m = LIModifierEdgeSplit(oldobj, mod)
				newobj = m.apply()
				newobj.select = True
		return {'FINISHED'}

	def invoke(self, context, event):
		return self.execute(context)
