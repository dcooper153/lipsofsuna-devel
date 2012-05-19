from .shape_part import *

class LIShape:

	def __init__(self, name):
		self.name = name
		self.parts = []

	def add_mesh(self, obj, offset):
		self.parts.append(LIShapePart(obj, offset))

	def write(self, writer):
		writer.write_string(self.name)
		writer.write_int(len(self.parts))
		writer.write_marker()
		for p in self.parts:
			p.write(writer)
