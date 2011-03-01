/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtTiles Tiles
 * @{
 */

#include "ext-module.h"

#define SCRIPT_POINTER_MODEL ((void*) -1)

/* @luadoc
 * module "core/tiles"
 * --- Terrain material.
 * -- @name Material
 * -- @class table
 */

/* @luadoc
 * --- Creates a new terrain material.
 * -- @param clss Material class.
 * -- @param args Arguments.
 * -- @return Material.
 */
static void Material_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIVoxMaterial* material;

	/* Allocate material. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_MATERIAL);
	material = livox_material_new ();
	if (material == NULL)
		return;

	/* Insert to the voxel manager. */
	/* We currently rely on materials not being deletable. Even if they are
	   garbage collected, the actual pointers stored to the manager remain
	   until the game ends. */
	material->id = module->voxels->materials->size + 1;
	if (!livox_manager_insert_material (module->voxels, material))
	{
		livox_material_free (material);
		return;
	}

	/* Allocate userdata. */
	data = liscr_data_new (args->script, material, args->clss, NULL);
	if (data == NULL)
	{
		livox_manager_remove_material (module->voxels, material->id);
		return;
	}
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data);
}

/* @luadoc
 * --- Face culling toggle.
 * -- @name Material.cullface
 * -- @class table
 */
static void Material_getter_cullface (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_int (args, (self->material.flags & LIMDL_MATERIAL_FLAG_CULLFACE) != 0);
}
static void Material_setter_cullface (LIScrArgs* args)
{
	int value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		if (value)
			self->material.flags |= LIMDL_MATERIAL_FLAG_CULLFACE;
		else
			self->material.flags &= ~LIMDL_MATERIAL_FLAG_CULLFACE;
	}
}

/* @luadoc
 * --- Diffuse color.
 * -- @name Material.diffuse
 * -- @class table
 */
static void Material_getter_diffuse (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, self->material.diffuse[0]);
	liscr_args_seti_float (args, self->material.diffuse[1]);
	liscr_args_seti_float (args, self->material.diffuse[2]);
	liscr_args_seti_float (args, self->material.diffuse[3]);
}
static void Material_setter_diffuse (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_geti_float (args, 0, self->material.diffuse + 0);
	liscr_args_geti_float (args, 1, self->material.diffuse + 1);
	liscr_args_geti_float (args, 2, self->material.diffuse + 2);
	liscr_args_geti_float (args, 3, self->material.diffuse + 3);
}

/* @luadoc
 * --- Material flags.
 * -- @name Material.flags
 * -- @class table
 */
static void Material_getter_flags (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_int (args, self->flags);
}
static void Material_setter_flags (LIScrArgs* args)
{
	int value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_int (args, 0, &value))
		self->flags = value;
}

/* @luadoc
 * --- Material friction.
 * -- @name Material.friction
 * -- @class table
 */
static void Material_getter_friction (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_float (args, self->friction);
}
static void Material_setter_friction (LIScrArgs* args)
{
	float value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		self->friction = value;
}

/* @luadoc
 * --- Material ID.
 * -- @name Material.id
 * -- @class table
 */
static void Material_getter_id (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_int (args, self->id);
}

/* @luadoc
 * --- Material name.
 * -- @name Material.name
 * -- @class table
 */
static void Material_getter_name (LIScrArgs* args)
{
	liscr_args_seti_string (args, ((LIVoxMaterial*) args->self)->name);
}
static void Material_setter_name (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		livox_material_set_name (args->self, value);
}

/* @luadoc
 * --- Surface shader.
 * -- @name Material.shader
 * -- @class table
 */
static void Material_getter_shader (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_string (args, self->material.shader);
}
static void Material_setter_shader (LIScrArgs* args)
{
	const char* value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_string (args, 0, &value))
		limdl_material_set_shader (&self->material, value);
}

/* @luadoc
 * --- Shininess.
 * -- @name Material.shininess
 * -- @class table
 */
static void Material_getter_shininess (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_float (args, self->material.shininess);
}
static void Material_setter_shininess (LIScrArgs* args)
{
	float value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		self->material.shininess = value;
}

/* @luadoc
 * --- Specular color.
 * -- @name Material.specular
 * -- @class table
 */
static void Material_getter_specular (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, self->material.specular[0]);
	liscr_args_seti_float (args, self->material.specular[1]);
	liscr_args_seti_float (args, self->material.specular[2]);
	liscr_args_seti_float (args, self->material.specular[3]);
}
static void Material_setter_specular (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_geti_float (args, 0, self->material.specular + 0);
	liscr_args_geti_float (args, 1, self->material.specular + 1);
	liscr_args_geti_float (args, 2, self->material.specular + 2);
	liscr_args_geti_float (args, 3, self->material.specular + 3);
}

/* @luadoc
 * --- Texture list.
 * -- @name Material.texture
 * -- @class table
 */
static void Material_getter_texture (LIScrArgs* args)
{
	int i;
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	for (i = 0 ; i < self->material.textures.count ; i++)
		liscr_args_seti_string (args, self->material.textures.array[i].string);
}
static void Material_setter_texture (LIScrArgs* args)
{
	int i;
	const int texflags = LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP | LIMDL_TEXTURE_FLAG_REPEAT;
	const char* value;
	LIVoxMaterial* self = args->self;

	limdl_material_clear_textures (&self->material);
	for (i = 0 ; 1 ; i++)
	{
		if (!liscr_args_geti_string (args, i, &value))
			break;
		limdl_material_append_texture (&self->material,
			LIMDL_TEXTURE_TYPE_IMAGE, texflags, value);
	}
}

/* @luadoc
 * --- Texture scale factor.
 * -- @name Material.texture_scale
 * -- @class table
 */
static void Material_getter_texture_scale (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_float (args, self->texture_scale);
}
static void Material_setter_texture_scale (LIScrArgs* args)
{
	float value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		self->texture_scale = value;
}

/* @luadoc
 * --- Material type.
 * -- @name Material.type
 * -- @class table
 */
static void Material_getter_type (LIScrArgs* args)
{
	LIVoxMaterial* self;

	self = args->self;
	switch (self->type)
	{
		case LIVOX_MATERIAL_TYPE_ROUNDED:
			liscr_args_seti_string (args, "rounded");
			break;
		case LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL:
			liscr_args_seti_string (args, "rounded fractal");
			break;
		default:
			liscr_args_seti_string (args, "cube");
			break;
	}
}
static void Material_setter_type (LIScrArgs* args)
{
	const char* value;
	LIVoxMaterial* self;

	self = args->self;
	if (liscr_args_geti_string (args, 0, &value))
	{
		if (!strcmp (value, "cube"))
			self->type = LIVOX_MATERIAL_TYPE_CUBE;
		else if (!strcmp (value, "rounded"))
			self->type = LIVOX_MATERIAL_TYPE_ROUNDED;
		else if (!strcmp (value, "rounded fractal"))
			self->type = LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL;
	}
}

/*****************************************************************************/

void liext_script_material (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_MATERIAL, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "new", Material_new);
	liscr_class_insert_mvar (self, "cullface", Material_getter_cullface, Material_setter_cullface);
	liscr_class_insert_mvar (self, "diffuse", Material_getter_diffuse, Material_setter_diffuse);
	liscr_class_insert_mvar (self, "flags", Material_getter_flags, Material_setter_flags);
	liscr_class_insert_mvar (self, "friction", Material_getter_friction, Material_setter_friction);
	liscr_class_insert_mvar (self, "id", Material_getter_id, NULL);
	liscr_class_insert_mvar (self, "name", Material_getter_name, Material_setter_name);
	liscr_class_insert_mvar (self, "shader", Material_getter_shader, Material_setter_shader);
	liscr_class_insert_mvar (self, "shininess", Material_getter_shininess, Material_setter_shininess);
	liscr_class_insert_mvar (self, "specular", Material_getter_specular, Material_setter_specular);
	liscr_class_insert_mvar (self, "texture", Material_getter_texture, Material_setter_texture);
	liscr_class_insert_mvar (self, "texture_scale", Material_getter_texture_scale, Material_setter_texture_scale);
	liscr_class_insert_mvar (self, "type", Material_getter_type, Material_setter_type);
}

/** @} */
/** @} */
