#!/usr/bin/python

from __future__ import unicode_literals
import os
import json

def process(d):
	datadir = 'data/'
	datarepo = '../lipsofsuna-data/'

	# Validate the licenses.
	licenses = {}
	for v in data['licenses']:
		if 'name' not in v:
			raise Exception("license with no name")
		if 'url' not in v:
			raise Exception("license with no URL")
		if v['name'] in licenses:
			raise Exception("duplicate license %s" % v['name'])
		licenses[v['name']] = v

	# Validate the authors.
	authors = {}
	for v in data['authors']:
		if 'name' not in v:
			raise Exception("authors with no name")
		if v['name'] in authors:
			raise Exception("duplicate authors %s" % v['name'])
		authors[v['name']] = v

	# Validate the assets.
	for v in data['assets']:
		if 'files' not in v and 'files_original' not in v:
			raise Exception("asset with no files")
		if 'licenses' not in v:
			raise Exception("asset with no licenses")
		if 'authors' not in v:
			raise Exception("asset with no authors")
		for a in v['authors']:
			if a not in authors:
				raise Exception("invalid author %s" % a)
		for l in v['licenses']:
			if l not in licenses:
				raise Exception("invalid license %s" % l)

	# Validate the listed files.
	asset_paths = {}
	asset_names = {}
	print("Duplicate files:")
	for v in data['assets']:
		for path in v.get('files', []):
			if path in asset_paths:
				print(" %s" % path)
			asset_paths[path] = True
			asset_names[os.path.split(path)[1]] = True
	print("")
	print("Non-existing files:")
	for path in asset_paths:
		if not os.path.exists(path):
			print(" %s" % path)
	print("")

	# Validate the real files.
	print("Unlisted files:")
	exts = ['.png', '.dds', '.lmdl', '.svg', '.blend', '.ogg', '.flac']
	for root,dirs,names in os.walk('data/lipsofsuna'):
		for file in names:
			found = False
			for ext in exts:
				if file.endswith(ext):
					found = True
					break
			if found:
				path = os.path.join(root, file)
				if path not in asset_paths:
					print(" %s" % path)
	print("")

	# Validate the listed originals.
	files_orig = {}
	print("Duplicate original files:")
	for v in data['assets']:
		for path in v.get('files_original', []):
			if path in asset_paths:
				print(" %s" % path)
			files_orig[str(path)] = True
	print("")
	print("Non-existing original files:")
	for path in files_orig:
		realpath = os.path.join(datarepo, path)
		if not os.path.exists(realpath):
			print(" %s" % path)
	print("")

	# Validate the real originals.
	print("Unlisted original files:")
	exts = ['.png', '.dds', '.svg', '.blend', '.ogg', '.flac']
	for root,dirs,names in os.walk(datarepo):
		for file in names:
			found = False
			for ext in exts:
				if file.endswith(ext):
					found = True
					break
			if found:
				path = os.path.join(root, file)
				path = path[len(datarepo):]
				if path not in files_orig:
					print(" %s" % path)
	print("")

	# Find all specs.
	spec_files = {}
	for root,dirs,names in os.walk(datadir):
		for file in names:
			if file == '__mod__.json':
				path = os.path.join(root, file)
				try:
					with open(path, 'r') as f:
						mod = json.load(f)
						for spec in mod.get('specs', []):
							spec_path = os.path.join(root, spec + '.json')
							spec_files[spec_path] = True
				except Exception as e:
					print("ERROR: %s: %s" % (path, str(e)))

	# Validate the asset references in the specs.
	print("Non-existing assets referenced by specs:")
	for path in spec_files:
		with open(path, 'r') as f:
			try:
				specs = json.load(f)
				for spec in specs:
					# Model.
					model = spec.get('model')
					if model:
						model_file = model + '.lmdl'
						if model_file not in asset_names:
							print(" %s (%s)" % (model_file, path))
					# Equipment models.
					models = spec.get('equipment_models')
					if models:
						for mdllist in models.values():
							for model in mdllist.values():
								if len(model):
									model_file = model + '.lmdl'
									if model_file not in asset_names:
										print(" %s    (%s)" % (model_file, path))
					# Equipment textures.
					textures = spec.get('equipment_textures')
					if textures:
						for texlist in textures.values():
							for texture in texlist.values():
								if len(texture):
									texture_file = texture + '.png'
									texture_file1 = texture + '.dds'
									if texture_file not in asset_names and texture_file1 not in asset_names:
										print(" %s    (%s)" % (texture_file, path))
			except Exception as e:
				print("ERROR: %s: %s" % (path, str(e)))
	# TODO

	# Check for unused assets.
	# TODO

with open("ASSETS.json", "r") as f:
	data = json.load(f)
	process(data)
