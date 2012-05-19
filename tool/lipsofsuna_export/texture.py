import os
from .format import *

class LITexture:

	def __init__(self, index, tex, img):
		# Defaults.
		self.index = index
		self.string = ''
		self.flags = LIFormat.TEXFLAG_DEFAULT
		self.type = LIFormat.TEXTYPE_NONE
		# Parameters.
		if tex:
			self.flags = 0
			if tex.extension == 'REPEAT':
				self.flags |= LIFormat.TEXFLAG_CLAMP
			else:
				self.flags |= LIFormat.TEXFLAG_REPEAT
			if tex.use_interpolation:
				self.flags |= LIFormat.TEXFLAG_BILINEAR
			if tex.use_mipmap:
				self.flags |= LIFormat.TEXFLAG_MIPMAP
		# Filename.
		if img:
			self.type = LIFormat.TEXTYPE_IMAGE
			self.string = os.path.splitext(os.path.basename(img.filepath))[0]

	def write(self, writer):
		writer.write_int(self.type)
		writer.write_int(self.flags)
		writer.write_int(0)#512)
		writer.write_int(0)#512)
		writer.write_string(self.string)
		writer.write_marker()
