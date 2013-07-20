import bpy
from .file import *
from .format import *
from .utils import *

class LIExporter(bpy.types.Operator):
	'''Export to Lips of Suna (.lmdl)'''

	bl_idname = "export.lipsofsuna"
	bl_label = 'Export to Lips of Suna (.lmdl)'

	def execute(self, context):
		if self.process():
			bpy.context.scene.frame_set(bpy.context.scene.frame_current)
			return {'RUNNING_MODAL'}
		else:
			return {'FINISHED'}

	def invoke(self, context, event):
		LIFormat.files = []
		self.done = False
		self.state = 0
		self.files = []
		self.message = ""
		self.orig_layers = [x for x in bpy.context.scene.layers]
		self.orig_frame = bpy.context.scene.frame_current
		# Initialize the state.
		bpy.context.scene.layers = [True for x in range(0, 20)]
		active = bpy.context.scene.objects.active
		if active and not active.library:
			bpy.ops.object.mode_set(mode='OBJECT', toggle=False)
		# Find exported files.
		for obj in bpy.data.objects:
			for file in LIUtils.get_files_for_object(obj):
				if file not in self.files:
					self.files.append(file)
		self.files.sort()
		# Start exporting the files.
		# It seems that there's no way to update the GUI without user
		# input so we have to export in one pass even though our code
		# could easily do it iteratively.
		bpy.context.user_preferences.edit.use_global_undo = False
		while self.process():
			pass
		bpy.context.user_preferences.edit.use_global_undo = True
		return {'FINISHED'}

	def process(self):
		if self.state < len(self.files):
			# Get the processed file.
			if len(LIFormat.files) <= self.state:
				file = LIFile(self.files[self.state])
				LIFormat.files.append(file)
			else:
				file = LIFormat.files[self.state]
			# Process the file some.
			if file.process():
				file.write()
				file.write_anims()
				self.state += 1
			self.message = file.get_progress_info()
			return True
		elif self.state == len(self.files):
			# Restore state.
			bpy.context.scene.layers = self.orig_layers
			bpy.context.scene.frame_set(self.orig_frame)
			# Show the results.
			bpy.ops.object.lipsofsuna_export_finished_operator('INVOKE_DEFAULT')
			self.state += 1
			return False
		else:
			return False
