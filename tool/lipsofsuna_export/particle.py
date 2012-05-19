class LIParticle:

	def __init__(self):
		self.frame_start = 0
		self.frame_end = 0
		self.frames = []

	def add_frame(self, frame):
		self.frames.append(frame)
		self.frame_end += 1

	def is_empty(self):
		return len(self.frames) <= 1

	# \brief Saves the particle.
	# \param self Particle.
	# \param writer Writer.
	def write(self, writer):
		# FIXME: The first frame is broken for some reason.
		if len(self.frames):
			del self.frames[0]
		writer.write_int(self.frame_start)
		writer.write_int(len(self.frames))
		for f in self.frames:
			writer.write_float(f.x)
			writer.write_float(f.y)
			writer.write_float(f.z)
		writer.write_marker()
