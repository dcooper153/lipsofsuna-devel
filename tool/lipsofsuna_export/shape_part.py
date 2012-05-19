from .format import *

class LIShapePart:

	def __init__(self, obj, offset):
		self.vertices = []
		matrix = LIFormat.matrix1.copy().to_3x3() * obj.matrix_world.to_3x3()
		for v in obj.data.vertices:
			self.vertices.append(v.co * matrix + offset)

	def write(self, writer):
		writer.write_int(len(self.vertices))
		writer.write_marker()
		for v in self.vertices:
			writer.write_float(v.x)
			writer.write_float(v.y)
			writer.write_float(v.z)
			writer.write_marker()
