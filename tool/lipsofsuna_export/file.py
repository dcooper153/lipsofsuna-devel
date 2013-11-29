import os, re
import bpy
from .collision import *
from .format import *
from .hierarchy import *
from .mesh import *
from .mesh_utils import *
from .modifier_edgesplit import *
from .modifier_mirror import *
from .particles import *
from .utils import *
from .writer import *

class LIFile:

	def __init__(self, file, objects):
		self.mesh_objects = objects
		self.file_objects = {}
		self.created_objects = {}
		self.coll = LICollision()
		self.hier = None
		self.mesh = None
		self.object = None
		self.state = 0
		# Determine the file name.
		# Try to save to ../graphics/ if it exists and fallback to ./ otherwise.
		self.filename = file
		path1 = os.path.split(bpy.data.filepath)[0]
		path2 = os.path.join(os.path.split(path1)[0], "graphics")
		if os.path.exists(path2):
			self.filepath = os.path.join(path2, file)
		else:
			self.filepath = os.path.join(path1, file)

	def get_finish_info(self):
		info = [0, 0, 0, 0, 0]
		if self.mesh:
			info[0] = len(self.mesh.vertlist)
			info[1] = len(self.mesh.matlist)
		if self.hier:
			info[2] = len(self.hier.animlist)
			info[3] = len(self.hier.nodelist)
		if self.mesh:
			info[4] = len(self.mesh.shapekeylist)
		return info

	def get_progress_info(self):
		if self.hier:
			info = self.hier.get_progress_info()
			if info:
				return "Exporting %s (%s)" % (self.filename, info)
		return "Exporting %s" % self.filename

	# \brief Iteratively collects the data belonging to the file.
	# \return True when done, False if more work remains.
	def process(self):
		# Find all the render meshes.
		if self.state == 0:
			for obj,orig in self.mesh_objects.items():
				if LIUtils.object_check_export(orig, self.filename, 'RENDER'):
					self.file_objects[obj] = True
			self.state += 1
			return False
		# Create collision shapes.
		elif self.state == 1:
			for obj in bpy.data.objects:
				if LIUtils.object_check_export(obj, self.filename, 'SHAPE'):
					self.coll.add_mesh(obj)
			self.state += 1
			return False
		# Create copies of the render meshes.
		elif self.state == 2:
			for obj in bpy.data.objects:
				if obj in self.file_objects:
					obj.select = True
				else:
					obj.select = False
			bpy.ops.object.duplicate()
			for obj in bpy.data.objects:
				if obj.select and obj not in self.file_objects:
					self.created_objects[obj] = True
			for obj in bpy.data.objects:
				obj.select = False
			self.state += 4
			return False
		# Join the render meshes.
		elif self.state == 6:
			if len(self.created_objects):
				for obj in self.created_objects:
					obj.select = True
					self.object = obj
				if len(self.created_objects) > 1:
					bpy.ops.object.join()
					self.object = bpy.context.scene.objects.active
				else:
					bpy.context.scene.objects.active = self.object
			self.state += 1
			return False
		# Build the mesh.
		elif self.state == 7:
			if self.object != None:
				try:
					self.mesh = LIMesh(self.object, self.filename)
				except Exception as e:
					bpy.ops.object.delete()
					raise e
			self.state += 1
			return False
		# Build the default collision shape.
		elif self.state == 8:
			if self.mesh:
				self.coll.add_default(self.object)
			self.state += 1
			return False
		# Delete the temporary mesh.
		elif self.state == 9:
			if self.object != None:
				for obj in bpy.data.objects:
					if obj == self.object:
						obj.select = True
					else:
						obj.select = False
				bpy.ops.object.delete()
			self.state += 1
			return False
		# Build the node hierarchy.
		elif self.state == 10:
			self.hier = LIHierarchy(self.filename)
			self.state += 1
			return False
		# Bake skeletal animations.
		elif self.state == 11:
			if self.hier == None or self.hier.process():
				self.state += 1
			return False
		# Build particle animations.
		elif self.state == 12:
			self.particles = LIParticles(self.filename)
			self.state += 1
			return False
		else:
			return True

	def write(self):
		if not self.mesh and not self.hier:
			return
		if LIFormat.debug:
			debug = open(self.filepath + ".dbg.txt", "w")
		else:
			debug = None
		self.file = open(self.filepath, "wb")
		# Header.
		data = LIWriter(debug)
		data.clear(LIFormat.MAGIC)
		data.write_int(LIFormat.VERSION)
		data.write_int(0)
		data.write_marker()
		self.write_block(LIFormat.MAGIC, data)
		if self.mesh:
			# Bounds.
			data.clear("bou")
			if self.mesh.write_bounds(data):
				self.write_block("bou", data)
			# Materials.
			data.clear("mat")
			if self.mesh.write_materials(data):
				self.write_block("mat", data)
			# Vertices.
			data.clear("ver")
			if self.mesh.write_vertices(data):
				self.write_block("ver", data)
			# Indices.
			data.clear("fac")
			if self.mesh.write_groups(data):
				self.write_block("fac", data)
			# Weights.
			data.clear("wei")
			if self.mesh.write_weights(data):
				self.write_block("wei", data)
			# Partitions.
			data.clear("ptt")
			if self.mesh.write_partitions(data):
				self.write_block("ptt", data)
			# Shape keys.
			data.clear("shk")
			if self.mesh.write_shape_keys(data):
				self.write_block("shk", data)
			# TODO: Hairs.
		if self.hier:
			# Nodes.
			data.clear("nod")
			if self.hier.write(data):
				self.write_block("nod", data)
		# Shapes.
		data.clear("sha")
		if self.coll.write(data):
			self.write_block("sha", data)
		# Particles.
		data.clear("par")
		if self.particles.write(data):
			self.write_block("par", data)
		# Done.
		self.file.close()
		if debug:
			debug.close()

	def write_anims(self):
		if not self.hier:
			return
		for anim in self.hier.animlist:
			# Create the animation path.
			name = re.sub("[^A-Za-z0-9]", "-", anim.name)
			suff = "-" + name + ".lani"
			path = suff.join(self.filepath.rsplit(".lmdl", 1))
			# Open the file.
			if LIFormat.debug:
				debug = open(path + ".dbg.txt", "w")
			else:
				debug = None
			self.file = open(path, "wb")
			# Header.
			data = LIWriter(debug)
			data.clear(LIFormat.MAGIC_ANIM)
			data.write_int(LIFormat.VERSION_ANIM)
			data.write_marker()
			self.write_block(LIFormat.MAGIC_ANIM, data)
			# Animation.
			data.clear("ani")
			anim.write(data)
			self.write_block("ani", data)
			# Done.
			self.file.close()
			if debug:
				debug.close()

	def write_block(self, name, data):
		self.file.write(name.encode())
		self.file.write(struct.pack('B', 0))
		self.file.write(struct.pack("!I", data.pos))
		self.file.write(data.data)
