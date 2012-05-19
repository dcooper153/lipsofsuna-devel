import bpy
from .format import *

class LIExportFinishedDialog(bpy.types.Operator):
	bl_idname = "object.lipsofsuna_export_finished_operator"
	bl_label = "EXPORTING FINISHED"

	def draw(self, context):
		layout = self.layout
		if len(LIFormat.files):
			col = layout.column()
			flow = col.row()
			col1 = flow.column()
			col1.label(text = "FILE")
			col2 = flow.column()
			col2.scale_x = 0.3
			col2.label(text = "VTX")
			col3 = flow.column()
			col3.scale_x = 0.15
			col3.label(text = "MAT")
			col4 = flow.column()
			col4.scale_x = 0.15
			col4.label(text = "ANI")
			col5 = flow.column()
			col5.scale_x = 0.15
			col5.label(text = "NOD")
			col6 = flow.column()
			col6.scale_x = 0.15
			col6.label(text = "KEY")
			for file in LIFormat.files:
				info = file.get_finish_info()
				col1.label(text = file.filename)
				col2.label(text = "%s" % info[0])
				col3.label(text = "%s" % info[1])
				col4.label(text = "%s" % info[2])
				col5.label(text = "%s" % info[3])
				col6.label(text = "%s" % info[4])
			layout.column()
		else:
			col = layout.column()
			col.label(text = "No objects were exported.")
			row = col.row()
			row.label(text = "Add the custom property 'file' to your objects.")

	def execute(self, context):
		return {'FINISHED'}

	def invoke(self, context, event):
		return context.window_manager.invoke_props_dialog(self)
