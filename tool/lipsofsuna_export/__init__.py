import array, math, os, re, struct, sys
import bpy, mathutils
from .animation import *
from .collision import *
from .copy_shape_keys_operator import *
from .copy_weight_groups_operator import *
from .exporter import *
from .export_finished_dialog import *
from .file import *
from .format import *
from .material import *
from .material_export_settings_operator import *
from .material_property_panel import *
from .mesh import *
from .node import *
from .node_armature import *
from .node_bone import *
from .node_light import *
from .node_root import *
from .object_export_settings_operator import *
from .object_property_panel import *
from .particle import *
from .particles import *
from .particle_system import *
from .shape import *
from .shape_part import *
from .shapekey import *
from .texture import *
from .utils import *
from .vertex import *
from .writer import *

bl_info = {
	"name": "Export Lips of Suna (.lmdl)",
	"author": "Lips of Suna development team",
	"version": (1, 0),
	"blender": (2, 6, 0),
	"api": 35622,
	"location": "File > Export",
	"description": "Lips of Suna (.lmdl)",
	"warning": "",
	"wiki_url": "http://sourceforge.net/apps/trac/lipsofsuna/wiki",
	"tracker_url": "http://sourceforge.net/apps/trac/lipsofsuna/wiki",
	"category": "Import-Export"}

##############################################################################

bpy.utils.register_class(LIExportFinishedDialog)
bpy.utils.register_class(LIMaterialExportSettingsOperator)
bpy.utils.register_class(LIObjectExportSettingsOperator)

def menu_func(self, context):
	self.layout.operator(LIExporter.bl_idname, text="Lips of Suna (.lmdl)")

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
	register()
