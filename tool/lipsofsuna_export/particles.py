import bpy
from .particle_system import *
from .utils import *

class LIParticles:

	def __init__(self, file):
		# Create particle systems.
		self.file = file
		self.particlesystems = []
		obj = bpy.context.scene.objects.active
		for object in bpy.context.scene.objects:
			if LIUtils.object_check_export(object, self.file, 'PARTICLE'):
				for sys in object.particle_systems:
					self.particlesystems.append(LIParticleSystem(object, sys))
		# Calculate total animation length.
		self.frame_start = 0
		self.frame_end = 0
		self.frame_end_emit = 0
		for sys in self.particlesystems:
			if self.frame_end_emit < sys.frame_end:
				self.frame_end_emit = sys.frame_end
			if self.frame_end < sys.frame_end + sys.lifetime:
				self.frame_end = sys.frame_end + sys.lifetime
		# Animate particle systems.
		if len(self.particlesystems):
			for frame in range(self.frame_start, self.frame_end):
				# Switch frame.
				bpy.context.scene.frame_set(frame)
				# Add particle frame.
				for sys in self.particlesystems:
					sys.add_frame()

	# \brief Saves all particle systems.
	# \param self Particle manager.
	# \param writer Writer.
	def write(self, writer):
		if not len(self.particlesystems):
			return False
		writer.write_int(len(self.particlesystems))
		writer.write_marker()
		for sys in self.particlesystems:
			sys.write(writer)
		return True
