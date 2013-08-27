import bpy
from .update_shape_keys import *

class LIUpdateShapeKeysOperator(bpy.types.Operator):
	bl_idname = "wm.lipsofsuna_update_shape_keys"
	bl_label = "Update Shape Keys"
	bl_description = 'Update shape keys from another object that has exactly same basis shape'

	li_object = bpy.props.StringProperty(name='Source object', description='Object from which to get the updated the shape keys')

	def draw(self, context):
		layout = self.layout
		box = layout.column()
		box.prop(self, 'li_object')

	def execute(self, context):
		src = bpy.context.blend_data.objects.get(self.li_object)
		dst = bpy.context.active_object
		err = LIUpdateShapeKeys.validate(src, dst)
		if not err:
			count = LIUpdateShapeKeys.run(src, dst)
			self.report({'INFO'}, "%d shape keys changed" % count)
		else:
			self.report({'ERROR'}, err)
		return {'FINISHED'}

	def invoke(self, context, event):
		self.li_object = ''
		for obj in bpy.context.selected_objects:
			if obj.type == 'MESH' and obj != bpy.context.active_object:
				self.li_object = obj.name
				break
		return context.window_manager.invoke_props_dialog(self)
