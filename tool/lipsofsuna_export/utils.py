import re
import bpy, mathutils
from .modifier_mirror import *

class LIUtils:

	@classmethod
	def get_files_for_material(cls, material):
		def getprop(mat, prop):
			try:
				return mat[prop]
			except:
				return ""
		files = getprop(material, 'file').split(',')
		if getprop(material, 'lod') == 'true':
			return [f + 'l.lmdl' for f in files if len(f)]
		else:
			return [f + '.lmdl' for f in files if len(f)]

	@classmethod
	def get_files_for_object(cls, object):
		def getprop(obj, prop):
			try:
				return obj[prop]
			except:
				return ""
		# No files for explicitly disabled objects.
		if getprop(object, 'export') == 'false':
			return []
		# Don't export links or proxies.
		try:
			if object.proxy or object.library:
				return []
			if object.data.library:
				return []
		except:
			pass
		# Otherwise get the list from the file property.
		files = getprop(object, 'file').split(',')
		if getprop(object, 'lod') == 'true':
			return [f + 'l.lmdl' for f in files if len(f)]
		else:
			return [f + '.lmdl' for f in files if len(f)]

	@classmethod
	def object_check_export(cls, object, file, type='NODE'):
		def getprop(object, prop):
			try:
				return object[prop]
			except:
				return ""
		# Don't export links or proxies.
		try:
			if object.proxy or object.library:
				return False
			if object.data.library:
				return False
		except:
			pass
		# Make sure that the filename matches.
		if file not in cls.get_files_for_object(object):
			return False
		# Make sure that the type is a requested one.
		if type == 'NODE':
			return True
		elif type == 'PARTICLE':
			if object.type != 'MESH':
				return False
			if getprop(object, 'shape') != "":
				return False
			return True
		elif type == 'RENDER':
			if object.type != 'MESH':
				return False
			if getprop(object, 'shape') != "" or getprop(object, 'render') == "false":
				return False
			return True
		elif type == 'SHAPE':
			if object.type != 'MESH':
				return False
			if getprop(object, 'shape') == "":
				return False
			return True
		else:
			return False
