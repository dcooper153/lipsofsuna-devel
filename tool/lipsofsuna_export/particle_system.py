import os
from .format import *
from .particle import *

class LIParticleSystem:

	def __init__(self, object, system):
		self.object = object
		self.system = system
		self.particles = []
		self.frame_start = 0
		self.frame_end = int(self.system.settings.frame_end)
		self.lifetime = int(self.system.settings.lifetime)
		self.particle_size = self.system.settings.particle_size
		# Set material properties.
		self.shader = "particle"
		self.texture = "particle1"
		mat = self.system.settings.material - 1
		if mat < len(self.object.material_slots):
			# Set material shader.
			mat = self.object.material_slots[mat]
			try:
				self.shader = mat.material["material"]
			except:
				pass
			# Set material texture.
			tex = mat.material.texture_slots[0]
			if tex and tex.texture and tex.texture.type == "IMAGE":
				img = tex.texture.image
				self.texture = os.path.splitext(os.path.basename(img.filepath))[0]
		# Create particles.
		for sys in self.system.particles:
			self.particles.append(LIParticle())

	def add_frame(self):
		i = 0
		for bpar in self.system.particles:
			lpar = self.particles[i]
			if bpar.alive_state == 'UNBORN':
				lpar.frame_start += 1
			elif bpar.alive_state == 'ALIVE':
				lpar.add_frame((bpar.location - self.object.location) * LIFormat.matrix)
			i += 1

	# \brief Saves the particle system.
	# \param self Particle system.
	# \param writer Writer.
	def write(self, writer):
		for i in range(len(self.particles) - 1, -1, -1):
			if self.particles[i].is_empty():
				del self.particles[i]
		writer.write_int(LIFormat.PARTICLEFLAG_NEWFORMAT)
		writer.write_int(self.frame_start)
		writer.write_int(self.frame_end)
		writer.write_int(self.frame_end + self.lifetime)
		writer.write_float(self.particle_size)
		writer.write_string(self.shader)
		writer.write_string(self.texture)
		writer.write_int(len(self.particles))
		writer.write_marker()
		for par in self.particles:
			par.write(writer)
