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
 * \addtogroup LIExtVoxel Voxel
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.Tiles"
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
	data = liscr_data_new (args->script, material, LIEXT_SCRIPT_MATERIAL, NULL);
	if (data == NULL)
	{
		livox_manager_remove_material (module->voxels, material->id);
		return;
	}
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Top surface diffuse color.
 * -- @name Material.diffuse0
 * -- @class table
 */
static void Material_getter_diffuse0 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, self->mat_top.diffuse[0]);
	liscr_args_seti_float (args, self->mat_top.diffuse[1]);
	liscr_args_seti_float (args, self->mat_top.diffuse[2]);
	liscr_args_seti_float (args, self->mat_top.diffuse[3]);
}
static void Material_setter_diffuse0 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_geti_float (args, 0, self->mat_top.diffuse + 0);
	liscr_args_geti_float (args, 1, self->mat_top.diffuse + 1);
	liscr_args_geti_float (args, 2, self->mat_top.diffuse + 2);
	liscr_args_geti_float (args, 3, self->mat_top.diffuse + 3);
}

/* @luadoc
 * --- Side surface diffuse color.
 * -- @name Material.diffuse1
 * -- @class table
 */
static void Material_getter_diffuse1 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, self->mat_side.diffuse[0]);
	liscr_args_seti_float (args, self->mat_side.diffuse[1]);
	liscr_args_seti_float (args, self->mat_side.diffuse[2]);
	liscr_args_seti_float (args, self->mat_side.diffuse[3]);
}
static void Material_setter_diffuse1 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_geti_float (args, 0, self->mat_side.diffuse + 0);
	liscr_args_geti_float (args, 1, self->mat_side.diffuse + 1);
	liscr_args_geti_float (args, 2, self->mat_side.diffuse + 2);
	liscr_args_geti_float (args, 3, self->mat_side.diffuse + 3);
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
 * --- Material model.
 * -- @name Material.model
 * -- @class table
 */
static void Material_getter_model (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	if (self->model)
		liscr_args_seti_data (args, self->model->script);
}
static void Material_setter_model (LIScrArgs* args)
{
	LIScrData* data = NULL;
	LIVoxMaterial* self = args->self;

	liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &data);
	if (self->model != NULL)
		liscr_data_unref (self->model->script, args->data);
	if (data != NULL)
		livox_material_set_model (self, data->data);
	else
		livox_material_set_model (self, NULL);
	if (self->model != NULL)
		liscr_data_ref (self->model->script, args->data);
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
 * --- Top surface shader.
 * -- @name Material.shader0
 * -- @class table
 */
static void Material_getter_shader0 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_string (args, self->mat_top.shader);
}
static void Material_setter_shader0 (LIScrArgs* args)
{
	const char* value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_string (args, 0, &value))
		limdl_material_set_shader (&self->mat_top, value);
}

/* @luadoc
 * --- Side surface shader.
 * -- @name Material.shader1
 * -- @class table
 */
static void Material_getter_shader1 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_string (args, self->mat_side.shader);
}
static void Material_setter_shader1 (LIScrArgs* args)
{
	const char* value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_string (args, 0, &value))
		limdl_material_set_shader (&self->mat_side, value);
}

/* @luadoc
 * --- Top surface shininess.
 * -- @name Material.shininess0
 * -- @class table
 */
static void Material_getter_shininess0 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_float (args, self->mat_top.shininess);
}
static void Material_setter_shininess0 (LIScrArgs* args)
{
	float value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		self->mat_top.shininess = value;
}

/* @luadoc
 * --- Side surface shininess.
 * -- @name Material.shininess1
 * -- @class table
 */
static void Material_getter_shininess1 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_seti_float (args, self->mat_side.shininess);
}
static void Material_setter_shininess1 (LIScrArgs* args)
{
	float value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_float (args, 0, &value))
		self->mat_side.shininess = value;
}

/* @luadoc
 * --- Top surface specular color.
 * -- @name Material.specular0
 * -- @class table
 */
static void Material_getter_specular0 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, self->mat_top.specular[0]);
	liscr_args_seti_float (args, self->mat_top.specular[1]);
	liscr_args_seti_float (args, self->mat_top.specular[2]);
	liscr_args_seti_float (args, self->mat_top.specular[3]);
}
static void Material_setter_specular0 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_geti_float (args, 0, self->mat_top.specular + 0);
	liscr_args_geti_float (args, 1, self->mat_top.specular + 1);
	liscr_args_geti_float (args, 2, self->mat_top.specular + 2);
	liscr_args_geti_float (args, 3, self->mat_top.specular + 3);
}

/* @luadoc
 * --- Side surface specular color.
 * -- @name Material.specular1
 * -- @class table
 */
static void Material_getter_specular1 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, self->mat_side.specular[0]);
	liscr_args_seti_float (args, self->mat_side.specular[1]);
	liscr_args_seti_float (args, self->mat_side.specular[2]);
	liscr_args_seti_float (args, self->mat_side.specular[3]);
}
static void Material_setter_specular1 (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	liscr_args_geti_float (args, 0, self->mat_side.specular + 0);
	liscr_args_geti_float (args, 1, self->mat_side.specular + 1);
	liscr_args_geti_float (args, 2, self->mat_side.specular + 2);
	liscr_args_geti_float (args, 3, self->mat_side.specular + 3);
}

/* @luadoc
 * --- Top surface textures.
 * -- @name Material.texture0
 * -- @class table
 */
static void Material_getter_texture0 (LIScrArgs* args)
{
	int i;
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	for (i = 0 ; i < self->mat_top.textures.count ; i++)
		liscr_args_seti_string (args, self->mat_top.textures.array[i].string);
}
static void Material_setter_texture0 (LIScrArgs* args)
{
	int i;
	const int texflags = LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP | LIMDL_TEXTURE_FLAG_REPEAT;
	const char* value;
	LIVoxMaterial* self = args->self;

	limdl_material_clear_textures (&self->mat_top);
	for (i = 0 ; 1 ; i++)
	{
		if (!liscr_args_geti_string (args, i, &value))
			break;
		limdl_material_append_texture (&self->mat_top,
			LIMDL_TEXTURE_TYPE_IMAGE, texflags, value);
	}
}

/* @luadoc
 * --- Side surface textures.
 * -- @name Material.texture1
 * -- @class table
 */
static void Material_getter_texture1 (LIScrArgs* args)
{
	int i;
	LIVoxMaterial* self = args->self;

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	for (i = 0 ; i < self->mat_side.textures.count ; i++)
		liscr_args_seti_string (args, self->mat_side.textures.array[i].string);
}
static void Material_setter_texture1 (LIScrArgs* args)
{
	int i;
	const int texflags = LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP | LIMDL_TEXTURE_FLAG_REPEAT;
	const char* value;
	LIVoxMaterial* self = args->self;

	limdl_material_clear_textures (&self->mat_side);
	for (i = 0 ; 1 ; i++)
	{
		if (!liscr_args_geti_string (args, i, &value))
			break;
		limdl_material_append_texture (&self->mat_side,
			LIMDL_TEXTURE_TYPE_IMAGE, texflags, value);
	}
}

/* @luadoc
 * --- Material type.
 * -- @name Material.type
 * -- @class table
 */
static void Material_getter_type (LIScrArgs* args)
{
	LIVoxMaterial* self = args->self;

	if (self->type == LIVOX_MATERIAL_TYPE_HEIGHT)
		liscr_args_seti_string (args, "height");
	else
		liscr_args_seti_string (args, "tile");
}
static void Material_setter_type (LIScrArgs* args)
{
	const char* value;
	LIVoxMaterial* self = args->self;

	if (liscr_args_geti_string (args, 0, &value))
	{
		if (!strcmp (value, "height"))
			self->type = LIVOX_MATERIAL_TYPE_HEIGHT;
		else if (!strcmp (value, "tile"))
			self->type = LIVOX_MATERIAL_TYPE_TILE;
	}
}

/*****************************************************************************/

void liext_script_material (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_MATERIAL, data);
	liscr_class_inherit (self, liscr_script_class, NULL);
	liscr_class_insert_cfunc (self, "new", Material_new);
	liscr_class_insert_mvar (self, "diffuse0", Material_getter_diffuse0, Material_setter_diffuse0);
	liscr_class_insert_mvar (self, "diffuse1", Material_getter_diffuse1, Material_setter_diffuse1);
	liscr_class_insert_mvar (self, "flags", Material_getter_flags, Material_setter_flags);
	liscr_class_insert_mvar (self, "friction", Material_getter_friction, Material_setter_friction);
	liscr_class_insert_mvar (self, "id", Material_getter_id, NULL);
	liscr_class_insert_mvar (self, "name", Material_getter_name, Material_setter_name);
	liscr_class_insert_mvar (self, "model", Material_getter_model, Material_setter_model);
	liscr_class_insert_mvar (self, "shader0", Material_getter_shader0, Material_setter_shader0);
	liscr_class_insert_mvar (self, "shader1", Material_getter_shader1, Material_setter_shader1);
	liscr_class_insert_mvar (self, "shininess0", Material_getter_shininess0, Material_setter_shininess0);
	liscr_class_insert_mvar (self, "shininess1", Material_getter_shininess1, Material_setter_shininess1);
	liscr_class_insert_mvar (self, "specular0", Material_getter_specular0, Material_setter_specular0);
	liscr_class_insert_mvar (self, "specular1", Material_getter_specular1, Material_setter_specular1);
	liscr_class_insert_mvar (self, "texture0", Material_getter_texture0, Material_setter_texture0);
	liscr_class_insert_mvar (self, "texture1", Material_getter_texture1, Material_setter_texture1);
	liscr_class_insert_mvar (self, "type", Material_getter_type, Material_setter_type);
}

/** @} */
/** @} */
