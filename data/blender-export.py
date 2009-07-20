#!BPY

import Blender
from Blender.Mathutils import *
import array, math, os, struct, sys, zlib, StringIO

def PowerOfTwo(value):
	for i in range(2, 12):
		size = (1<<i)
		if value <= size:
			return size
	return 2048

class LipsEnum:
	def __init__(self):
		self.TexFlags = LipsEnumTexFlags()
		self.TexTypes = LipsEnumTexTypes()
class LipsEnumTexFlags:
	def __init__(self):
		self.BILINEAR = 1
		self.CLAMP = 2
		self.MIPMAP = 4
		self.REPEAT = 8
class LipsEnumTexTypes:
	def __init__(self):
		self.NONE = 0
		self.ENVMAP = 1
		self.IMAGE = 2
Lips = LipsEnum()

#############################################################################
# Default functions.

def MaterialDiffuse(bmat):
	if bmat:
		if bmat.getMode() & Blender.Material.Modes['ZTRANSP']:
			return [bmat.R, bmat.G, bmat.B, bmat.alpha]
		return [bmat.R, bmat.G, bmat.B, 1.0]
	return [1.0, 1.0, 1.0, 1.0]

def MaterialEmission(bmat):
	if bmat:
		return bmat.emit
	return 0.0

def MaterialFlags(bmat, bmesh, bface):
	flags = 0
	if bmat:
		if bmat.getMode() & Blender.Material.Modes['ZTRANSP']:
			# Texture and static transparency.
			flags |= 8
	if bmesh:
		if bface and bmesh.faceUV:
			if bface.mode & Blender.Mesh.FaceModes['DYNAMIC']:
				# Collisions enabled.
				flags |= 2
			if not (bface.mode & Blender.Mesh.FaceModes['TWOSIDE']):
				# Back face culling.
				flags |= 4
			if bface.mode & Blender.Mesh.FaceModes['BILLBOARD']:
				# Render as billboard.
				flags |= 1
		else:
			if not (bmesh.mode & Blender.Mesh.Modes['TWOSIDED']):
				# Back face culling.
				flags |= 4
			# Collisions enabled.
			flags |= 2
	return flags

def MaterialShader(bmat):
	if bmat:
		tmp = bmat.name.split("::")
		if len(tmp) >= 2:
			return tmp[1]
	return "default"

def MaterialShininess(bmat):
	if bmat:
		return bmat.hard / 511.0 * 128.0
	return 0.0

def MaterialSpecular(bmat):
	if bmat:
		return [bmat.specR, bmat.specG, bmat.specB, bmat.spec]
	return [1.0, 1.0, 1.0, 1.0]

def MaterialStrands(bmat):
	# FIXME: No python API for strands apparently so abusing Z offset.
	if bmat:
		return [bmat.getZOffset(), 0.0, 0.0]
	return [0.2, 0.0, 0.0]

def MaterialTextures(bmat, bmesh, bface):
	count = 0
	index = 0
	textures = []
	if bmat:
		for texture in bmat.getTextures():
			index = index + 1
			if not texture or not texture.tex:
				# Nil texture.
				textures.append(LipsTexture(Lips.TexTypes.NONE, 0, 0, 0, ""))
				continue
			texture = texture.tex
			if texture.type is Blender.Texture.Types.NONE:
				# None texture.
				textures.append(LipsTexture(Lips.TexTypes.NONE, 0, 0, 0, ""))
			elif texture.type is Blender.Texture.Types.IMAGE and texture.image:
				# Image texture.
				name = os.path.basename(texture.image.filename)
				name = os.path.splitext(name)[0]
				flags = 0
				if texture.interpol: flags |= Lips.TexFlags.BILINEAR
				if texture.mipmap: flags |= Lips.TexFlags.MIPMAP
				if texture.extend is Blender.Texture.ExtendModes.EXTEND: flags |= Lips.TexFlags.CLAMP
				if texture.extend is Blender.Texture.ExtendModes.REPEAT: flags |= Lips.TexFlags.REPEAT
				if texture.extend is Blender.Texture.ExtendModes.CHECKER: flags |= Lips.TexFlags.REPEAT
				textures.append(LipsTexture(Lips.TexTypes.IMAGE, flags, 0, 0, name))
				count = index
			elif texture.type is Blender.Texture.Types.ENVMAP:
				# Envmap texture.
				# FIXME: Blender doesn't support querying the Cuberes variable?
				width = PowerOfTwo(128)#texture.cubeRes
				height = width
				flags = Lips.TexFlags.BILINEAR | Lips.TexFlags.REPEAT
				textures.append(LipsTexture(Lips.TexTypes.ENVMAP, flags, width, height, ""))
				count = index
			else:
				# Other texture.
				textures.append(LipsTexture(Lips.TexTypes.NONE, 0, 0, 0, ""))
	if bmesh and bface and bmesh.faceUV and bface.image:
		if not bmat or (bmat.getMode() & Blender.Material.Modes['TEXFACE']):
			name = os.path.basename(bface.image.filename)
			name = os.path.splitext(name)[0]
			flags = Lips.TexFlags.BILINEAR | Lips.TexFlags.MIPMAP | Lips.TexFlags.REPEAT
			if count > 0:
				if textures[0].type is Lips.TexTypes.IMAGE:
					flags = textures[0].flags
				textures[0] = LipsTexture(Lips.TexTypes.IMAGE, flags, 0, 0, name)
			else:
				textures.append(LipsTexture(Lips.TexTypes.IMAGE, flags, 0, 0, name))
				count = 1
	return textures[:count]

def MeshVisible(object, mesh):
	bsystems = object.getParticleSystems()
	if len(bsystems) == 0:
		return 1
	for bparticles in bsystems:
		if bparticles.renderEmitter:
			return 1
	return 0

def NodeChild(scene, object, bone, child):
	if object != child.parent:
		return 0
	if child.parentbonename:
		if not bone or bone.name != child.parentbonename:
			return 0
	elif bone:
		return 0
	if child.type == "Armature":
		return 1
	if child.type == "Empty":
		return 1
	if child.type == "Lamp" and child.name.find("::on") != -1:
		#if child.getData().type == Blender.Lamp.Types["Spot"]:
		return 1
	return 0

def NodeChildren(scene, object, bone, parent):
	nodes = []
	if object and object.type == "Armature":
		for b in object.getData().bones.values():
			if b.parent == bone:
				nodes.append(LipsNode(LipsNodeType.BONE, scene, object, b, parent))
	for o in scene.objects:
		if lips_exporter_calls["NodeChild"](scene, object, bone, o):
			if o.type == "Armature":
				nodes.append(LipsNode(LipsNodeType.EMPTY, scene, o, None, parent))
			if o.type == "Empty":
				nodes.append(LipsNode(LipsNodeType.EMPTY, scene, o, None, parent))
			if o.type == "Lamp":
				nodes.append(LipsNode(LipsNodeType.LIGHT, scene, o, None, parent))
	return nodes

def ObjectHairs(lips, object):
	global lips_correction_matrix
	for bparticles in object.getParticleSystems():
		if bparticles.type is not Blender.Particle.TYPE['HAIR']:
			continue
		bmat = bparticles.getMat()
		locss = bparticles.getLoc(1)
		sizes = bparticles.getSize(1)
		for locs in locss:
			if locs is not None:
				hair = LipsHair()
				for loc in locs:
					tmp = Vector(loc[0], loc[1], loc[2]) * lips_correction_matrix
					hair.AppendNode(tmp[0], tmp[1], tmp[2], 1.0) # FIXME: Size.
				if len(hair.nodes):
					lips.AddHair(bmat, hair)

def VertexCoord(object, mesh, face, index):
	global lips_correction_matrix
	matrix = object.matrix * lips_correction_matrix
	coord = face.v[index].co.copy().resize4D() * matrix
	return coord

def VertexNormal(object, mesh, face, index, conn):
	global lips_correction_matrix
	matrix = object.matrix * lips_correction_matrix
	matrix = matrix.rotationPart()
	if not face.smooth:
		normal = face.no * matrix
		normal.normalize()
		return normal
	if not conn.edgesplit:
		normal = face.v[index].no * matrix
		normal.normalize()
		return normal
	normal = conn.CalculateNormal(face.index, face.v[index].index) * matrix
	normal.normalize()
	return normal

def VertexTexcoords(object, mesh, face, index):
	number = 0
	values = [0, 0, 0, 0, 0, 0]
	# TODO: Support sticky coordinates.
	# TODO: Support texture coordinate modes other than UV.
	if not mesh.faceUV:
		return values
	for layer in mesh.getUVLayerNames():
		mesh.activeUVLayer = layer
		values[number + 0] = face.uv[index][0]
		values[number + 1] = 1.0 - face.uv[index][1]
		number = number + 2
		if number == 6:
			break
	return values

def VertexWeights(object, mesh, face, index, bones):
	total = 0
	weights = []
	for i in mesh.getVertexInfluences(face.v[index].index):
		if i[1] > 0 and i[0] in bones:
			total = total + i[1]
			index = bones.index(i[0])
			weights.append([index, i[1]])
	if total <= 0:
		return []
	for weight in weights:
		weight[1] = weight[1] / total
	return weights

#############################################################################
# Function customization.

lips_format_version = 0xFFFFFFF9
lips_animation_timescale = 0.01
lips_minimum_box_size = 0.3
lips_correction_matrix = Euler(-90, 0, 0).toMatrix().resize4x4()
lips_correction_quat = Euler(-90, 0, 0).toQuat()
lips_exporter_calls = \
{ \
	"MaterialDiffuse": MaterialDiffuse, \
	"MaterialEmission": MaterialEmission, \
	"MaterialFlags": MaterialFlags, \
	"MaterialShader": MaterialShader, \
	"MaterialShininess": MaterialShininess, \
	"MaterialSpecular": MaterialSpecular, \
	"MaterialStrands": MaterialStrands, \
	"MaterialTextures": MaterialTextures, \
	"ObjectHairs": ObjectHairs, \
	"MeshVisible": MeshVisible, \
	"NodeChild": NodeChild, \
	"NodeChildren": NodeChildren, \
	"VertexCoord": VertexCoord, \
	"VertexNormal": VertexNormal, \
	"VertexTexcoords": VertexTexcoords, \
	"VertexWeights": VertexWeights \
}

sys.path.append(os.getcwd())
if os.access("custom.py", os.R_OK):
	import custom

#############################################################################
# Animation.

class LipsAnimation:

	# Initializes an animation from Blender data.
	def __init__(self, bstrip):
		self.strip = bstrip
		self.action = bstrip.action

	# Gets the interpolation curves for the requested bone.
	def GetCurves(self, name):
		if name not in self.action.getChannelNames():
			return [LipsCurve(self.strip, None), \
			        LipsCurve(self.strip, None), \
			        LipsCurve(self.strip, None), \
			        LipsCurve(self.strip, None), \
			        LipsCurve(self.strip, None), \
			        LipsCurve(self.strip, None), \
			        LipsCurve(self.strip, None),]
		ipo = self.action.getChannelIpo(name)
		curves = []
		channels = ["LocX", "LocY", "LocZ", "QuatX", "QuatY", "QuatZ", "QuatW"]
		for channel in channels:
			found = 0
			for curve in ipo.curves:
				if curve.name == channel:
					curves.append(LipsCurve(self.strip, curve))
					found = 1
					break
			if not found:
				curves.append(LipsCurve(self.strip, None))
		return curves

	def Write(self, writer):
		channels = self.action.getChannelNames()
		writer.WriteString(self.action.name)
		writer.WriteInt(len(channels))
		writer.WriteFloat(lips_animation_timescale * (self.strip.actionEnd - self.strip.actionStart))
		writer.WriteFloat(lips_animation_timescale * self.strip.blendIn)
		writer.WriteFloat(lips_animation_timescale * self.strip.blendOut)
		for channel in channels:
			curves = self.GetCurves(channel)
			writer.WriteString(channel)
			writer.WriteInt(len(curves))
			for curve in curves:
				writer.WriteInt(curve.type)
				writer.WriteInt(curve.length)
				for value in curve.values:
					writer.WriteFloat(value)


class LipsAnimations:

	# \brief Initializes a new armature.
	def __init__(self):
		self.animations = []

	# \brief Add animations from a Blender armature object.
	#
	# \param self Animation manager.
	# \param object Blender armature object.
	# \param armature Blender armature data.
	def AddArmature(self, object, armature):
		for action in object.actionStrips:
			self.animations.append(LipsAnimation(action))

	# \brief Writes animations.
	#
	# \param self Animation manager.
	# \param writer Writer.
	def Write(self, writer):
		writer.WriteInt(len(self.animations))
		for animation in self.animations:
			animation.Write(writer)


class LipsCurve:

	# \brief Initializes a new IPO curve from Blender data.
	def __init__(self, bstrip, bcurve):
		self.strip = bstrip
		self.curve = bcurve
		self.values = []
		if bcurve is None:
			self.type = 0
			self.length = 0
			return
		scale = lips_animation_timescale
		offset = self.strip.actionStart
		if bcurve.interpolation == Blender.IpoCurve.InterpTypes.CONST:
			self.type = 0
			self.length = 2 * len(bcurve.bezierPoints)
			for bezier in bcurve.bezierPoints:
				self.values.append(scale * (bezier.vec[1][0] - offset))
				self.values.append(bezier.vec[1][1])
		elif bcurve.interpolation == Blender.IpoCurve.InterpTypes.LINEAR:
			self.type = 1
			self.length = 2 * len(bcurve.bezierPoints)
			for bezier in bcurve.bezierPoints:
				self.values.append(scale * (bezier.vec[1][0] - offset))
				self.values.append(bezier.vec[1][1])
		elif bcurve.interpolation == Blender.IpoCurve.InterpTypes.BEZIER:
			self.type = 2
			self.length = 6 * len(bcurve.bezierPoints)
			for bezier in bcurve.bezierPoints:
				self.values.append(scale * (bezier.vec[0][0] - offset))
				self.values.append(bezier.vec[0][1])
				self.values.append(scale * (bezier.vec[1][0] - offset))
				self.values.append(bezier.vec[1][1])
				self.values.append(scale * (bezier.vec[2][0] - offset))
				self.values.append(bezier.vec[2][1])
		else:
			print("W: Unknown curve type.")

#############################################################################
# Mesh.

class LipsConnectivity:

	# \brief Creates face connectivity data for the mesh.
	#
	# \param self Connectivity.
	# \param object Blender object.
	# \param mesh Blender mesh data.
	def __init__(self, object, mesh):
		self.mesh = mesh
		self.edgesplit = 0
		for mod in object.modifiers:
			if mod.type == Blender.Modifier.Types.EDGESPLIT:
				self.edgesplit = 1
				break
		if self.edgesplit:
			self.vertexfaces = []
			for vert in mesh.verts:
				self.vertexfaces.append([])
			for face in mesh.faces:
				for vert in face.verts:
					if face.index not in self.vertexfaces[vert.index]:
						self.vertexfaces[vert.index].append(face.index)

	# \brief Recalculates the normal for a face vertex.
	#
	# Calculates a face vertex normal using sharp edge information
	# in a manner similar to how the EdgeSplit modifier works.
	#
	# \param self Connectivity.
	# \param faceidx Face index.
	# \param vertidx Vertex index.
	# \return Vector.
	def CalculateNormal(self, faceidx, vertidx):

		# Initialize face loop.
		normal = self.mesh.faces[faceidx].no
		edgeidx, sharp = self.FindFirstEdge(faceidx, vertidx)
		visited = [edgeidx]

		# Loop faces backward.
		if not sharp:
			tmpe, tmpf = edgeidx, faceidx
			faceidx = self.FindOtherFace(faceidx, edgeidx, vertidx)
			if faceidx != -1:
				while faceidx != tmpf:
					normal = normal + self.mesh.faces[faceidx].no
					edgeidx, sharp = self.FindOtherEdge(faceidx, edgeidx, vertidx)
					if sharp or edgeidx in visited:
						break
					visited.append(edgeidx)
					faceidx = self.FindOtherFace(faceidx, edgeidx, vertidx)
					if faceidx == -1:
						break
			edgeidx, faceidx = tmpe, tmpf

		# Loop faces forward.
		while 1:
			edgeidx, sharp = self.FindOtherEdge(faceidx, edgeidx, vertidx)
			if sharp or edgeidx in visited:
				break
			visited.append(edgeidx)
			faceidx = self.FindOtherFace(faceidx, edgeidx, vertidx)
			if faceidx == -1:
				break
			normal = normal + self.mesh.faces[faceidx].no

		# Return average normal.
		normal.normalize()
		return normal

	# \brief Favoring sharp edges, finds an edge in the face that contains the vertex.
	#
	# \param faceidx Face index.
	# \param vertidx Vertex index.
	# \return Edge index, sharp flag.
	def FindFirstEdge(self, faceidx, vertidx):
		for edgekey in self.mesh.faces[faceidx].edge_keys:
			edge = self.mesh.edges[self.mesh.findEdges([edgekey])]
			if vertidx in edge.key:
				if edge.flag & Blender.Mesh.EdgeFlags.SHARP:
					return edge.index, 1
		for edgekey in self.mesh.faces[faceidx].edge_keys:
			edge = self.mesh.edges[self.mesh.findEdges([edgekey])]
			if vertidx in edge.key:
				return edge.index, 0
		return -1, 1

	# \brief Finds the second edge in the face that contains the vertex.
	#
	# \param self Connectivity.
	# \param faceidx Face index.
	# \param edgeidx Edge index.
	# \param vertidx Vertex index.
	# \return Edge index, sharp flag.
	def FindOtherEdge(self, faceidx, edgeidx, vertidx):
		for edgekey in self.mesh.faces[faceidx].edge_keys:
			edge = self.mesh.edges[self.mesh.findEdges([edgekey])]
			if edge.index != edgeidx:
				if vertidx in edgekey:
					if edge.flag & Blender.Mesh.EdgeFlags.SHARP:
						return edge.index, 1
					else:
						return edge.index, 0
		return -1, 1

	# Finds the second face in the mesh that contains the edge and the vertex.
	#
	# \param self Connectivity.
	# \param faceidx Face index.
	# \param edgeidx Edge index.
	# \param vertidx Vertex index.
	# \return Face index.
	def FindOtherFace(self, faceidx, edgeidx, vertidx):
		for face in self.vertexfaces[vertidx]:
			if face != faceidx:
				for edgekey in self.mesh.faces[face].edge_keys:
					edge = self.mesh.edges[self.mesh.findEdges([edgekey])]
					if edge.index == edgeidx:
						return face
		return -1


class LipsFaces:

	# Initializes a new face manager.
	#
	# \param self Face manager.
	def __init__(self):
		self.groups = {}

	# Adds a new face.
	#
	# \param self Face manager.
	# \param mat Material index.
	# \param verts Array of vertices.
	def AddFace(self, mat, verts):
		if mat in self.groups:
			self.groups[mat].append(verts[0])
			self.groups[mat].append(verts[1])
			self.groups[mat].append(verts[2])
		else:
			self.groups[mat] = [verts[0], verts[1], verts[2]]

	# \brief Gets the bounding box of the mesh.
	#
	# \return Array of type [xmin, ymin, zmin, xmax, ymax, zmax].
	def GetBounds(self):
		first = 1
		bounds = [0, 0, 0, 0, 0, 0]
		# Calculate box size.
		for group in self.groups.values():
			for vertex in group:
				if first:
					first = 0
					bounds[0] = vertex.values[9]
					bounds[1] = vertex.values[10]
					bounds[2] = vertex.values[11]
					bounds[3] = vertex.values[9]
					bounds[4] = vertex.values[10]
					bounds[5] = vertex.values[11]
				else:
					if bounds[0] > vertex.values[9]:
						bounds[0] = vertex.values[9]
					if bounds[1] > vertex.values[10]:
						bounds[1] = vertex.values[10]
					if bounds[2] > vertex.values[11]:
						bounds[2] = vertex.values[11]
					if bounds[3] < vertex.values[9]:
						bounds[3] = vertex.values[9]
					if bounds[4] < vertex.values[10]:
						bounds[4] = vertex.values[10]
					if bounds[5] < vertex.values[11]:
						bounds[5] = vertex.values[11]
		# Enforce minimum size.
		for i in range(3):
			if abs(bounds[i+3] - bounds[i+0]) < lips_minimum_box_size:
				tmp = 0.5 * (bounds[i+3] + bounds[i+0])
				bounds[i+0] = tmp - 0.5 * lips_minimum_box_size
				bounds[i+3] = tmp + 0.5 * lips_minimum_box_size
		return bounds

	# Saves all face groups.
	#
	# \param self Face manager.
	# \param writer Writer.
	def Write(self, writer):
		keys = self.groups.keys()
		writer.WriteInt(len(keys))
		for mat in keys:
			verts = self.groups[mat]
			writer.WriteInt(mat)
			writer.WriteInt(len(verts))
			for vertex in verts:
				vertex.WriteCoords(writer)
			for vertex in verts:
				vertex.WriteWeights(writer)


class LipsMaterial:

	def __init__(self, index, flags, shininess, diffuse, specular, strands, shader, textures):
		self.index = index
		self.flags = flags
		self.shader = shader
		self.textures = textures
		self.emission = 0.0
		self.shininess = shininess
		self.diffuse = diffuse
		self.strands = strands
		self.specular = specular

	def Write(self, writer):
		writer.WriteInt(self.flags)
		writer.WriteFloat(self.emission)
		writer.WriteFloat(self.shininess)
		writer.WriteFloat(self.diffuse[0])
		writer.WriteFloat(self.diffuse[1])
		writer.WriteFloat(self.diffuse[2])
		writer.WriteFloat(self.diffuse[3])
		writer.WriteFloat(self.specular[0])
		writer.WriteFloat(self.specular[1])
		writer.WriteFloat(self.specular[2])
		writer.WriteFloat(self.specular[3])
		writer.WriteFloat(self.strands[0])
		writer.WriteFloat(self.strands[1])
		writer.WriteFloat(self.strands[2])
		writer.WriteInt(len(self.textures))
		writer.WriteString(self.shader)
		for texture in self.textures:
			texture.Write(writer)


class LipsTexture:

	def __init__(self, type, flags, width, height, string):
		self.type = type
		self.flags = flags
		self.width = width
		self.height = height
		self.string = string

	def Write(self, writer):
		writer.WriteInt(self.type)
		writer.WriteInt(self.flags)
		writer.WriteInt(self.width)
		writer.WriteInt(self.height)
		writer.WriteString(self.string)

	def GetKey(self):
		return "%d %d %d %d %s" % (self.type, self.flags, self.width, self.height, self.string)


class LipsVertex:

	# \param mesh Blender mesh.
	# \param co Vertex coordinates.
	# \param no Vertex normal.
	# \param te List of six texture coordinates.
	# \param we List of bone weights of type [index, influence].
	def __init__(self, co, no, te, we):
		self.weights = we
		self.values = array.array('f', [
			te[0], te[1], te[2], te[3], te[4], te[5],
			no.x, no.y, no.z, co.x, co.y, co.z])

	def WriteCoords(self, writer):
		writer.WriteFloat(self.values[0])
		writer.WriteFloat(self.values[1])
		writer.WriteFloat(self.values[2])
		writer.WriteFloat(self.values[3])
		writer.WriteFloat(self.values[4])
		writer.WriteFloat(self.values[5])
		writer.WriteFloat(self.values[6])
		writer.WriteFloat(self.values[7])
		writer.WriteFloat(self.values[8])
		writer.WriteFloat(self.values[9])
		writer.WriteFloat(self.values[10])
		writer.WriteFloat(self.values[11])

	def WriteWeights(self, writer):
		writer.WriteInt(len(self.weights))
		for weight in self.weights:
			writer.WriteInt(weight[0])
			writer.WriteFloat(weight[1])


class LipsWeightGroup:

	# \brief Creates a new weight group.
	#
	# \param index Weight group index.
	# \param name Weight group name.
	def __init__(self, index, name):
		self.index = index
		self.name = name

	# \brief Saves the weight group.
	#
	# \param self Weight group.
	# \param writer Writer.
	def Write(self, writer):
		writer.WriteString(self.name)
		writer.WriteString(self.name)


#############################################################################
# Hierarchy.

class LipsEnum:

	def __init__(self, dict):
		self.dict = dict

	def __getattr__(self, attr):
		return self.dict[attr]

LipsNodeType = LipsEnum({'BONE': 0, 'EMPTY': 1, 'LIGHT': 2})

class LipsNode:

	# \brief Creates a new node and its children recursively.
	#
	# \param type Lips node type.
	# \param scene Lips scene.
	# \param object Blender object or None if this is the root node.
	# \param bone Blender bone or None.
	# \param parent Lips node or None if this is the root node.
	def __init__(self, type, scene, object, bone, parent):
		self.type = type
		self.scene = scene
		self.object = object
		if object:
			self.data = object.getData()
		self.bone = bone
		self.parent = parent
		self.nodes = lips_exporter_calls["NodeChildren"](scene, object, bone, self)
		return

	# \brief Prints debug information about the node and its children.
	def Debug(self):
		if self.bone:
			pos = self.GetPosition()
			rot = self.GetRotation()
			if self.parent and self.parent.type == LipsNodeType.BONE:
				pos = self.parent.debugtail + self.parent.debugquat * pos
				rot = (rot.toMatrix() * self.parent.debugquat.toMatrix()).toQuat()
			head0 = pos
			tail0 = pos + rot * self.GetLength()
			head1 = Vector(self.bone.head['ARMATURESPACE'])
			tail1 = Vector(self.bone.tail['ARMATURESPACE'])
			self.debugquat = rot
			self.debughead = head0
			self.debugtail = tail0
			if (head0 - head1).length > 0.01 or (tail0 - tail1).length > 0.01:
				pos = self.GetPosition()
				rot = self.GetRotation()
				print("W: ========== %s FAILED ==========" % self.bone.name)
				print("W: P={%f,%f,%f} R={%f,%f,%f,%f}" % (pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, rot.w))
				print("W: LS H={%f,%f,%f} T={%f,%f,%f}" % (head0.x, head0.y, head0.z, tail0.x, tail0.y, tail0.z))
				print("W: BL H={%f,%f,%f} T={%f,%f,%f}" % (head1.x, head1.y, head1.z, tail1.x, tail1.y, tail1.z))
		for node in self.nodes:
			node.Debug()

	# \brief Writes the node to a stream.
	#
	# \param self Node.
	# \param writer Writer.
	def Write(self, writer):
		pos = self.GetPosition()
		rot = self.GetRotation()
		writer.WriteInt(self.type)
		writer.WriteInt(len(self.nodes))
		writer.WriteString(self.GetName())
		writer.WriteFloat(pos.x)
		writer.WriteFloat(pos.y)
		writer.WriteFloat(pos.z)
		writer.WriteFloat(rot.x)
		writer.WriteFloat(rot.y)
		writer.WriteFloat(rot.z)
		writer.WriteFloat(rot.w)
		if self.type == LipsNodeType.BONE:
			length = self.GetLength()
			writer.WriteFloat(length.x)
			writer.WriteFloat(length.y)
			writer.WriteFloat(length.z)
		if self.type == LipsNodeType.LIGHT:
			writer.WriteInt(self.GetFlags())
			# Projection.
			spot = (self.data.type == Blender.Lamp.Types["Spot"])
			if spot:
				writer.WriteFloat(self.data.spotSize / 180.0 * math.pi)
			else:
				writer.WriteFloat(math.pi)
			writer.WriteFloat(self.data.clipStart)
			writer.WriteFloat(self.data.clipEnd)
			# Color.
			writer.WriteFloat(self.data.energy * self.data.R)
			writer.WriteFloat(self.data.energy * self.data.G)
			writer.WriteFloat(self.data.energy * self.data.B)
			# Equation.
			writer.WriteFloat(1.0)
			writer.WriteFloat(self.data.quad1 / self.data.dist)
			writer.WriteFloat(self.data.quad2 / (self.data.dist * self.data.dist))
			# Spot.
			if spot:
				writer.WriteFloat(0.5 * self.data.spotSize / 180.0 * math.pi)
				writer.WriteFloat(32.0 * self.data.spotBlend)
			else:
				writer.WriteFloat(math.pi)
				writer.WriteFloat(0)
		for node in self.nodes:
			node.Write(writer)

	def GetFlags(self):
		flags = 0
		if self.data.mode & Blender.Lamp.Modes["Shadows"]:
			flags |= 1
		return flags

	def GetLength(self):
		return Vector(0, self.bone.length, 0)

	def GetName(self):
		if self.bone:
			return self.bone.name
		elif self.object:
			if self.type == LipsNodeType.EMPTY:
				return "#" + self.object.name
			else:
				return self.object.name
		else:
			return "#"

	def GetPoseMatrix(self):
		posebone = self.object.getPose().bones[self.bone.name]
		matrix = Matrix(posebone.poseMatrix)
		matrix = TranslationMatrix(self.GetLength()) * matrix
		return matrix

	# \brief Gets the position of the object relative to its parent.
	#
	# \param self Node.
	# \return Vector.
	def GetPosition(self):
		if self.bone:
			return self.bone.head['BONESPACE']
		elif self.object:
			matrix = self.GetRestMatrix()
			return matrix.translationPart()
		else:
			return Vector(0, 0, 0)

	# FIXME: Not used by lights.
	def GetResolution(self):
		return self.data.bufferSize

	def GetRestMatrix(self):
		if self.parent and self.parent.bone:
			matrix = self.parent.GetPoseMatrix()
			matrix = self.object.matrixLocal * matrix.invert()
		else:
			matrix = self.object.matrixLocal
		return Matrix(matrix)

	# \brief Gets the rotation of the object relative to its parent.
	#
	# \param self Node.
	# \return Quaternion.
	def GetRotation(self):
		global lips_correction_quat
		if self.bone:
			matrix = Matrix(self.bone.matrix['BONESPACE'])
			matrix = matrix.rotationPart()
			quat = matrix.toQuat()
			quat.normalize()
			return quat
		elif self.object:
			matrix = self.GetRestMatrix()
			matrix = matrix.rotationPart()
			#matrix = blender_rotation_to_lips(matrix)
			quat = matrix.toQuat()
			quat.normalize()
			return quat
		else:
			quat = lips_correction_quat
			return quat

#############################################################################
# Particles.

class LipsHairs:

	# Initializes a new hair manager.
	#
	# \param self Hair manager.
	def __init__(self):
		self.groups = {}

	# Appends a new hair to the hair group.
	#
	# \param self Hair manager.
	# \param mat Material index.
	# \param hair Hair.
	def AddHair(self, mat, hair):
		if mat in self.groups:
			self.groups[mat].append(hair)
		else:
			self.groups[mat] = [hair]

	# Saves all hair groups.
	#
	# \param self Hair manager.
	# \param writer Writer.
	def Write(self, writer):
		keys = self.groups.keys()
		writer.WriteInt(len(keys))
		for mat in keys:
			hairs = self.groups[mat]
			writer.WriteInt(mat)
			writer.WriteInt(len(hairs))
			for hair in hairs:
				hair.Write(writer)


class LipsHair:

	# Initializes a new hair strip.
	def __init__(self):
		self.nodes = []

	# Appends a new node to the hair strip.
	#
	# \param self Hair.
	def AppendNode(self, x, y, z, s):
		self.nodes.append([x, y, z, s])

	# Saves the hair strip.
	#
	# \param self Hair.
	# \param writer Writer.
	def Write(self, writer):
		writer.WriteInt(len(self.nodes))
		for node in self.nodes:
			writer.WriteFloat(node[0])
			writer.WriteFloat(node[1])
			writer.WriteFloat(node[2])
			writer.WriteFloat(node[3])


#############################################################################
# Storage.

class LipsStorage:

	def __init__(self, scene):
		self.animations = LipsAnimations()
		self.hairs = LipsHairs()
		self.faces = LipsFaces()
		self.materials = {}
		self.node = None
		self.weightnames = []
		self.AddNode(LipsNode(LipsNodeType.EMPTY, scene, None, None, None))
		for obj in scene.objects:
			if obj.parent == None:
				if obj.type == "Armature":
					self.AddArmature(obj)
				if obj.type == "Mesh":
					self.AddMesh(obj)
					lips_exporter_calls["ObjectHairs"](self, obj)

	def AddArmature(self, bobj):
		self.animations.AddArmature(bobj, bobj.getData())

	def AddHair(self, bmat, hair):
		mat = self.AddMaterial(bmat, None, None)
		self.hairs.AddHair(mat.index, hair)

	def AddMaterial(self, bmat, bmesh, bface):
		flag = lips_exporter_calls["MaterialFlags"](bmat, bmesh, bface)
		shin = lips_exporter_calls["MaterialShininess"](bmat)
		diff = lips_exporter_calls["MaterialDiffuse"](bmat)
		spec = lips_exporter_calls["MaterialSpecular"](bmat)
		stra = lips_exporter_calls["MaterialStrands"](bmat)
		shad = lips_exporter_calls["MaterialShader"](bmat)
		text = lips_exporter_calls["MaterialTextures"](bmat, bmesh, bface)
		key = ""
		for tex in text:
			key = "%s %s" % (key, tex.GetKey())
		key = "%d %s %s %s %s %s [%s]" % (flag, shad, shin, diff, stra, spec, key)
		if key in self.materials:
			mat = self.materials[key]
		else:
			idx = len(self.materials.keys())
			mat = LipsMaterial(idx, flag, shin, diff, spec, stra, shad, text)
			self.materials[key] = mat
		return mat

	# \brief Adds a mesh.
	#
	# The model format requires normals and texture coordinates to be stored
	# per vertex so we need to create extra vertices for seams and sharp edges.
	# Faces also need to be grouped so that the material and UV texture is
	# constant within each group.
	#
	# \param self Storage.
	# \param bobj Blender object.
	def AddMesh(self, bobj):
		bmesh = bobj.getData(0, 1)
		if lips_exporter_calls["MeshVisible"](bobj, bmesh):
			conn = LipsConnectivity(bobj, bmesh)
			for name in bmesh.getVertGroupNames():
				if name not in self.weightnames:
					self.weightnames.append(name)
			for bface in bmesh.faces:
				# Choose material.
				bmat = None
				if bmesh.materials:
					bmat = bmesh.materials[bface.mat]
				mat = self.AddMaterial(bmat, bmesh, bface)
				# Create vertices.
				v = []
				for i in range(len(bface.v)):
					co = lips_exporter_calls["VertexCoord"](bobj, bmesh, bface, i)
					no = lips_exporter_calls["VertexNormal"](bobj, bmesh, bface, i, conn)
					te = lips_exporter_calls["VertexTexcoords"](bobj, bmesh, bface, i)
					we = lips_exporter_calls["VertexWeights"](bobj, bmesh, bface, i, self.weightnames)
					v.append(LipsVertex(co, no, te, we))
				# Insert faces.
				if len(v) == 3:
					self.faces.AddFace(mat.index, v)
				elif len(v) == 4:
					self.faces.AddFace(mat.index, [v[0], v[1], v[2]])
					self.faces.AddFace(mat.index, [v[0], v[2], v[3]])

	def AddNode(self, node):
		self.node = node

	# \brief Writes the model file.
	#
	# \param self Storage.
	# \param writer Writer.
	def Write(self, writer):
		bounds = self.faces.GetBounds();
		writer.WriteString("lips/mdl")
		writer.WriteInt(lips_format_version)
		writer.WriteInt(0)
		writer.WriteFloat(bounds[0])
		writer.WriteFloat(bounds[1])
		writer.WriteFloat(bounds[2])
		writer.WriteFloat(bounds[3])
		writer.WriteFloat(bounds[4])
		writer.WriteFloat(bounds[5])
		# Materials.
		materials = self.materials.values()
		materials.sort(lambda x,y: x.index - y.index)
		writer.WriteString("mat")
		writer.WriteInt(len(materials))
		for material in materials:
			material.Write(writer)
		# Faces.
		writer.WriteString("fac")
		self.faces.Write(writer)
		# Weights.
		writer.WriteString("wei")
		writer.WriteInt(len(self.weightnames))
		for name in self.weightnames:
			writer.WriteString(name)
			writer.WriteString(name)
		# Nodes.
		writer.WriteString("nod")
		writer.WriteInt(1)
		self.node.Write(writer)
		# Animations.
		writer.WriteString("ani")
		self.animations.Write(writer)
		# Hairs.
		writer.WriteString("hai")
		self.hairs.Write(writer)

#############################################################################
# Writer.

class LipsWriter:

	# Initializes a new data writer.
	def __init__(self, name):
		self.file = open(name, "wb")

	def Close(self):
		self.file.close()

	# Appends an integer.
	def WriteInt(self, value):
		self.file.write(struct.pack("!I", value))

	# Appends a floating point number.
	def WriteFloat(self, value):
		self.file.write(struct.pack("!f", value))

	# Appends a string.
	def WriteString(self, value):
		self.file.write(value)
		self.file.write(struct.pack('c', '\0'))

#############################################################################
# Main.

name = os.path.splitext(Blender.Get("filename"))[0]
lips = LipsStorage(Blender.Scene.GetCurrent())
writer = LipsWriter(name + ".lmdl")
lips.Write(writer)
writer.Close()
