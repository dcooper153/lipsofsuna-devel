#!/usr/bin/python3

from __future__ import unicode_literals
import os
import json
import struct

class Model(object):

	def __init__(self, path):
		self.path = path
		self.textures = {}
		with open(path, 'rb') as file:
			self.load_file(file)

	def load_file(self, file):
		while True:
			# Read the block.
			name = self.__read_str(file)
			length = self.__read_int(file)
			if not length:
				break
			# Read materials.
			start = file.tell()
			if name == 'mat':
				matn = self.__read_int(file)
				for i in range(matn):
					flag = self.__read_int(file)
					emit = self.__read_flt(file)
					shin = self.__read_flt(file)
					diff = [self.__read_flt(file) for i in range(4)]
					spec = [self.__read_flt(file) for i in range(4)]
					stra = [self.__read_flt(file) for i in range(3)]
					texn = self.__read_int(file)
					shad = self.__read_str(file)
					mate = ''
					if flag & 0x10:
						mate = self.__read_str(file)
					for j in range(texn):
						ttype = self.__read_int(file)
						tflag = self.__read_int(file)
						twidt = self.__read_int(file)
						theig = self.__read_int(file)
						tname = self.__read_str(file)
						self.textures[tname] = True
			file.seek(start + length)

	def __read_block(self, file):
		return (name, data)

	def __read_int(self, file):
		data = file.read(4)
		if len(data) < 4:
			return
		i, = struct.unpack('>i', data)
		return i

	def __read_flt(self, file):
		data = file.read(4)
		if len(data) < 4:
			return
		i, = struct.unpack('>f', data)
		return i

	def __read_str(self, file):
		s = []
		for i in range(0,512):
			c = file.read(1).decode('utf8')
			if c == '\0':
				break
			s.append(c)
		s = ''.join(s)
		if len(s) >= 512:
			raise Exception("invalid string")
		return s

class OgreScript(object):

	def __init__(self, type, path):
		self.type = type
		self.path = path
		self.textures = {}
		with open(path, 'r') as file:
			self.load_file(file)

	def load_file(self, file):
		for line in file:
			line = line.strip()
			tags = [t for t in line.split(' ') if t != '']
			if len(tags) > 1:
				if tags[0] == 'texture':
					self.textures[tags[1]] = True
				elif tags[0] == 'cubic_texture':
					for t in tags[1:]:
						self.textures[t] = True

class Specs(object):

	def __init__(self, path):
		self.path = path
		self.models = {}
		self.sounds = {}
		self.textures = {}
		with open(path, 'r') as file:
			self.data = json.load(file)
			self.load_json(self.data)

	def load_json(self, data):
		for spec in data:
			# Model.
			model = spec.get('model')
			if model and not model.startswith('#'):
				self.models[model] = True
			# Models.
			for slot,model in spec.get('models', {}).items():
				if not model.startswith('#'):
					self.models[model] = True
			# Equipment models.
			models = spec.get('equipment_models', {})
			for mdllist in models.values():
				for model in mdllist.values():
					if len(model) and not model.startswith('#'):
						self.models[model] = True
			# Equipment textures.
			textures = spec.get('equipment_textures', {})
			for texlist in textures.values():
				for texture in texlist.values():
					if len(texture):
						self.textures[texture] = True
			# Customization textures.
			textures = spec.get('eye_styles', {})
			for texture in textures.values():
				if len(texture):
					self.textures[texture] = True
			# Sound effects.
			if spec['type'] == 'Effectspec':
				sound = spec.get('sound')
				if sound:
					self.sounds[sound] = True
			if spec['type'] == 'MusicSpec':
				sound = spec.get('file')
				if sound:
					self.sounds[sound] = True

class Assets(object):

	def __init__(self, datarepo):

		self.datarepo = datarepo

		self.licenses = {}
		self.authors = {}

		self.json_data_names = {}
		self.json_data_paths = {}
		self.json_orig_names = {}
		self.json_orig_paths = {}

		self.disk_data_names = {}
		self.disk_data_paths = {}
		self.disk_orig_names = {}
		self.disk_orig_paths = {}

		self.specs = {}
		self.models = {}
		self.ogre_scripts = {}

	def load_licenses(self, data):
		for v in data['licenses']:
			if 'name' not in v:
				raise Exception("license with no name")
			if 'url' not in v:
				raise Exception("license with no URL")
			if v['name'] in self.licenses:
				raise Exception("duplicate license %s" % v['name'])
			self.licenses[v['name']] = v

	def load_authors(self, data):
		for v in data['authors']:
			if 'name' not in v:
				raise Exception("author with no name")
			if v['name'] in self.authors:
				raise Exception("duplicate author %s" % v['name'])
			self.authors[v['name']] = v

	def validate_assets(self, data):
		for v in data['assets']:
			if 'files' not in v and 'files_original' not in v:
				raise Exception("asset with no files")
			if 'licenses' not in v:
				raise Exception("asset with no licenses")
			if 'authors' not in v:
				raise Exception("asset with no authors")
			for a in v['authors']:
				if a not in self.authors:
					raise Exception("invalid author %s" % a)
			for l in v['licenses']:
				if l not in self.licenses:
					raise Exception("invalid license %s" % l)

	def parse_json_files(self, data):
		for v in data['assets']:
			for path in v.get('files', []):
				name = os.path.split(path)[1]
				self.json_data_names[name] = self.json_data_names.get(name, 0) + 1
				self.json_data_paths[path] = self.json_data_paths.get(path, 0) + 1
			for path in v.get('files_original', []):
				name = os.path.split(path)[1]
				self.json_orig_names[name] = self.json_orig_names.get(name, 0) + 1
				self.json_orig_paths[path] = self.json_orig_paths.get(path, 0) + 1

	def parse_disk_files(self):
		# Find data files.
		exts = ['.png', '.dds', '.lmdl', '.svg', '.blend', '.ogg', '.flac']
		for root,dirs,names in os.walk('data/lipsofsuna'):
			for name in names:
				for ext in exts:
					if name.endswith(ext):
						path = os.path.join(root, name)
						self.disk_data_names[name] = True
						self.disk_data_paths[path] = True
						break
		# Find original files.
		exts = ['.png', '.dds', '.svg', '.blend', '.ogg', '.flac']
		for root,dirs,names in os.walk(self.datarepo):
			for name in names:
				for ext in exts:
					if name.endswith(ext):
						path = os.path.join(root, name)
						path = path[len(self.datarepo):]
						self.disk_orig_names[name] = True
						self.disk_orig_paths[path] = True
						break
		# Load compiled models.
		for path in self.disk_data_paths:
			if path.endswith('.lmdl'):
				try:
					model = Model(path)
					self.models[path] = model
				except Exception as e:
					print("ERROR: %s: %s" % (path, str(e)))
		# Load Ogre scripts.
		exts = ['.material']
		for root,dirs,names in os.walk('data/lipsofsuna'):
			for name in names:
				for ext in exts:
					if name.endswith(ext):
						path = os.path.join(root, name)
						self.ogre_scripts[path] = OgreScript(ext, path)
						break

	def parse_disk_mods(self):
		for root,dirs,names in os.walk('data'):
			for file in names:
				if file == '__mod__.json':
					path = os.path.join(root, file)
					try:
						with open(path, 'r') as f:
							mod = json.load(f)
							for spec in mod.get('specs', []):
								path1 = os.path.join(root, spec + '.json')
								try:
									self.specs[path1] = Specs(path1)
								except Exception as e:
									print("ERROR: %s: %s" % (path1, str(e)))
					except Exception as e:
						print("ERROR: %s: %s" % (path, str(e)))

	def get_duplicate_data_paths(self):
		res = {}
		for path,count in self.json_data_paths.items():
			if count > 1:
				res[path] = True
		return sorted([k for k in res])

	def get_duplicate_orig_paths(self):
		res = {}
		for path,count in self.json_orig_paths.items():
			if count > 1:
				res[path] = True
		return sorted([k for k in res])

	def get_missing_data_paths(self):
		res = {}
		for path in self.json_data_paths:
			if not os.path.exists(path):
				res[path] = True
		return sorted([k for k in res])

	def get_missing_orig_paths(self):
		res = {}
		for path in self.json_orig_paths:
			realpath = os.path.join(self.datarepo, path)
			if not os.path.exists(realpath):
				res[path] = True
		return sorted([k for k in res])

	def get_unlisted_data_paths(self):
		res = {}
		for path in self.disk_data_paths:
			if path not in self.json_data_paths:
				res[path] = True
		return sorted([k for k in res])

	def get_unlisted_orig_paths(self):
		res = {}
		for path in self.disk_orig_paths:
			if path not in self.json_orig_paths:
				res[path] = True
		return sorted([k for k in res])

	def get_unused_data_names(self):
		used = {}
		# Check for files referenced by specs.
		for path,spec in self.specs.items():
			for name in spec.models:
				name1 = name + '.lmdl'
				if name1 in self.disk_data_names:
					used[name1] = True
			for name in spec.textures:
				name1 = name + '.png'
				name2 = name + '.dds'
				if name1 in self.disk_data_names:
					used[name1] = True
				elif name2 in self.disk_data_names:
					used[name2] = True
			for name in spec.sounds:
				name1 = name + '.ogg'
				name2 = name + '.flac'
				if name1 in self.disk_data_names:
					used[name1] = True
				elif name2 in self.disk_data_names:
					used[name2] = True
		# Check for textures referenced by model files.
		for path,model in self.models.items():
			for name in model.textures:
				name1 = name + '.png'
				name2 = name + '.dds'
				if name1 in self.disk_data_names:
					used[name1] = True
				elif name2 in self.disk_data_names:
					used[name2] = True
		# Check for textures referenced by Ogre scripts.
		for path,script in self.ogre_scripts.items():
			for name in script.textures:
				if name in self.disk_data_names:
					used[name] = True
		# Find unreferenced files.
		res = {}
		for name in self.disk_data_names:
			if name not in used:
				res[name] = True
		return sorted([k for k in res])

	def print_errors(self):
		print("Duplicate files:")
		for path in self.get_duplicate_data_paths():
			print(" %s" % k)
		print("")
		print("Non-existing files:")
		for path in self.get_missing_data_paths():
			print(" %s" % path)
		print("")
		print("Unlisted files:")
		for path in self.get_unlisted_data_paths():
			print(" %s" % path)
		print("")
		print("Duplicate originals:")
		for path in self.get_duplicate_orig_paths():
			print(" %s" % path)
		print("")
		print("Non-existing originals:")
		for path in self.get_missing_orig_paths():
			print(" %s" % path)
		print("")
		print("Unlisted originals:")
		for path in self.get_unlisted_orig_paths():
			print(" %s" % path)
		print("")
		print("Potentially unused files:")
		for path in self.get_unused_data_names():
			print(" %s" % path)
		print("")

def process(d):
	datadir = 'data/'
	datarepo = '../lipsofsuna-data/'

	a = Assets(datarepo)
	a.load_licenses(d)
	a.load_authors(d)
	a.validate_assets(d)
	a.parse_json_files(d)
	a.parse_disk_files()
	a.parse_disk_mods()
	a.print_errors()

	# Find all specs.
	specs = sorted([(k,v) for k,v in a.specs.items()])
	print("Non-existing assets referenced by specs:")
	for path,spec in specs:
		for model in spec.models:
			model_file = model + '.lmdl'
			if model_file not in a.disk_data_names:
				print(" %s (%s)" % (model_file, path))
		for texture in spec.textures:
			texture_file = texture + '.png'
			texture_file1 = texture + '.dds'
			if texture_file not in a.disk_data_names and texture_file1 not in a.disk_data_names:
				print(" %s (%s)" % (texture_file, path))

	# Check for unused assets.
	# TODO

with open("ASSETS.json", "r") as f:
	data = json.load(f)
	process(data)
