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

def FaceDiffuse(object, mesh, face):
	if mesh.materials and mesh.materials[face.mat]:
		m = mesh.materials[face.mat]
		flags = m.getMode()
		if flags & Blender.Material.Modes['ZTRANSP']:
			return [m.R, m.G, m.B, m.alpha]
		else:
			return [m.R, m.G, m.B, 1.0]
	else:
		return [1.0, 1.0, 1.0, 1.0]

def FaceEmission(object, mesh, face):
	if mesh.materials and mesh.materials[face.mat]:
		m = mesh.materials[face.mat]
		return m.emit
	else:
		return 0.0

def FaceFlags(object, mesh, face):
	flags = 0
	if mesh.materials and mesh.materials[face.mat]:
		mode = mesh.materials[face.mat].getMode()
		if mode & Blender.Material.Modes['ZTRANSP']:
			# Texture and static transparency.
			flags |= 8
	if mesh.faceUV:
		if face.mode & Blender.Mesh.FaceModes['DYNAMIC']:
			# Collisions enabled.
			flags |= 2
		if not (face.mode & Blender.Mesh.FaceModes['TWOSIDE']):
			# Back face culling.
			flags |= 4
		if face.mode & Blender.Mesh.FaceModes['BILLBOARD']:
			# Render as billboard.
			flags |= 1
	else:
		if not (mesh.mode & Blender.Mesh.Modes['TWOSIDED']):
			# Back face culling.
			flags |= 4
		# Collisions enabled.
		flags |= 2
	return flags

def FaceShader(object, mesh, face):
	if mesh.materials and mesh.materials[face.mat]:
		tmp = mesh.materials[face.mat].name.split("::")
		if len(tmp) >= 2:
			return tmp[1]
		return "default"
	else:
		return "default"

def FaceShininess(object, mesh, face):
	if mesh.materials and mesh.materials[face.mat]:
		return mesh.materials[face.mat].hard / 511.0 * 128.0
	else:
		return 0.0

def FaceSpecular(object, mesh, face):
	if mesh.materials and mesh.materials[face.mat]:
		m = mesh.materials[face.mat]
		return [m.specR, m.specG, m.specB, m.spec]
	else:
		return [1.0, 1.0, 1.0, 1.0]

def FaceTextures(object, mesh, face):
	count = 0
	index = 0
	textures = []

	# Use default if no material found.
	if not mesh.materials or not mesh.materials[face.mat]:
		if mesh.faceUV and face.image:
			image = face.image
			name = os.path.basename(image.filename)
			name = os.path.splitext(name)[0]
			flags = Lips.TexFlags.BILINEAR | Lips.TexFlags.MIPMAP | Lips.TexFlags.REPEAT
			textures = [LipsTexture(Lips.TexTypes.IMAGE, flags, 0, 0, name)]
		else:
			textures = [LipsTexture(Lips.TexTypes.NONE, 0, 0, 0, "")]
		return textures

	# Get textures from material.
	bmaterial = mesh.materials[face.mat]
	btextures = bmaterial.getTextures()
	for texture in btextures:
		index = index + 1
		if not texture or not texture.tex:
			# Nil texture.
			textures.append(LipsTexture(Lips.TexTypes.NONE, 0, 0, 0, ""))
			continue
		texture = texture.tex
		if texture.type is Blender.Texture.Types.NONE:
			# None texture
			textures.append(LipsTexture(Lips.TexTypes.NONE, 0, 0, 0, ""))
		elif texture.type is Blender.Texture.Types.IMAGE and texture.image:
			# Image texture
			image = texture.image
			name = os.path.basename(image.filename)
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
			# Envmap texture
			# FIXME: Blender doesn't support querying the Cuberes variable?
			width = PowerOfTwo(128)#texture.cubeRes
			height = width
			flags = Lips.TexFlags.BILINEAR | Lips.TexFlags.REPEAT
			textures.append(LipsTexture(Lips.TexTypes.ENVMAP, flags, width, height, ""))
			count = index
		else:
			# Other texture
			#print("W: Unsupported texture type")
			textures.append(LipsTexture(Lips.TexTypes.NONE, 0, 0, 0, ""))

	# Strip trailing unused textures.
	textures = textures[:count]
	if len(textures) == 0:
		textures = [LipsTexture(Lips.TexTypes.NONE, 0, 0, 0, "")]

	# Override first texture if TEXFACE is set.
	mode = bmaterial.getMode()
	if mode & Blender.Material.Modes['TEXFACE']:
		if mesh.faceUV and face.image:
			image = face.image
			name = os.path.basename(image.filename)
			name = os.path.splitext(name)[0]
			flags = Lips.TexFlags.BILINEAR | Lips.TexFlags.MIPMAP | Lips.TexFlags.REPEAT
			if textures[0].type is Lips.TexTypes.IMAGE:
				flags = textures[0].flags
			textures[0] = LipsTexture(Lips.TexTypes.IMAGE, flags, 0, 0, name)

	return textures

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
	for o in scene.GetObjects():
		if lips_exporter_calls["NodeChild"](scene, object, bone, o):
			if o.type == "Armature":
				nodes.append(LipsNode(LipsNodeType.EMPTY, scene, o, None, parent))
			if o.type == "Empty":
				nodes.append(LipsNode(LipsNodeType.EMPTY, scene, o, None, parent))
			if o.type == "Lamp":
				nodes.append(LipsNode(LipsNodeType.LIGHT, scene, o, None, parent))
	return nodes

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

lips_format_version = 0xFFFFFFFA
lips_animation_timescale = 0.01
lips_minimum_box_size = 0.3
lips_correction_matrix = Euler(-90, 0, 0).toMatrix().resize4x4()
lips_correction_quat = Euler(-90, 0, 0).toQuat()
lips_exporter_calls = \
{ \
	"FaceDiffuse": FaceDiffuse, \
	"FaceEmission": FaceEmission, \
	"FaceFlags": FaceFlags, \
	"FaceShader": FaceShader, \
	"FaceShininess": FaceShininess, \
	"FaceSpecular": FaceSpecular, \
	"FaceTextures": FaceTextures, \
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

	def GetBlendIn(self):
		return lips_animation_timescale * self.strip.blendIn;

	def GetBlendOut(self):
		return lips_animation_timescale * self.strip.blendOut;

	def GetDuration(self):
		return lips_animation_timescale * (self.strip.actionEnd - self.strip.actionStart)

	# \brief Gets the name of the animation.
	def GetName(self):
		return self.action.name

	def Write(self, writer):
		channels = self.action.getChannelNames()
		writer.WriteString(self.GetName())
		writer.WriteInt(len(channels))
		writer.WriteFloat(self.GetDuration())
		writer.WriteFloat(self.GetBlendIn())
		writer.WriteFloat(self.GetBlendOut())
		for channel in channels:
			curves = self.GetCurves(channel)
			writer.WriteString(channel)
			writer.WriteInt(len(curves))
			for curve in curves:
				writer.WriteInt(curve.GetType())
				writer.WriteInt(curve.GetLength())
				for value in curve.GetValues():
					writer.WriteFloat(value)


class LipsArmature:

	# Initializes a new armature.
	def __init__(self):
		self.animations = []

	# \brief Appends animations from a Blender armature object.
	#
	# \param self Armature.
	# \param object Blender armature object.
	# \param armature Blender armature data.
	def AppendArmature(self, object, armature):
		for action in object.actionStrips:
			self.animations.append(LipsAnimation(action))

	# Writes animations.
	#
	# \param self Armature.
	# \param writer Writer.
	def Write(self, writer):
		writer.WriteInt(len(self.animations))
		for animation in self.animations:
			animation.Write(writer)


class LipsCurve:

	# Initializes a new IPO curve from Blender data.
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

	# Returns the number of values in the curve.
	def GetLength(self):
		return self.length

	# Gets the interpolation type of the curve.
	def GetType(self):
		return self.type

	# Gets the nodes of the curve.
	def GetValues(self):
		return self.values

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


class LipsFaceGroup:

	def __init__(self, shader, textures):
		self.flags = 0
		self.start = 0
		self.shader = shader
		self.textures = textures
		self.vertices = []
		self.emission = 0.0
		self.shininess = 0.0
		self.diffuse = [1.0, 1.0, 1.0, 1.0]
		self.specular = [1.0, 1.0, 1.0, 1.0]

	def AppendFace(self, v0, v1, v2):
		self.vertices.append(v0)
		self.vertices.append(v1)
		self.vertices.append(v2)

	def GetEnd(self):
		return self.start + len(self.vertices) / 3

	def GetKey(self):
		key = ""
		for tex in self.textures:
			key = "%s %s" % (key, tex.GetKey())
		return "%d %s %s %s %s %s [%s]" % (self.flags, self.shader, self.emission, self.shininess, self.diffuse, self.specular, key)

	def GetStart(self):
		return self.start

	def SetStart(self, start):
		self.start = start

	def WriteHeader(self, writer):
		writer.WriteInt(self.GetStart())
		writer.WriteInt(self.GetEnd())
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
		writer.WriteInt(len(self.textures))
		writer.WriteString(self.shader)
		for texture in self.textures:
			texture.Write(writer)

	def WriteFaces(self, writer):
		for vertex in self.vertices:
			vertex.WriteCoords(writer)

	def WriteWeights(self, writer):
		for vertex in self.vertices:
			vertex.WriteWeights(writer)


class LipsMesh:

	# \brief Creates an empty mesh exporter.
	def __init__(self):
		self.facegroupindex = {}
		self.facegrouplist = []
		self.weightgroupnamelist = []
		self.weightgrouplist = []
		self.weightgroupdict = {}

	# \brief Appends a face to the mesh.
	#
	# The model format requires normals and texture coordinates to be stored
	# per vertex so we need to create extra vertices for seams and sharp edges.
	# Faces also need to be grouped so that the material and UV texture is
	# constant within each group.
	#
	# \param self Lips mesh.
	# \param object Blender mesh object.
	# \param mesh Blender mesh data. 
	# \param face Blender mesh face.
	# \param conn Face connectivity data for the mesh.
	def AppendFace(self, object, mesh, face, conn):

		v = []
		flags = lips_exporter_calls["FaceFlags"](object, mesh, face)
		shader = lips_exporter_calls["FaceShader"](object, mesh, face)
		textures = lips_exporter_calls["FaceTextures"](object, mesh, face)
		shininess = lips_exporter_calls["FaceShininess"](object, mesh, face)
		diffuse = lips_exporter_calls["FaceDiffuse"](object, mesh, face)
		specular = lips_exporter_calls["FaceSpecular"](object, mesh, face)
		for i in range(len(face.v)):
			co = lips_exporter_calls["VertexCoord"](object, mesh, face, i)
			no = lips_exporter_calls["VertexNormal"](object, mesh, face, i, conn)
			te = lips_exporter_calls["VertexTexcoords"](object, mesh, face, i)
			we = lips_exporter_calls["VertexWeights"](object, mesh, face, i, self.weightgroupnamelist)
			v.append(LipsVertex(co, no, te, we))

		# Create or reuse material group.
		group = LipsFaceGroup(shader, textures)
		group.flags = flags
		group.shininess = shininess
		group.diffuse = diffuse
		group.specular = specular
		key = group.GetKey()
		if key in self.facegroupindex:
			group = self.facegroupindex[key]
		else:
			self.facegrouplist.append(group)
			self.facegroupindex[key] = group

		# Append faces to the group.
		if len(face.v) == 3:
			group.AppendFace(v[0], v[1], v[2])
		elif len(face.v) == 4:
			group.AppendFace(v[0], v[1], v[2])
			group.AppendFace(v[0], v[2], v[3])

	# \brief Appends a mesh.
	#
	# \param self Lips mesh.
	# \param mesh Blender mesh object. 
	def AppendMesh(self, mesh):
		mdata = mesh.getData(0, 1)
		conn = LipsConnectivity(mesh, mdata)

		# Append weight group names.
		for name in mdata.getVertGroupNames():
			if name not in self.weightgroupnamelist:
				self.weightgroupnamelist.append(name)

		# Append face vertices.
		for face in mdata.faces:
			self.AppendFace(mesh, mdata, face, conn)

	def Compile(self, armature):

		# Set face group offsets.
		start = 0
		for facegroup in self.facegrouplist:
			facegroup.SetStart(start)
			start = facegroup.GetEnd()

		# Setup weight groups.
		index = 0
		for name in self.weightgroupnamelist:
			weightgroup = LipsWeightGroup(index, name)
			self.weightgrouplist.append(weightgroup)
			self.weightgroupdict[name] = weightgroup
			index = index + 1

	# \brief Gets the bounding box of the mesh.
	#
	# \return Array of type [xmin, ymin, zmin, xmax, ymax, zmax].
	def GetBounds(self):
		first = 1
		bounds = [0, 0, 0, 0, 0, 0]
		# Calculate box size.
		for group in self.facegrouplist:
			for vertex in group.vertices:
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

	# \brief Gets the mesh flags.
	#
	# \return Flags.
	def GetFlags(self):
		flags = 0
		#if not (self.mesh.mode & Blender.Mesh.Modes.TWOSIDE):
		#	flags |= 1
		return flags

	# \brief Gets the faces of the mesh sorted by texture.
	#
	# \return List of face groups.
	def GetFaceGroups(self):
		return self.facegrouplist

	# Gets the list of textures the mesh uses.
	#
	# The first element of the list is None if there are faces with no textures.
	def GetTextures(self):
		return self.textures

	# Gets the vertices of the mesh.
	def GetVertexCount(self):
		if len(self.facegrouplist) > 0:
			return 3 * self.facegrouplist[len(self.facegrouplist)-1].GetEnd()
		else:
			return 0

	# Gets the names of weight groups of the mesh.
	def GetWeightGroups(self):
		return self.weightgrouplist

	# Writes the mesh.
	def Write(self, writer):
		bounds = self.GetBounds();
		facegroups = self.GetFaceGroups()
		weightgroups = self.GetWeightGroups()

		# Write header.
		writer.WriteInt(self.GetFlags())
		writer.WriteInt(self.GetVertexCount())
		writer.WriteInt(len(facegroups))
		writer.WriteInt(len(weightgroups))
		writer.WriteFloat(bounds[0])
		writer.WriteFloat(bounds[1])
		writer.WriteFloat(bounds[2])
		writer.WriteFloat(bounds[3])
		writer.WriteFloat(bounds[4])
		writer.WriteFloat(bounds[5])

		# Write vertices.
		for facegroup in facegroups:
			facegroup.WriteFaces(writer)

		# Write materials.
		for facegroup in facegroups:
			facegroup.WriteHeader(writer)

		# Write weight groups.
		for weightgroup in weightgroups:
			weightgroup.Write(writer)

		# Write vertex weights.
		for facegroup in facegroups:
			facegroup.WriteWeights(writer)

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
# Scene.

class LipsScene:

	# Initializes a new scene from Blender data.
	def __init__(self, bscene):
		self.scene = bscene

	# \brief Gets the list of armatures in the scene.
	#
	# \param self Scene.
	# \return Array of Blender objects.
	def GetArmatures(self):
		armatures = []
		objects = self.scene.objects
		for obj in objects:
			if obj.type == "Armature" and obj.parent == None:
				armatures.append(obj)
		return armatures

	# \brief Gets the list of meshes in the scene.
	#
	# \param self Scene.
	# \return Array of Blender objects.
	def GetMeshes(self):
		meshes = []
		objects = self.scene.objects
		for obj in objects:
			if obj.type == "Mesh" and obj.parent == None:
				meshes.append(obj)
		return meshes

	# \brief Gets the list of objects in the scene.
	#
	# \param self Scene.
	# \return Array of Blender objects.
	def GetObjects(self):
		return self.scene.objects


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


class LipsExport:

	# Initializes a new exporter.
	def __init__(self, magic, dest, ext):
		self.scene = LipsScene(Blender.Scene.GetCurrent())
		self.mesh = LipsMesh()
		self.armature = LipsArmature()
		self.export_dir = dest
		self.export_ext = ext
		self.export_magic = magic
		self.export_name = os.path.basename(Blender.Get("filename"))
		self.export_name = os.path.splitext(self.export_name)[0]
		self.export_name = os.path.splitext(self.export_name)[0]
		self.export_name = os.path.splitext(self.export_name)[0]

	def Write(self):

		# Append armatures.
		armatures = self.scene.GetArmatures()
		if len(armatures) > 1:
			armatures = [armatures[0]]
			print("W: File contains multiple armature.")
		for armature in armatures:
			self.armature.AppendArmature(armature, armature.getData())

		# Append meshes.
		meshes = self.scene.GetMeshes()
		if len(meshes) == 0:
			print("W: File contains no meshes.")
			return None
		for mesh in meshes:
			self.mesh.AppendMesh(mesh)
		self.mesh.Compile(self.armature)

		# Write data.
		self.writer = LipsWriter(self.export_dir + self.export_name + self.export_ext)
		self.writer.WriteString(self.export_magic)
		self.writer.WriteInt(lips_format_version)
		self.mesh.Write(self.writer)
		self.writer.WriteInt(1)
		node = LipsNode(LipsNodeType.EMPTY, self.scene, None, None, None)
		node.Write(self.writer)
		self.armature.Write(self.writer)
		self.writer.Close()

		# FIXME: Debug.
		node.Debug()

#############################################################################
# Specialized exporter.

exporter = LipsExport("lips/mdl", "", ".lmdl")
exporter.Write()
