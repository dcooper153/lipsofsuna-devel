import bpy

class LIMaterialPropertyPanel(bpy.types.Panel):
	bl_idname = "OBJECT_PT_lipsofsuna_material_property"
	bl_label = "Lips of Suna"
	bl_space_type = 'PROPERTIES'
	bl_region_type = 'WINDOW'
	bl_context = "material"
	bl_options = {'DEFAULT_CLOSED'}

	@classmethod
	def poll(cls, context):
		return (context.object is not None)

	def draw(self, context):
		layout = self.layout
		col = layout.column()
		col.operator("wm.lipsofsuna_material_export_settings", text="Exporting Settings", icon="FILE")
