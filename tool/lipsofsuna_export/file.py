import os
import bpy
from .collision import *
from .format import *
from .hierarchy import *
from .mesh import *
from .particles import *
from .utils import *
from .writer import *

class LIFile:

	def __init__(self, file):
		self.junkobjs = []
		self.tempobjs = []
		self.origobjs = {}
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
		# Find objects.
		if self.state == 0:
			for obj in bpy.data.objects:
				self.origobjs[obj] = True
			self.state += 1
			return False
		# Create collision shapes.
		elif self.state == 1:
			for obj in bpy.data.objects:
				if LIUtils.object_check_export(obj, self.filename, 'SHAPE'):
					self.coll.add_mesh(obj)
			self.state += 1
			return False
		# Select meshes.
		elif self.state == 2:
			for obj in bpy.data.objects:
				sel = False
				if LIUtils.object_check_export(obj, self.filename, 'RENDER'):
					sel = True
				obj.select = sel
			self.state += 1
			return False
		# Duplicate the meshes and select them.
		elif self.state == 3:
			bpy.ops.object.duplicate()
			for obj in bpy.data.objects:
				if obj.select:
					if obj in self.origobjs:
						# Unselect originals.
						obj.select = False
					else:
						# Select duplicates.
						obj.select = True
						self.tempobjs.append(obj)
			self.state += 1
			return False
		# Apply modifiers to the duplicated meshes.
		# This is currently setup so that multires and armature are the only
		# modifiers that can coexist with shape keys. If any other modifiers
		# exist, the shape key information is lost.
		elif self.state == 4:
			for obj in self.tempobjs:
				# Remove unwanted modifiers.
				num = 0
				for mod in obj.modifiers:
					if mod.type == 'ARMATURE' or mod.type == 'MULTIRES':
						obj.modifiers.remove(mod)
					elif mod.type == "MIRROR" and obj.data.shape_keys:
						LIUtils.apply_shape_key_mirror(obj, mod)
						self.junkobjs.append(obj)
						obj.select = False
						newobj = bpy.context.scene.objects.active
						newobj.select = True
						self.tempobjs[self.tempobjs.index(obj)] = newobj
					else:
						num += 1
				# Apply modifiers.
				if num:
					oldmesh = obj.data
					obj.data = obj.to_mesh(bpy.context.scene, True, 'PREVIEW')
					bpy.data.meshes.remove(oldmesh)
			self.state += 1
			return False
		# Simplify the duplicated meshes.
		elif self.state == 5:
			for obj in self.tempobjs:
				# Apply transformation.
				bpy.context.scene.objects.active = obj
				bpy.ops.object.location_clear()
				bpy.ops.object.transform_apply(scale=True)
				obj.matrix_local = LIFormat.matrix * obj.matrix_local
				bpy.ops.object.transform_apply(rotation=True)
				# Triangulate.
				if obj.mode != 'EDIT':
					bpy.ops.object.editmode_toggle()
				bpy.ops.mesh.select_all(action='SELECT')
				bpy.ops.mesh.quads_convert_to_tris()
				bpy.ops.object.editmode_toggle()
				# Tidy vertex weights.
				bpy.ops.object.vertex_group_clean(all_groups=True)
			self.state += 1
			return False
		# Join the duplicated meshes.
		elif self.state == 6:
			if len(self.tempobjs) > 1:
				bpy.ops.object.join()
			if len(self.tempobjs):
				self.object = bpy.context.scene.objects.active
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
			# Remove the armature and build a convex hull using shrinkwrap.
			if self.mesh:
				for mod in self.object.modifiers:
					self.object.modifiers.remove(mod)
				self.coll.add_default(self.object)
			self.state += 1
			return False
		# Delete the temporary mesh.
		elif self.state == 9:
			need = False
			if self.object != None:
				bpy.ops.object.delete()
				need = True
			for obj in self.junkobjs:
				obj.select = True
				need = True
			if need:
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
			# Animations.
			data.clear("ani")
			if self.hier.write_anims(data):
				self.write_block("ani", data)
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

	def write_block(self, name, data):
		self.file.write(name.encode())
		self.file.write(struct.pack('B', 0))
		self.file.write(struct.pack("!I", data.pos))
		self.file.write(data.data)
