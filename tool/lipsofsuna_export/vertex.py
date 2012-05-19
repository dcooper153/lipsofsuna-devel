class LIVertex:

	def __init__(self, index, co, no, uv, we):
		self.index = index
		self.co = co
		self.no = no
		self.uv = uv
		self.we = we

	def write(self, writer):
		writer.write_float(self.uv[0])
		writer.write_float(1.0 - self.uv[1])
		writer.write_float(self.no.x)
		writer.write_float(self.no.y)
		writer.write_float(self.no.z)
		writer.write_float(self.co.x)
		writer.write_float(self.co.y)
		writer.write_float(self.co.z)
		writer.write_marker()

	def write_weights(self, writer):
		writer.write_int(len(self.we))
		for weight in self.we:
			writer.write_int(weight[0])
			writer.write_float(weight[1])
		writer.write_marker()
