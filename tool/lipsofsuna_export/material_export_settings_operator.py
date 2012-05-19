import bpy

class LIMaterialExportSettingsOperator(bpy.types.Operator):
	bl_idname = "wm.lipsofsuna_material_export_settings"
	bl_label = "Material Exporting Settings"
	bl_description = 'Control how this material should be exporter and look in-game'

	li_file = bpy.props.StringProperty(name='File name', description='Target file name without path or extension')
	li_material = bpy.props.StringProperty(name='Ogre material', description='The name of the Ogre material to be used in-game')

	def draw(self, context):
		layout = self.layout
		box = layout.column()
		box.prop(self, 'li_file')
		box.prop(self, 'li_material')

	def execute(self, context):
		def setprop(obj, name, value):
			try:
				if value:
					obj[name] = value
				else:
					del obj[name]
			except:
				pass
		if not len(self.li_file):
			setprop(context.material, 'file', None)
		else:
			setprop(context.material, 'file', self.li_file)
		if not len(self.li_material):
			setprop(context.material, 'material', None)
		else:
			setprop(context.material, 'material', self.li_material)
		return {'FINISHED'}

	def invoke(self, context, event):
		def getprop(obj, name):
			try:
				v = str(obj[name])
				if len(v):
					return v
				return None
			except:
				return None
		target = getprop(context.material, 'file')
		if not target:
			self.li_file = ''
		else:
			self.li_file = target
		target = getprop(context.material, 'material')
		if not target:
			self.li_material = ''
		else:
			self.li_material = target
		return context.window_manager.invoke_props_dialog(self)
