import bpy

class LIAnimation:

	def __init__(self, hierarchy, armat, name, tracks):
		self.hierarchy = hierarchy
		self.armature = armat
		self.name = name
		self.state = None
		self.tracks = tracks
		self.framelist = []
		# Determine the duration of the animation.
		self.frame_start = None
		self.frame_end = None
		for track in tracks:
			for strip in track.strips:
				if self.frame_start == None or self.frame_start > strip.frame_start:
					self.frame_start = int(strip.frame_start)
				if self.frame_end == None or self.frame_end < strip.frame_end:
					self.frame_end = int(strip.frame_end)
		if self.frame_start == None:
			self.frame_start = 0
		if self.frame_end == None:
			self.frame_end = 0
		if self.frame_start < 0:
			self.frame_start = 0
		# Get the list of bones moved by FK.
		self.channeldict = {}
		self.channellist = []
		for track in tracks:
			for strip in track.strips:
				for fcurve in strip.action.fcurves:
					chan = fcurve.data_path
					if chan.find("pose.bones[\"") == 0:
						chan = chan.split("\"")[1]
						self.add_channel(chan)
		# Get the list of bones moved by constraints.
		for bone in armat.pose.bones:
			for cons in bone.constraints:
				if cons.type == 'IK' and cons.target and cons.subtarget in self.channeldict:
					# Part of the IK chain of an animated bone.
					if cons.chain_count > 0:
						next = bone
						for i in range(0, cons.chain_count):
							self.add_channel(next.name)
							next = next.parent
							if not next:
								break
					else:
						next = bone
						while next:
							self.add_channel(next.name)
							next = next.parent
				elif cons.type == 'COPY_ROTATION' and cons.target and cons.subtarget in self.channeldict:
					# Copy rotation from an animated bone.
					self.add_channel(bone.name)

	def add_channel(self, name):
		if name in self.armature.data.bones:
			if name not in self.channeldict:
				self.channeldict[name] = True
				self.channellist.append(name)

	def process(self):
		if self.state == None:
			# TODO: Make sure that there are no solo tracks.
			# Make sure that there is no active action.
			self.armature.animation_data.action = None
			# Disable blend in/out to avoid broken frames at start or end.
			for track in self.armature.animation_data.nla_tracks:
				for strip in track.strips:
					strip.use_auto_blend = False
					strip.blend_in = 0.0
					strip.blend_out = 0.0
			# Mute tracks that don't belong to the animation.
			for track in self.armature.animation_data.nla_tracks:
				if track.name != self.name:
					track.mute = True
				else:
					track.mute = False
			# Reset the armature to the rest pose.
			self.hierarchy.rest_pose()
			# Stabilize constraints.
			bpy.context.scene.frame_set(self.frame_start)
			bpy.context.scene.frame_set(self.frame_start)
			self.state = 0
			return False
		else:
			# Switch to the processed frame.
			frame = self.frame_start + self.state
			if frame >= self.frame_end:
				return True
			bpy.context.scene.frame_set(frame)
			bpy.context.scene.frame_set(frame)
			# Get channel transformations.
			xforms = []
			for chan in self.channellist:
				node = self.hierarchy.nodedict[chan]
				xforms.append(node.get_pose_transform())
			self.framelist.append(xforms)
			self.state += 1
			return False

	def write(self, writer):
		# Writer header.
		writer.write_string(self.name)
		writer.write_int(len(self.channellist))
		writer.write_int(self.frame_end - self.frame_start)
		writer.write_marker()
		# Write channel names.
		for chan in self.channellist:
			writer.write_string(chan)
		writer.write_marker()
		# Write channel transformations.
		for frame in range(0, self.frame_end - self.frame_start):
			for chan in range(len(self.channellist)):
				xform = self.framelist[frame][chan]
				writer.write_float(xform[0].x)
				writer.write_float(xform[0].y)
				writer.write_float(xform[0].z)
				writer.write_float(xform[1].x)
				writer.write_float(xform[1].y)
				writer.write_float(xform[1].z)
				writer.write_float(xform[1].w)
				writer.write_marker()
			writer.write_marker()
