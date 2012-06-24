import bpy

class LICopyShapeSlidersOperator(bpy.types.Operator):
	bl_idname = "wm.lipsofsuna_copy_shape_sliders"
	bl_label = "Copy Shape Key Sliders"
	bl_description = 'Copy shape key slider values from this object to other objects in the scene'

	li_object = bpy.props.StringProperty(name='Source object', description='Object from which to copy the slider values')

	def draw(self, context):
		layout = self.layout
		box = layout.column()
		box.prop(self, 'li_object')

	def execute(self, context):
		srcobj = bpy.context.blend_data.objects.get(self.li_object)
		if srcobj and srcobj.data.shape_keys:
			for srckey in srcobj.data.shape_keys.key_blocks:
				for dstobj in bpy.context.scene.objects:
					self._copy_slider(srcobj, srckey, dstobj)
		return {'FINISHED'}

	def invoke(self, context, event):
		self.li_object = ''
		if bpy.context.active_object:
			self.li_object = bpy.context.active_object.name
		return context.window_manager.invoke_props_dialog(self)

	def _copy_slider(self, srcobj, srckey, dstobj):
		if dstobj.type != 'MESH' or not dstobj.data.shape_keys:
			return
		dstkey = dstobj.data.shape_keys.key_blocks.get(srckey.name)
		if not dstkey:
			return
		dstkey.value = srckey.value
