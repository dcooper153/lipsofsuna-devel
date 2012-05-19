import bpy, mathutils
from .utils_geometry import *

class LICopyWeightGroups:

	@classmethod
	def get_weight(cls, group, index):
		try:
			return group.weight(index)
		except:
			return 0.0

	@classmethod
	def is_weight_group_empty(cls, object, group):
		for i in range(len(object.data.vertices)):
			if cls.get_weight(group, i) > 0.01:
				return False
		return True

	@classmethod
	def transfer_group(cls, srcobj, srcgrp, dstobj, dstgrp, closest):
		for i in range(len(closest)):
			cls.transfer_vertex(srcobj, srcgrp, dstobj, dstgrp, closest, i)

	@classmethod
	def transfer_vertex(cls, srcobj, srcgrp, dstobj, dstgrp, closest, index):
		mapping = closest[index]
		if mapping['type'] == 'vertex':
			weight = cls.get_weight(srcgrp, mapping['vertex'])
		else:
			face = srcobj.data.polygons[mapping['face']]
			coords = [mathutils.Vector(srcobj.data.vertices[v].co) for v in face.vertices]
			weights = [cls.get_weight(srcgrp, v) for v in face.vertices]
			weight = LIUtilsGeometry.interpolate_face(coords, mapping['point'], weights)
		if weight > 0.01:
			dstgrp.add([index], weight, 'REPLACE')

	@classmethod
	def run(cls, srcobj, dstobj):
		# Disable undo.
		bpy.context.user_preferences.edit.use_global_undo = False
		toggled = False
		if dstobj.mode == "EDIT":
			toggled = True
			bpy.ops.object.editmode_toggle()
		# Copy weight groups.
		dict_added = {}
		count_added = 0
		closest = LIUtilsGeometry.find_closest_points(srcobj, dstobj)
		if closest:
			for name,srcgrp in srcobj.vertex_groups.items():
				if not dstobj.vertex_groups or name not in dstobj.vertex_groups:
					dict_added[name] = True
					count_added += 1
					dstobj.vertex_groups.new(name=name)
					dstgrp = dstobj.vertex_groups[name]
					cls.transfer_group(srcobj, srcgrp, dstobj, dstgrp, closest)
		# Remove useless weight groups.
		if count_added:
			i = 0
			for name,group in dstobj.vertex_groups.items():
				if name in dict_added:
					if cls.is_weight_group_empty(dstobj, group):
						dstobj.vertex_groups.remove(group)
						count_added -= 1
		# Restore state.
		if toggled:
			bpy.ops.object.editmode_toggle()
		bpy.context.user_preferences.edit.use_global_undo = True
		return count_added

	@classmethod
	def validate(cls, srcobj, dstobj):
		# Make sure that the objects exist.
		if not srcobj:
			return "No source object was found."
		if srcobj == dstobj:
			return "The source and the destination objects are the same object."
		# Make sure that the objects are aligned.
		if (srcobj.location - dstobj.location).length > 0.01:
			return "The source and destination objects are not in the same position."
		# Make sure the source object doesn't have nasty modifiers.
		# FIXME: Disabling them would be nicer but Blender doesn't make it easy.
		for index,mod in enumerate(srcobj.modifiers):
			if mod.type == 'MIRROR':
				if mod.use_x or mod.use_y or mod.use_z:
					return "You must disable mirror modifiers from the source object."
