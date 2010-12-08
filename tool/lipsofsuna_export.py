import array, math, os, struct, sys, bpy, mathutils

class LIEnum:
	def __init__(self):
		self.debug = False
		self.files = []
		self.MAGIC = 'lips/mdl'
		self.VERSION = 0xFFFFFFF3
		self.LIGHTFLAG_NOSHADOW = 1
		self.MATRFLAG_BILLBOARD = 1
		self.MATRFLAG_COLLISION = 2
		self.MATRFLAG_CULLFACE = 4
		self.MATRFLAG_ALPHA = 8
		self.MATRFLAG_DEFAULT = self.MATRFLAG_COLLISION | self.MATRFLAG_CULLFACE
		self.NODETYPE_BONE = 0
		self.NODETYPE_EMPTY = 1
		self.NODETYPE_LIGHT = 2
		self.TEXFLAG_BILINEAR = 1
		self.TEXFLAG_CLAMP = 2
		self.TEXFLAG_MIPMAP = 4
		self.TEXFLAG_REPEAT = 8
		self.TEXFLAG_DEFAULT = self.TEXFLAG_BILINEAR | self.TEXFLAG_MIPMAP | self.TEXFLAG_REPEAT
		self.TEXTYPE_NONE = 0
		self.TEXTYPE_ENVMAP = 1
		self.TEXTYPE_IMAGE = 2
		self.euler = mathutils.Euler((-0.5 * math.pi, 0, 0))
		self.matrix = self.euler.to_matrix().to_4x4()
		self.quat = self.euler.to_quat()
LIFormat = LIEnum()

##############################################################################

def object_files(object):
	if object.library:
		return []
	try:
		prop = object['export']
		if prop == "false":
			return []
	except:
		pass
	try:
		lod = object['lod']
	except:
		lod = 'false'
	try:
		path = os.path.split(bpy.data.filepath)[0]
		path = os.path.join(os.path.split(path)[0], "graphics")
		prop = object['file'].split(',')
		result = []
		for item in prop:
			if item != '':
				if lod == 'true':
					result.append(os.path.join(path, item + 'l.lmdl'))
				else:
					result.append(os.path.join(path, item + '.lmdl'))
		return result
	except:
		return LIFormat.files

##############################################################################

class LIAnimation:

	def __init__(self, hierarchy, armat, name, tracks):
		self.hierarchy = hierarchy
		self.armature = armat
		self.name = name
		self.tracks = tracks
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
							next = bone.parent
							if not next:
								break
					else:
						next = bone
						while next:
							self.add_channel(next.name)
							next = bone.parent
				elif cons.type == 'COPY_ROTATION' and cons.target and cons.subtarget in self.channeldict:
					# Copy rotation from an animated bone.
					self.add_channel(bone.name)

	def add_channel(self, name):
		if name in self.armature.data.bones:
			if name not in self.channeldict:
				self.channeldict[name] = True
				self.channellist.append(name)

	def write(self, writer):
		# TODO: Make sure that there are no solo tracks.
		# Make sure that there is no active action.
		self.armature.animation_data.action = None
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
		# Evaluate channels for each frame.
		framelist = []
		for frame in range(self.frame_start, self.frame_end):
			# Switch frame.
			bpy.context.scene.frame_set(frame)
			# Get channel transformations.
			xforms = []
			for chan in self.channellist:
				node = self.hierarchy.nodedict[chan]
				xforms.append(node.get_pose_transform())
			framelist.append(xforms)
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
		for frame in range(self.frame_start, self.frame_end):
			for chan in range(len(self.channellist)):
				xform = framelist[frame][chan]
				writer.write_float(xform[0].x)
				writer.write_float(xform[0].y)
				writer.write_float(xform[0].z)
				writer.write_float(xform[1].x)
				writer.write_float(xform[1].y)
				writer.write_float(xform[1].z)
				writer.write_float(xform[1].w)
				writer.write_marker()
			writer.write_marker()

class LICollision:

	# \brief Initializes a new collision shape manager.
	# \param self Shape manager.
	def __init__(self):
		self.shapes = {}

	def add_mesh(self, object):
		try:
			prop = object['shape']
			if prop not in self.shapes:
				self.shapes[prop] = LIShape(prop)
			self.shapes[prop].add_mesh(object)
		except:
			pass

	# \brief Saves all shapes.
	# \param self Shape manager.
	# \param writer Writer.
	def write(self, writer):
		if not len(self.shapes):
			return False
		writer.write_int(len(self.shapes))
		writer.write_marker()
		for name in self.shapes:
			self.shapes[name].write(writer)
		return True

class LIHierarchy:

	def __init__(self, file):
		self.file = file
		# Reset armatures to rest pose.
		self.rest_pose()
		# Build the node hierarchy.
		self.node = LINodeRoot(self)
		self.nodedict = {}
		self.nodelist = []
		self.add_node(self.node)
		self.node.add_children()
		# Check for armatures.
		self.armature = None
		for node in self.nodelist:
			if node.object and node.object.type == 'ARMATURE':
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

	# \brief Resets all armatures in the scene to their rest positions.
	def rest_pose(self):
		active = bpy.context.scene.objects.active
		for obj in bpy.data.objects:
			if obj.type == 'ARMATURE' and not obj.library:
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

class LIMaterial:

	def __init__(self, index, mat, img):
		self.index = index
		self.indices = []
		self.diffuse = [1, 1, 1, 1]
		self.emission = 0
		self.flags = LIFormat.MATRFLAG_DEFAULT
		self.name = 'default'
		self.shader = 'default'
		self.shininess = 0
		self.specular = [1, 1, 1, 1]
		self.strands = [0.2, 0.0, 0.0]
		self.textures = []
		# Material properties.
		if mat:
			self.name = mat.name
			try:
				self.shader = mat["shader"]
			except:
				pass
			try:
				if mat["cull"] == "false":
					self.flags &= ~LIFormat.MATRFLAG_CULLFACE
			except:
				pass
			try:
				if mat["collision"] == "false":
					self.flags &= ~LIFormat.MATRFLAG_COLLISION
			except:
				pass
			if mat.use_transparency:
				self.flags |= LIFormat.MATRFLAG_ALPHA
			self.diffuse[0] = mat.diffuse_color[0]
			self.diffuse[1] = mat.diffuse_color[1]
			self.diffuse[2] = mat.diffuse_color[2]
			self.diffuse[3] = mat.alpha
			self.emission = mat.emit
			self.shininess = mat.specular_hardness / 511.0 * 128.0
			self.specular[0] = mat.specular_color[0]
			self.specular[1] = mat.specular_color[1]
			self.specular[2] = mat.specular_color[2]
			self.specular[3] = mat.specular_intensity
			self.strands[0] = mat.strand.root_size
			self.strands[1] = mat.strand.tip_size
			self.strands[2] = mat.strand.shape
		# Material textures.
		if mat:
			index = 0
			for slot in mat.texture_slots:
				if slot and slot.texture and slot.texture.type == 'IMAGE':
					tex = slot.texture
					while len(self.textures) < index:
						self.textures.append(LITexture(len(self.textures), None, None))
					self.textures.append(LITexture(index, tex, tex.image))
				index = index + 1
		# Face texture.
		if img and ((not mat) or (mat and mat.use_face_texture)):
			if not len(self.textures):
				self.textures.append(0)
			self.textures[0] = LITexture(0, None, img)

	def write_indices(self, writer):
		writer.write_int(len(self.indices))
		for i in self.indices:
			writer.write_int(i)
		writer.write_marker()

	def write_info(self, writer):
		writer.write_int(self.flags)
		writer.write_float(self.emission)
		writer.write_float(self.shininess)
		writer.write_float(self.diffuse[0])
		writer.write_float(self.diffuse[1])
		writer.write_float(self.diffuse[2])
		writer.write_float(self.diffuse[3])
		writer.write_float(self.specular[0])
		writer.write_float(self.specular[1])
		writer.write_float(self.specular[2])
		writer.write_float(self.specular[3])
		writer.write_float(self.strands[0])
		writer.write_float(self.strands[1])
		writer.write_float(self.strands[2])
		writer.write_int(len(self.textures))
		writer.write_string(self.shader)
		writer.write_marker()
		for tex in self.textures:
			tex.write(writer)

class LIMesh:

	def __init__(self, obj):
		mesh = obj.data
		# Initialize storage.
		self.matdict = {}
		self.matlist = []
		self.vertdict = {}
		self.vertlist = []
		self.weightgroupdict = {}
		self.weightgrouplist = []
		# Emit faces.
		for face in mesh.faces:
			# Vertices.
			verts = [mesh.vertices[x] for x in face.vertices]
			indices = [x for x in face.vertices]
			# Material attributes.
			idx = face.material_index
			bmat = None
			bimg = None
			if idx < len(obj.material_slots):
				bmat = obj.material_slots[idx].material
			# Texture attributes.
			if mesh.uv_textures.active:
				buvs = mesh.uv_textures.active.data[face.index]
				bimg = buvs.image
				uvs = buvs.uv
			else:
				uvs = ((0, 0), (0, 0), (0, 0), (0, 0))
			# Emit material.
			key = (bmat and idx or -1, bimg and bimg.name or '')
			if key not in self.matdict:
				mat = LIMaterial(len(self.matlist), bmat, bimg)
				self.matdict[key] = mat
				self.matlist.append(mat)
			else:
				mat = self.matdict[key]
			# Emit face.
			for i in range(0, len(verts)):
				# Vertex attributes.
				bvert = verts[i]
				no = face.use_smooth and bvert.normal or face.normal
				uv = uvs[i]
				# Emit vertex.
				key = (indices[i], no.x, no.y, no.z, uv[0], uv[1])
				if key not in self.vertdict:
					# Add weights.
					weights = []
					for weight in bvert.groups:
						group = obj.vertex_groups[weight.group]
						if group.name not in self.weightgroupdict:
							grpidx = len(self.weightgrouplist)
							self.weightgroupdict[group.name] = grpidx
							self.weightgrouplist.append(group.name)
						else:
							grpidx = self.weightgroupdict[group.name]
						weights.append((grpidx, weight.weight))
					# Add vertex.
					vert = LIVertex(len(self.vertlist), bvert.co, no, uv, weights)
					self.vertdict[key] = vert
					self.vertlist.append(vert)
				else:
					vert = self.vertdict[key]
				# Emit index.
				mat.indices.append(vert.index)

	def write_bounds(self, writer):
		if not len(self.vertlist):
			return False
		min = self.vertlist[0].co.copy()
		max = self.vertlist[0].co.copy()
		for vert in self.vertlist:
			if min.x > vert.co.x: min.x = vert.co.x
			if min.y > vert.co.y: min.y = vert.co.y
			if min.z > vert.co.z: min.z = vert.co.z
			if max.x < vert.co.x: max.x = vert.co.x
			if max.y < vert.co.y: max.y = vert.co.y
			if max.z < vert.co.z: max.z = vert.co.z
		writer.write_float(min.x)
		writer.write_float(min.y)
		writer.write_float(min.z)
		writer.write_float(max.x)
		writer.write_float(max.y)
		writer.write_float(max.z)
		writer.write_marker()
		return True

	def write_groups(self, writer):
		if not len(self.matlist):
			return False
		writer.write_int(len(self.matlist))
		writer.write_marker()
		index = 0
		for mat in self.matlist:
			writer.write_int(index)
			mat.write_indices(writer)
			index += 1
		return True

	def write_materials(self, writer):
		if not len(self.matlist):
			return False
		writer.write_int(len(self.matlist))
		writer.write_marker()
		for mat in self.matlist:
			mat.write_info(writer)
		return True

	def write_vertices(self, writer):
		if not len(self.vertlist):
			return False
		writer.write_int(len(self.vertlist))
		writer.write_marker()
		for vert in self.vertlist:
			vert.write(writer)
		return True

	def write_weights(self, writer):
		if not len(self.weightgrouplist):
			return False
		writer.write_int(len(self.weightgrouplist))
		for group in self.weightgrouplist:
			writer.write_string(group)
			writer.write_string(group)
		writer.write_marker()
		writer.write_int(len(self.vertlist))
		writer.write_marker()
		for vertex in self.vertlist:
			vertex.write_weights(writer)
		return True

class LINode:

	# \brief Creates a new generic object node.
	# \param object Blender object.
	# \param parent Parent node.
	def __init__(self, parent, object):
		self.type = LIFormat.NODETYPE_EMPTY
		self.nodes = []
		self.object = object
		self.parent = parent
		self.hierarchy = self.parent.hierarchy
		self.name = '#' + object.name
		self.loc,self.rot = self.get_rest_transform()

	def add_object(self, object):
		if self.hierarchy.file in object_files(object):
			if object.type == 'ARMATURE':
				self.add_node(LINodeArmature(self, object))
			if object.type == 'EMPTY':
				self.add_node(LINode(self, object))
			if object.type == 'LAMP' and (object.data.type == 'SPOT' or object.data.type == 'POINT'):
				self.add_node(LINodeLight(self, object))

	def add_node(self, node):
		self.nodes.append(node)
		self.hierarchy.add_node(node)

	# \brief Recursively adds children to the node.
	def add_children(self):
		# Add child objects.
		for obj in bpy.data.objects:
			if self.object == obj.parent and not obj.parent_bone:
				self.add_object(obj)
		# Recurse to children.
		for node in self.nodes:
			node.add_children()

	# \brief Gets the parent relative rest transformation of the node.
	def get_rest_transform(self):
		matrix = self.object.matrix_local.copy()
		if self.object.parent_bone:
			parentbone = self.object.parent.pose.bones[self.object.parent_bone]
			parentmatr = parentbone.matrix * mathutils.Matrix.Translation(self.parent.len)
			matrix = parentmatr.invert() * matrix
		loc = matrix.translation_part()
		rot = matrix.rotation_part().to_quat().normalize()
		return (loc, rot)

	# \brief Writes the node to a stream.
	# \param self Node.
	# \param writer Writer.
	def write(self, writer):
		writer.write_int(self.type)
		writer.write_int(len(self.nodes))
		writer.write_string(self.name)
		writer.write_float(self.loc.x)
		writer.write_float(self.loc.y)
		writer.write_float(self.loc.z)
		writer.write_float(self.rot.x)
		writer.write_float(self.rot.y)
		writer.write_float(self.rot.z)
		writer.write_float(self.rot.w)
		self.write_special(writer)
		writer.write_marker()
		for node in self.nodes:
			node.write(writer)

	def write_special(self, writer):
		pass

class LINodeArmature(LINode):

	def __init__(self, parent, object):
		LINode.__init__(self, parent, object)
		self.name = object.name

	def add_children(self):
		# Add root bones.
		for bone in self.object.data.bones.values():
			if not bone.parent:
				self.add_node(LINodeBone(self, bone))
		# Add child objects.
		LINode.add_children(self)

class LINodeBone(LINode):

	def __init__(self, parent, bone):
		self.type = LIFormat.NODETYPE_BONE
		self.nodes = []
		self.object = parent.object
		self.parent = parent
		self.hierarchy = self.parent.hierarchy
		self.bone = bone
		self.pose_bone = self.object.pose.bones[bone.name]
		self.name = bone.name
		self.len = mathutils.Vector((0, bone.length, 0))
		self.head = bone.head.copy()
		self.matrix = bone.matrix.copy()
		self.loc,self.rot = self.get_rest_transform()

	def add_children(self):
		# Add child bones.
		for bone in self.object.data.bones.values():
			if bone.parent == self.bone:
				self.add_node(LINodeBone(self, bone))
		# Add child objects.
		for obj in bpy.data.objects:
			if self.object == obj.parent and self.bone.name == obj.parent_bone:
				self.add_object(obj)
		# Recurse to children.
		for node in self.nodes:
			node.add_children()

	# \brief Gets the parent relative rest transformation of the node.
	def get_rest_transform(self):
		matrix = self.get_head_rest_matrix()
		if self.bone.parent:
			matrix = self.parent.get_tail_rest_matrix().invert() * matrix
		loc = matrix.translation_part()
		rot = matrix.rotation_part().to_quat().normalize()
		return (loc, rot)

	# \brief Gets the parent relative pose transformation of the node.
	def get_pose_transform(self):
		rest = self.get_head_rest_matrix()
		pose = self.get_head_pose_matrix()
		if self.bone.parent:
			rest = self.parent.get_tail_rest_matrix().invert() * rest
			pose = self.parent.get_tail_pose_matrix().invert() * pose
		matrix = rest.invert() * pose
		loc = matrix.translation_part()
		rot = matrix.rotation_part().to_quat().normalize()
		return (loc, rot)

	# \brief Gets the armature space rest matrix of the bone head.
	def get_head_rest_matrix(self):
		return self.bone.matrix_local.copy()

	# \brief Gets the armature space rest matrix of the bone tail.
	def get_tail_rest_matrix(self):
		return self.get_head_rest_matrix() * mathutils.Matrix.Translation(self.len)

	# \brief Gets the armature space pose transformation of the bone head.
	def get_head_pose_matrix(self):
		return self.pose_bone.matrix.copy()

	# \brief Gets the armature space pose transformation of the bone tail.
	def get_tail_pose_matrix(self):
		return self.get_head_pose_matrix() * mathutils.Matrix.Translation(self.len)

	def write_special(self, writer):
		writer.write_float(self.len.x)
		writer.write_float(self.len.y)
		writer.write_float(self.len.z)

class LINodeLight(LINode):

	def __init__(self, parent, object):
		LINode.__init__(self, parent, object)
		self.type = LIFormat.NODETYPE_LIGHT
		self.flags = 0
		if object.data.shadow_method != 'NOSHADOW':
			self.flags |= LIFormat.LIGHTFLAG_NOSHADOW
		if object.data.type == 'SPOT':
			self.clip_start = object.data.shadow_buffer_clip_start
			self.clip_end = object.data.shadow_buffer_clip_end
			self.spot_cutoff = object.data.spot_size
			self.spot_size = 0.5 * object.data.spot_size
			self.spot_blend = 32.0 * object.data.spot_blend
		else:
			self.clip_start = 0.1
			self.clip_end = 100
			self.spot_cutoff = math.pi
			self.spot_size = math.pi
			self.spot_blend = 0
		self.diffuse = [0, 0, 0]
		self.diffuse[0] = object.data.energy * object.data.color[0]
		self.diffuse[1] = object.data.energy * object.data.color[1]
		self.diffuse[2] = object.data.energy * object.data.color[2]
		self.equation = [1, 1, 1]
		self.equation[1] = object.data.linear_attenuation / object.data.distance
		self.equation[2] = object.data.quadratic_attenuation / (object.data.distance * object.data.distance)

	def write_special(self, writer):
		writer.write_int(self.flags)
		writer.write_float(self.spot_cutoff)
		writer.write_float(self.clip_start)
		writer.write_float(self.clip_end)
		writer.write_float(self.diffuse[0])
		writer.write_float(self.diffuse[1])
		writer.write_float(self.diffuse[2])
		writer.write_float(self.equation[0])
		writer.write_float(self.equation[1])
		writer.write_float(self.equation[2])
		writer.write_float(self.spot_size)
		writer.write_float(self.spot_blend)

class LINodeRoot(LINode):

	def __init__(self, hierarchy):
		self.type = LIFormat.NODETYPE_EMPTY
		self.nodes = []
		self.object = None
		self.parent = None
		self.hierarchy = hierarchy
		self.name = '#'
		self.rot = LIFormat.quat
		self.loc = mathutils.Vector((0, 0, 0))

class LIParticle:

	def __init__(self):
		self.frame_start = 0
		self.frame_end = 0
		self.frames = []

	def add_frame(self, frame):
		self.frames.append(frame)
		self.frame_end += 1

	def is_empty(self):
		return len(self.frames) <= 1

	# \brief Saves the particle.
	# \param self Particle.
	# \param writer Writer.
	def write(self, writer):
		# FIXME: The first frame is broken for some reason.
		if len(self.frames):
			del self.frames[0]
		writer.write_int(self.frame_start)
		writer.write_int(len(self.frames))
		for f in self.frames:
			writer.write_float(f.x)
			writer.write_float(f.y)
			writer.write_float(f.z)
		writer.write_marker()

class LIParticles:

	def __init__(self, file):
		# Create particle systems.
		self.file = file
		self.particlesystems = []
		obj = bpy.context.scene.objects.active
		for object in bpy.context.scene.objects:
			if self.file in object_files(object):
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

class LIParticleSystem:

	def __init__(self, object, system):
		self.object = object
		self.system = system
		self.particles = []
		self.frame_start = 0
		self.frame_end = int(self.system.settings.frame_end)
		self.lifetime = int(self.system.settings.lifetime)
		self.particle_size = self.system.settings.particle_size
		# Set material texture.
		self.texture = "particle1"
		mat = self.system.settings.material - 1
		if mat < len(self.object.material_slots):
			mat = self.object.material_slots[mat]
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
		writer.write_int(self.frame_start)
		writer.write_int(self.frame_end)
		writer.write_int(self.frame_end + self.lifetime)
		writer.write_float(self.particle_size)
		writer.write_string(self.texture)
		writer.write_int(len(self.particles))
		writer.write_marker()
		for par in self.particles:
			par.write(writer)

class LIShape:

	def __init__(self, name):
		self.name = name
		self.parts = []

	def add_mesh(self, obj):
		self.parts.append(LIShapePart(obj))

	def write(self, writer):
		writer.write_string(self.name)
		writer.write_int(len(self.parts))
		writer.write_marker()
		for p in self.parts:
			p.write(writer)

class LIShapePart:

	def __init__(self, obj):
		self.vertices = []
		matrix = LIFormat.matrix.copy().to_3x3() * obj.matrix_world.rotation_part()
		for v in obj.data.vertices:
			self.vertices.append(v.co * matrix)

	def write(self, writer):
		writer.write_int(len(self.vertices))
		writer.write_marker()
		for v in self.vertices:
			writer.write_float(v.x)
			writer.write_float(v.y)
			writer.write_float(v.z)
			writer.write_marker()

class LITexture:

	def __init__(self, index, tex, img):
		# Defaults.
		self.index = index
		self.string = ''
		self.flags = LIFormat.TEXFLAG_DEFAULT
		self.type = LIFormat.TEXTYPE_NONE
		# Parameters.
		if tex:
			self.flags = 0
			if tex.extension == 'REPEAT':
				self.flags |= LIFormat.TEXFLAG_CLAMP
			else:
				self.flags |= LIFormat.TEXFLAG_REPEAT
			if tex.use_interpolation:
				self.flags |= LIFormat.TEXFLAG_BILINEAR
			if tex.use_mipmap:
				self.flags |= LIFormat.TEXFLAG_MIPMAP
		# Filename.
		if img:
			self.type = LIFormat.TEXTYPE_IMAGE
			self.string = os.path.splitext(os.path.basename(img.filepath))[0]

	def write(self, writer):
		writer.write_int(self.type)
		writer.write_int(self.flags)
		writer.write_int(0)#512)
		writer.write_int(0)#512)
		writer.write_string(self.string)
		writer.write_marker()

class LIVertex:

	def __init__(self, index, co, no, uv, we):
		self.index = index
		self.co = co
		self.no = no
		self.uv = uv
		self.we = we

	def write(self, writer):
		writer.write_float(self.uv[0])
		writer.write_float(1.0 - self.uv[1])
		writer.write_float(self.no.x)
		writer.write_float(self.no.y)
		writer.write_float(self.no.z)
		writer.write_float(self.co.x)
		writer.write_float(self.co.y)
		writer.write_float(self.co.z)
		writer.write_marker()

	def write_weights(self, writer):
		writer.write_int(len(self.we))
		for weight in self.we:
			writer.write_int(weight[0])
			writer.write_float(weight[1])
		writer.write_marker()

##############################################################################

class LIFile:

	def __init__(self, file):
		tempobjs = []
		origobjs = {}
		self.coll = LICollision()
		self.mesh = None
		self.filepath = file
		# Find objects.
		for obj in bpy.data.objects:
			origobjs[obj] = True
		# Create collision shapes.
		for obj in bpy.data.objects:
			if obj.type == 'MESH' and file in object_files(obj):
				self.coll.add_mesh(obj)
		# Select meshes.
		for obj in bpy.data.objects:
			sel = False
			if obj.type == 'MESH' and file in object_files(obj):
				try:
					sel = (obj["render"] != "false")
				except:
					sel = True
			obj.select = sel
		# Duplicate meshes.
		bpy.ops.object.duplicate()
		# Select duplicates.
		for obj in bpy.data.objects:
			if obj.select:
				if obj in origobjs:
					# Unselect originals.
					obj.select = False
				else:
					# Select duplicates.
					obj.select = True
					tempobjs.append(obj)
		# Apply modifiers to duplicated meshes.
		for obj in tempobjs:
			# Remove unwanted modifiers.
			for mod in obj.modifiers:
				if mod.type == 'SUBSURF':
					obj.modifiers.remove(mod)
				if mod.type == mod.type == 'ARMATURE':
					obj.modifiers.remove(mod)
			# Apply modifiers.
			oldmesh = obj.data
			obj.data = obj.create_mesh(bpy.context.scene, True, 'PREVIEW')
			bpy.data.meshes.remove(oldmesh)
		# Simplify duplicated meshes.
		for obj in tempobjs:
			# Apply transformation.
			bpy.context.scene.objects.active = obj
			bpy.ops.object.location_clear()
			bpy.ops.object.scale_apply()
			obj.matrix_local = LIFormat.matrix * obj.matrix_local
			bpy.ops.object.rotation_apply()
			# Triangulate.
			if obj.mode != 'EDIT':
				bpy.ops.object.editmode_toggle()
			bpy.ops.mesh.select_all(action='SELECT')
			bpy.ops.mesh.quads_convert_to_tris()
			bpy.ops.object.editmode_toggle()
			# Tidy vertex weights.
			bpy.ops.object.vertex_group_clean(all_groups=True)
		if len(tempobjs):
			# Join meshes.
			bpy.ops.object.join()
			object = bpy.context.scene.objects.active
			# Build the mesh.
			try:
				self.mesh = LIMesh(object)
			except Exception as e:
				bpy.ops.object.delete()
				raise e
			# Delete the temporary mesh.
			bpy.ops.object.delete()
		# Build the node hierarchy.
		self.hier = LIHierarchy(file)
		# Build particle animations.
		self.particles = LIParticles(file)

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
		self.file.write(struct.pack('c', '\0'))
		self.file.write(struct.pack("!I", data.pos))
		self.file.write(data.data)

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
		self.data += struct.pack('c', '\0')
		self.pos += len(value) + 1

##############################################################################

class LIExporter(bpy.types.Operator):
	'''Export to Lips of Suna (.lmdl)'''

	bl_idname = "export.lipsofsuna"
	bl_label = 'Export to Lips of Suna (.lmdl)'

	def execute(self, context):
		# Adjust state.
		alllayers = [True for x in range(0, 20)]
		origlayers = [x for x in bpy.context.scene.layers]
		origframe = bpy.context.scene.frame_current
		bpy.context.scene.layers = alllayers
		if bpy.context.scene.objects.active:
			bpy.ops.object.mode_set(mode='OBJECT', toggle=False)
		# Find target files.
		files = []
		for obj in bpy.data.objects:
			for file in object_files(obj):
				if file not in files:
					files.append(file)
		# If no target file was specified by any object, assume that the
		# user wanted to export all the objects to the default file.
		if not len(files):
			path,name = os.path.split(bpy.data.filepath)
			path = os.path.join(os.path.split(path)[0], "graphics")
			name = os.path.splitext(name)[0] + ".lmdl"
			files = [os.path.join(path, name)]
			LIFormat.files = files
		# Export each file.
		for file in files:
			f = LIFile(file)
			f.write()
		# Restore state.
		bpy.context.scene.layers = origlayers
		bpy.context.scene.frame_set(origframe)
		return {'FINISHED'}

	def invoke(self, context, event):
		return self.execute(context)

##############################################################################

def menu_func(self, context):
	self.layout.operator(LIExporter.bl_idname, text="Lips of Suna (.lmdl)")

def register():
	bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
	bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
	register()
