import struct

class LIWriter:

	def __init__(self, debug):
		self.data = bytearray()
		self.pos = 0
		self.debug = debug

	def clear(self, block):
		if self.debug:
			self.debug.write("\n=== %s ===\n" % block)
		self.data = bytearray()
		self.pos = 0

	# Appends an integer.
	def write_int(self, value):
		if self.debug:
			self.debug.write("%d " % value)
		self.data += struct.pack("!I", value)
		self.pos += 4

	# Appends a floating point number.
	def write_float(self, value):
		if self.debug:
			self.debug.write("%f " % value)
		self.data += struct.pack("!f", value)
		self.pos += 4

	# Appends a marker when in debug mode.
	def write_marker(self):
		if self.debug:
			self.debug.write("\n")

	# Appends a string.
	def write_string(self, value):
		if self.debug:
			self.debug.write("\"" + value + "\" ")
		self.data += value.encode()
		self.data += struct.pack('B', 0)
		self.pos += len(value) + 1
