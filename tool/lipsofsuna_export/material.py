from .format import *
from .texture import *

class LIMaterial:

	def __init__(self, index, mat, img):
		self.index = index
		self.indices = []
		self.diffuse = [1, 1, 1, 1]
		self.emission = 0
		self.flags = LIFormat.MATRFLAG_DEFAULT
		self.name = 'default'
		self.shader = 'diff1'
		self.material = 'diff1'
		self.shininess = 0
		self.specular = [1, 1, 1, 1]
		self.strands = [0.2, 0.0, 0.0]
		self.textures = []
		# Material properties.
		if mat:
			self.name = mat.name
			try:
				self.shader = mat["shader"]
			except:
				pass
			try:
				self.material = mat["material"]
			except:
				pass
			try:
				if mat["cull"] == "false":
					self.flags &= ~LIFormat.MATRFLAG_CULLFACE
			except:
				pass
			try:
				if mat["collision"] == "false":
					self.flags &= ~LIFormat.MATRFLAG_COLLISION
			except:
				pass
			if mat.use_transparency:
				self.flags |= LIFormat.MATRFLAG_ALPHA
			self.diffuse[0] = mat.diffuse_color[0]
			self.diffuse[1] = mat.diffuse_color[1]
			self.diffuse[2] = mat.diffuse_color[2]
			self.diffuse[3] = mat.alpha
			self.emission = mat.emit
			self.shininess = mat.specular_hardness / 511.0 * 128.0
			self.specular[0] = mat.specular_color[0]
			self.specular[1] = mat.specular_color[1]
			self.specular[2] = mat.specular_color[2]
			self.specular[3] = mat.specular_intensity
			self.strands[0] = mat.strand.root_size
			self.strands[1] = mat.strand.tip_size
			self.strands[2] = mat.strand.shape
		# Material textures.
		if mat:
			index = 0
			for slot in mat.texture_slots:
				if slot and slot.texture and slot.texture.type == 'IMAGE':
					tex = slot.texture
					while len(self.textures) < index:
						self.textures.append(LITexture(len(self.textures), None, None))
					self.textures.append(LITexture(index, tex, tex.image))
				index = index + 1
		# Face texture.
		if img and ((not mat) or (mat and mat.use_face_texture)):
			if not len(self.textures):
				self.textures.append(0)
			self.textures[0] = LITexture(0, None, img)

	def write_indices(self, writer):
		writer.write_int(len(self.indices))
		for i in self.indices:
			writer.write_int(i)
		writer.write_marker()

	def write_info(self, writer):
		writer.write_int(self.flags)
		writer.write_float(self.emission)
		writer.write_float(self.shininess)
		writer.write_float(self.diffuse[0])
		writer.write_float(self.diffuse[1])
		writer.write_float(self.diffuse[2])
		writer.write_float(self.diffuse[3])
		writer.write_float(self.specular[0])
		writer.write_float(self.specular[1])
		writer.write_float(self.specular[2])
		writer.write_float(self.specular[3])
		writer.write_float(self.strands[0])
		writer.write_float(self.strands[1])
		writer.write_float(self.strands[2])
		writer.write_int(len(self.textures))
		writer.write_string(self.shader)
		writer.write_string(self.material)
		writer.write_marker()
		for tex in self.textures:
			tex.write(writer)
