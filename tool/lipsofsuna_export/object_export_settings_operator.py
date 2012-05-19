import bpy

class LIObjectExportSettingsOperator(bpy.types.Operator):
	bl_idname = "wm.lipsofsuna_export_settings"
	bl_label = "Exporting Settings"
	bl_description = 'Control what data is exported of this object'

	li_export = bpy.props.EnumProperty(name='Export mode', description='The type of data exported from the object', items=[\
		('particle', 'Particle animations', ''),\
		('shape', 'Collision shape', ''),\
		('render', 'Graphics', ''),\
		('none', 'Disabled', '')],
		default='none')
	li_file = bpy.props.StringProperty(name='File name', description='Target file name without path or extension')
	li_shape = bpy.props.StringProperty(name='Shape name', description='The name of the exported collision shape')

	def draw(self, context):
		layout = self.layout
		box = layout.column()
		box.prop(self, 'li_export')
		box.prop(self, 'li_file')
		box.prop(self, 'li_shape')

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
			try:
				path,name = os.path.split(bpy.data.filepath)
				self.li_file = os.path.splitext(name)[0]
			except:
				self.li_file = 'unnamed'
		if self.li_export == 'render':
			setprop(context.object, 'file', self.li_file)
			setprop(context.object, 'render', None)
			setprop(context.object, 'shape', None)
			setprop(context.object, 'export', None)
		elif self.li_export == 'shape':
			setprop(context.object, 'file', self.li_file)
			setprop(context.object, 'render', 'false')
			setprop(context.object, 'shape', self.li_shape or 'default')
			setprop(context.object, 'export', None)
		elif self.li_export == 'particle':
			setprop(context.object, 'file', self.li_file)
			setprop(context.object, 'render', 'false')
			setprop(context.object, 'shape', None)
			setprop(context.object, 'export', None)
		elif self.li_export == 'none':
			setprop(context.object, 'export', 'false')
		else:
			setprop(context.object, 'file', None)
			setprop(context.object, 'render', None)
			setprop(context.object, 'shape', None)
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
		target = getprop(context.object, 'file')
		if not target:
			self.li_export = 'none'
			self.li_file = ''
			self.li_shape = ''
		elif getprop(context.object, 'render') == 'false':
			shape = getprop(context.object, 'shape')
			if shape:
				self.li_export = 'shape'
				self.li_file = target
				self.li_shape = shape
			else:
				self.li_export = 'particle'
				self.li_file = target
				self.li_shape = ''
		else:
			self.li_export = 'render'
			self.li_file = target
			self.li_shape = ''
		return context.window_manager.invoke_props_dialog(self)
