import bpy, mathutils

class LIUpdateShapeKeys:

	@classmethod
	def run(cls, srcobj, dstobj):

		# Disable undo.
		bpy.context.user_preferences.edit.use_global_undo = False
		toggled = False
		if dstobj.mode == "EDIT":
			toggled = True
			bpy.ops.object.editmode_toggle()
		# Map the basis vertices.
		srcvtx = {(v.co.x, v.co.y, v.co.z): k for k,v in enumerate(srcobj.data.vertices)}
		mapping = [srcvtx.get((v.co.x, v.co.y, v.co.z)) for v in dstobj.data.vertices]
		# Update the shape key vertices.
		num_changed = 0
		for srckey in srcobj.data.shape_keys.key_blocks:
			if srckey.name == 'Basis':
				continue
			if not dstobj.data.shape_keys or srckey.name not in dstobj.data.shape_keys.key_blocks:
				dstobj.shape_key_add(name=srckey.name, from_mix=False)
			dstkey = dstobj.data.shape_keys.key_blocks[srckey.name]
			if cls.transfer_key(srcobj, srckey, dstobj, dstkey, mapping):
				num_changed += 1
		# Restore the state.
		if toggled:
			bpy.ops.object.editmode_toggle()
		bpy.context.user_preferences.edit.use_global_undo = True
		return num_changed

	@classmethod
	def transfer_key(cls, srcobj, srckey, dstobj, dstkey, mapping):

		changed = False
		for k,v in enumerate(mapping):
			if v != None:
				if dstkey.data[k].co != srckey.data[v].co:
					dstkey.data[k].co = srckey.data[v].co
					changed = True
		return changed

	@classmethod
	def validate(cls, srcobj, dstobj):

		# Make sure that the objects exist.
		if not srcobj:
			return "No source object was found."
		if srcobj == dstobj:
			return "The source and the destination objects are the same object."
		# Make sure that the source object has shape keys.
		if not srcobj.data.shape_keys:
			return "The source object has no shape keys."
