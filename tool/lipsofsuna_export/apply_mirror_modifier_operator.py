import bpy
from .modifier_mirror import *
from .utils import *

class LIApplyMirrorModifierOperator(bpy.types.Operator):
	bl_idname = "wm.lipsofsuna_apply_mirror_modifier"
	bl_label = "Apply Mirror Modifier"
	bl_description = 'Apply a mirror modifier even if the mesh has shape keys'

	def execute(self, context):
		oldobj = bpy.context.active_object
		for mod in oldobj.modifiers:
			if mod.type == "MIRROR":
				oldobj.select = False
				m = LIModifierMirror(oldobj, mod)
				newobj = m.apply()
				newobj.select = True
		return {'FINISHED'}

	def invoke(self, context, event):
		return self.execute(context)
