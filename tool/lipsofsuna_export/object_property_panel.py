import bpy

class LIObjectPropertyPanel(bpy.types.Panel):
	bl_idname = "OBJECT_PT_lipsofsuna_object_property"
	bl_label = "Lips of Suna"
	bl_space_type = 'PROPERTIES'
	bl_region_type = 'WINDOW'
	bl_context = "object"
	bl_options = {'DEFAULT_CLOSED'}

	@classmethod
	def poll(cls, context):
		return (context.object is not None)

	def draw(self, context):
		layout = self.layout
		col = layout.column()
		col.operator("wm.lipsofsuna_object_export_settings", text="Exporting Settings", icon="FILE")
		col.operator("wm.lipsofsuna_copy_shape_keys", text="Copy Shape Keys", icon="COPY_ID")
		col.operator("wm.lipsofsuna_copy_weight_groups", text="Copy Weight Groups", icon="COPY_ID")
