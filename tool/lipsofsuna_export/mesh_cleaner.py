import os, re
import bpy
from .format import *
from .mesh_utils import *
from .utils import *

class LIMeshCleaner:

	def __init__(self):

		self.orig_objects = {}
		self.created_objects = {}
		self.state = 0

	def delete_created_objects(self):

		for obj in bpy.data.objects:
			if obj in self.created_objects:
				obj.select = True
			else:
				obj.select = False
		bpy.ops.object.delete()

	def process_all(self):
		
		while not self.process():
			pass

	def process(self):
		"""Collects and cleanups of the meshes.
		
		Returns:
			True when done, False if more work remains.
		"""

		# Find all the render meshes.
		if self.state == 0:
			for obj in bpy.data.objects:
				if LIUtils.object_check_export(obj, None, 'RENDER'):
					self.orig_objects[obj] = True
				obj.select = False
			self.state += 1
			return False
		# Copy the render meshes.
		elif self.state == 1:
			for obj in self.orig_objects:
				self.orig_objects[obj] = True
				created = LIMeshUtils.copy_object(obj)
				self.created_objects[created] = obj
			self.state += 1
			return False
		# Apply the modifiers of the render meshes.
		elif self.state == 2:
			for created,original in self.created_objects.items():
				LIMeshUtils.apply_modifiers(created, original.modifiers)
				created.select = False
			self.state += 1
			return False
		# Transfer the shape keys of the render meshes.
		elif self.state == 3:
			for created,original in self.created_objects.items():
				if not original.data.shape_keys:
					continue
				for i,key in enumerate(original.data.shape_keys.key_blocks):
					# Skip the basis shape.
					if not i:
						continue
					# Create a mesh from the shape key.
					tmp = LIMeshUtils.copy_object_with_shape_keys(original)
					LIMeshUtils.apply_shape_key(tmp, i)
					# Apply the modifiers to the mesh.
					LIMeshUtils.apply_modifiers(tmp, original.modifiers)
					# Add a shape key to the temporary mesh.
					bpy.context.scene.objects.active = tmp
					bpy.ops.object.shape_key_add()
					# Transfer the shape key to the render mesh.
					if len(tmp.data.vertices) != len(created.data.vertices):
						print("""WARNING: Shape key \"%s\" cannot be exported because the vertex count was
         different to that of the basis after applying modifiers. The shape key
         had %d vertices while the basis shape had %d. Check that the mirror
         modifier merges the vertices identically in both of the shape keys.""" %
							(key.name, len(tmp.data.vertices), len(created.data.vertices)))
					else:
						tmp.select = True
						bpy.context.scene.objects.active = created
						original.data.shape_keys.key_blocks
						bpy.ops.object.join_shapes()
						shapes = created.data.shape_keys.key_blocks
						shapes[len(shapes) - 1].name = key.name
					# Delete the temporary mesh.
					bpy.context.scene.objects.active = tmp
					bpy.ops.object.delete()
				created.select = False
			self.state += 1
			return False
		# Simplify the render meshes.
		elif self.state == 4:
			for obj in self.created_objects:
				# Apply transformation.
				bpy.context.scene.objects.active = obj
				obj.select = True
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
				if obj.vertex_groups:
					try:
						bpy.ops.object.vertex_group_clean(group_select_mode='ALL')
					except:
						try:
							bpy.ops.object.vertex_group_clean(all_groups=True)
						except Exception as e:
							print("WARNING: Tidying vertex groups failed: %s" % str(e))
				obj.select = False
			self.state += 1
			return False
		return True
