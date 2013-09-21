import bpy
from .animation import *
from .node_root import *

class LIHierarchy:

	def __init__(self, file):
		self.file = file
		self.state = 0
		# Reset armatures to rest pose.
		self.rest_pose()
		# Build the node hierarchy.
		self.node = LINodeRoot(self)
		self.nodedict = {}
		self.nodelist = []
		self.add_node(self.node)
		self.node.add_children()
		if len(self.nodelist) == 1:
			self.nodedict = {}
			self.nodelist = []
		# Check for armatures.
		self.armature = None
		for node in self.nodelist:
			if node.object and node.object.type == 'ARMATURE':
				if LIUtils.object_check_export(node.object, self.file, 'NODE'):
					self.armature = node.object
					break
		# Build animations.
		self.trackdict = {}
		self.animlist = []
		if self.armature:
			# Get the list of track names. We group tracks with matching names
			# together so that they can be compiled into a single animation.
			for track in self.armature.animation_data.nla_tracks:
				if track.name not in self.trackdict:
					self.trackdict[track.name] = [track]
				else:
					self.trackdict[track.name].append(track)
			# Build animations from tracks. We create a new animation for each
			# unique track name present in the NLA editor.
			for name in self.trackdict:
				self.animlist.append(LIAnimation(self, self.armature, name, self.trackdict[name]))

	def add_node(self, node):
		self.nodedict[node.name] = node
		self.nodelist.append(node)

	def get_progress_info(self):
		if self.state >= len(self.animlist):
			return None
		anim = self.animlist[self.state]
		return anim.name

	def process(self):
		if self.state >= len(self.animlist):
			return True
		anim = self.animlist[self.state]
		if anim.process():
			self.state += 1
		return False

	# \brief Resets all armatures in the scene to their rest positions.
	def rest_pose(self):
		active = bpy.context.scene.objects.active
		for obj in bpy.data.objects:
			if not LIUtils.object_check_export(obj, self.file, 'NODE'):
				continue
			if obj.type == 'ARMATURE':
				bpy.context.scene.objects.active = obj
				bpy.ops.object.mode_set(mode='POSE', toggle=False)
				bpy.ops.pose.select_all(action='SELECT')
				bpy.ops.pose.loc_clear()
				bpy.ops.pose.rot_clear()
				bpy.ops.pose.scale_clear()
				bpy.ops.object.mode_set(mode='OBJECT', toggle=False)
		bpy.context.scene.objects.active = active

	def write(self, writer):
		if not len(self.node.nodes):
			return False
		writer.write_int(1)
		self.node.write(writer)
		return True

	def write_anims(self, writer):
		if not len(self.animlist):
			return False
		writer.write_int(len(self.animlist))
		for anim in self.animlist:
			anim.write(writer)
		return True
