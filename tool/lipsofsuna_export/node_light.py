import bpy
from .format import *
from .node import *

class LINodeLight(LINode):

	def __init__(self, parent, object):
		LINode.__init__(self, parent, object)
		self.type = LIFormat.NODETYPE_LIGHT
		self.flags = LIFormat.LIGHTFLAG_NEWFORMAT
		if object.data.shadow_method != 'NOSHADOW':
			self.flags |= LIFormat.LIGHTFLAG_NOSHADOW
		if object.data.type == 'SPOT':
			self.clip_start = object.data.shadow_buffer_clip_start
			self.clip_end = object.data.shadow_buffer_clip_end
			self.spot_cutoff = object.data.spot_size
			self.spot_size = 0.5 * object.data.spot_size
			self.spot_blend = 32.0 * object.data.spot_blend
		else:
			self.clip_start = 0.1
			self.clip_end = 100
			self.spot_cutoff = math.pi
			self.spot_size = math.pi
			self.spot_blend = 0
		self.ambient = [0, 0, 0, 1]
		self.ambient[0] = bpy.context.scene.world.ambient_color[0]
		self.ambient[1] = bpy.context.scene.world.ambient_color[1]
		self.ambient[2] = bpy.context.scene.world.ambient_color[2]
		self.diffuse = [0, 0, 0, 1]
		self.diffuse[0] = object.data.energy * object.data.color[0]
		self.diffuse[1] = object.data.energy * object.data.color[1]
		self.diffuse[2] = object.data.energy * object.data.color[2]
		self.specular = [0, 0, 0, 1]
		self.specular[0] = self.diffuse[0]
		self.specular[1] = self.diffuse[1]
		self.specular[2] = self.diffuse[2]
		self.equation = [1, 1, 1]
		self.equation[1] = object.data.linear_attenuation / object.data.distance
		self.equation[2] = object.data.quadratic_attenuation / (object.data.distance * object.data.distance)

	def write_special(self, writer):
		writer.write_int(self.flags)
		writer.write_float(self.spot_cutoff)
		writer.write_float(self.clip_start)
		writer.write_float(self.clip_end)
		writer.write_float(self.ambient[0])
		writer.write_float(self.ambient[1])
		writer.write_float(self.ambient[2])
		writer.write_float(self.ambient[3])
		writer.write_float(self.diffuse[0])
		writer.write_float(self.diffuse[1])
		writer.write_float(self.diffuse[2])
		writer.write_float(self.diffuse[3])
		writer.write_float(self.specular[0])
		writer.write_float(self.specular[1])
		writer.write_float(self.specular[2])
		writer.write_float(self.specular[3])
		writer.write_float(self.equation[0])
		writer.write_float(self.equation[1])
		writer.write_float(self.equation[2])
		writer.write_float(self.spot_size)
		writer.write_float(self.spot_blend)
