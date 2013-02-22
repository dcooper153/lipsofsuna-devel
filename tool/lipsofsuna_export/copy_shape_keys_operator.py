import bpy
from .copy_shape_keys import *

class LICopyShapeKeysOperator(bpy.types.Operator):
	bl_idname = "wm.lipsofsuna_copy_shape_keys"
	bl_label = "Copy Shape Keys"
	bl_description = 'Copy shape keys from another object that has different geometry'

	li_object = bpy.props.StringProperty(name='Source object', description='Object from which to copy the shape keys')
	li_vertex = bpy.props.BoolProperty(name='Vertex snapping only', description='Only copy vertices that close to each other')
	li_clipping = bpy.props.BoolProperty(name='X-axis clipping', description='Keeps the vertices in the same side of the X-axis')
	li_smooth = bpy.props.BoolProperty(name='Alternative normal calculation', description='Slow and mostly useless for now')

	def draw(self, context):
		layout = self.layout
		box = layout.column()
		box.prop(self, 'li_object')
		box.prop(self, 'li_vertex')
		box.prop(self, 'li_clipping')
		box.prop(self, 'li_smooth')

	def execute(self, context):
		src = bpy.context.blend_data.objects.get(self.li_object)
		dst = bpy.context.active_object
		err = LICopyShapeKeys.validate(src, dst)
		if not err:
			count = LICopyShapeKeys.run(src, dst, self.li_smooth, self.li_clipping, self.li_vertex)
			self.report({'INFO'}, "%d shape keys added" % count)
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
