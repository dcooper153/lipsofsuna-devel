import re
import bpy, mathutils
from .modifier import *

class LIModifierMirror(LIModifier):

	def __init__(self, obj, mod):
		LIModifier.__init__(self, obj)
		self.mirror_g = mod.use_mirror_vertex_groups
		self.mirror_u = mod.use_mirror_u
		self.mirror_v = mod.use_mirror_v
		self.mirror_x = mod.use_x
		self.vert_merged = {}

	def _add_vertices(self):
		# Add original vertices.
		LIModifier._add_vertices(self)
		# Add mirrored vertices.
		for i in range(0,self.old_num_verts):
			vert = self.verts[i]
			if vert.x > -0.001 and vert.x < 0.001:
				self.vert_newtoold[i] = i
				self.vert_oldtonew[i] = i
				self.vert_merged[i] = True
			else:
				l = len(self.verts)
				self.vert_newtoold[l] = i
				self.vert_oldtonew[i] = l
				self.verts.append(mathutils.Vector((-vert.x, vert.y, vert.z)))

	def _add_faces(self):
		# Add original faces.
		LIModifier._add_faces(self)
		# Add mirrored faces.
		for i in range(0,self.old_num_faces):
			face = self.faces[i]
			if len(face) == 3:
				indices = [face[2], face[1], face[0]]
			else:
				indices = [face[3], face[2], face[1], face[0]]
			for j in range(0,len(face)):
				indices[j] = self.vert_oldtonew[indices[j]]
			l = len(self.faces)
			self.face_newtoold[l] = i
			self.face_oldtonew[i] = l
			self.faces.append(indices)

	def _create_uv_polygons(self):
		LIModifier._create_uv_polygons(self)
		if not self.mirror_u:
			return
		for i in range(self.old_num_faces,len(self.faces)):
			old_face = self.old_mesh.polygons[self.face_newtoold[i]]
			new_face = self.new_mesh.polygons[i]
			for j in range(0,self.old_num_layers):
				for k in range(0,old_face.loop_total):
					old_uv_face = self.old_mesh.uv_layers[j].data[old_face.loop_start + k]
					new_uv_face = self.new_mesh.uv_layers[j].data[new_face.loop_start + old_face.loop_total - k - 1]
					new_uv_face.uv = (1.0-old_uv_face.uv[0], old_uv_face.uv[1])

	def _create_vertices(self):
		LIModifier._create_vertices(self)
		for i in range(0,len(self.verts)):
			old_vert = self.old_mesh.vertices[self.vert_newtoold[i]]
			new_vert = self.new_mesh.vertices[i]
			if i in self.vert_merged:
				n = old_vert.normal.copy()
				n.x = 0
				new_vert.normal = n.normalized()

	def _create_vertex_group_weights(self):
		if not self.mirror_g:
			LIModifier._create_vertex_group_weights(self)
			return
		for i in range(0,len(self.verts)):
			old_vert = self.old_mesh.vertices[self.vert_newtoold[i]]
			new_vert = self.new_mesh.vertices[i]
			for j in range(0,len(old_vert.groups)):
				old_elem = old_vert.groups[j]
				old_group = self.old_obj.vertex_groups[old_elem.group]
				if i != self.vert_newtoold[i]:
					new_group_name = re.sub(r'\.L$', '.R', old_group.name)
					if new_group_name == old_group.name:
						new_group_name = re.sub(r'\.R$', '.L', old_group.name)
					if new_group_name in self.new_obj.vertex_groups:
						new_group = self.new_obj.vertex_groups[new_group_name]
					else:
						new_group = self.new_obj.vertex_groups[old_group.name]
				else:
					new_group = self.new_obj.vertex_groups[old_elem.group]
				new_group.add([i], old_elem.weight, 'REPLACE')
