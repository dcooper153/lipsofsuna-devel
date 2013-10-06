import bpy
from .copy_weight_groups import *

class LICopyWeightGroupsOperator(bpy.types.Operator):
	bl_idname = "wm.lipsofsuna_copy_weight_groups"
	bl_label = "Copy Weight Groups"
	bl_description = 'Copy weight groups from another object that has different geometry'

	li_object = bpy.props.StringProperty(name='Source object', description='Object from which to copy the weight groups')
	li_vertex = bpy.props.BoolProperty(name='Vertex snapping only', description='Only copy vertices that are close to each other')
	li_overwrite = bpy.props.BoolProperty(name='Overwrite existing groups', description='Overwrite groups that already exist')

	def draw(self, context):
		layout = self.layout
		box = layout.column()
		box.prop(self, 'li_object')
		box.prop(self, 'li_vertex')
		box.prop(self, 'li_overwrite')

	def execute(self, context):
		src = bpy.context.blend_data.objects.get(self.li_object)
		dst = bpy.context.active_object
		err = LICopyWeightGroups.validate(src, dst)
		if not err:
			count = LICopyWeightGroups.run(src, dst, self.li_vertex, self.li_overwrite)
			self.report({'INFO'}, "%d weight groups added" % count)
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
