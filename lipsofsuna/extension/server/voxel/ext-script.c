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

#include <lipsofsuna/main.h>
#include <lipsofsuna/network.h>
#include <lipsofsuna/server.h>
#include "ext-listener.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Voxel"
 * ---
 * -- Use dynamic voxel terrain.
 * -- @name Voxel
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Material friction.
 * --
 * -- @name Material.friction
 * -- @class table
 */
static void Material_getter_friction (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIVoxMaterial*) args->self)->friction);
}

/* @luadoc
 * ---
 * -- Material model.
 * --
 * -- @name Material.model
 * -- @class table
 */
static void Material_getter_model (LIScrArgs* args)
{
	liscr_args_seti_string (args, ((LIVoxMaterial*) args->self)->model);
}

/* @luadoc
 * ---
 * -- Material name.
 * --
 * -- @name Material.name
 * -- @class table
 */
static void Material_getter_name (LIScrArgs* args)
{
	liscr_args_seti_string (args, ((LIVoxMaterial*) args->self)->name);
}

/*****************************************************************************/

/* @luadoc
 * ---
 * -- Creates a new tile.
 * --
 * -- @param self Tile class.
 * -- @param args Arguments.
 * -- @return New tile.
 * function Tile.new(self, args)
 */
static void Tile_new (LIScrArgs* args)
{
	LIScrData* data;

	/* Allocate userdata. */
	data = liscr_data_new_alloc (args->script, sizeof (LIVoxVoxel), LIEXT_SCRIPT_TILE);
	if (data == NULL)
		return;
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Damage counter of the tile.
 * --
 * -- @name Tile.damage
 * -- @class table
 */
static void Tile_getter_damage (LIScrArgs* args)
{
	liscr_args_seti_int (args, ((LIVoxVoxel*) args->self)->damage);
}
static void Tile_setter_damage (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
	{
		if (value < 0x00) value = 0;
		if (value > 0xFF) value = 0xFF;
		((LIVoxVoxel*) args->self)->damage = value;
	}
}

/* @luadoc
 * ---
 * -- Rotation of the tile.
 * --
 * -- @name Tile.rotation
 * -- @class table
 */
static void Tile_getter_rotation (LIScrArgs* args)
{
	liscr_args_seti_int (args, ((LIVoxVoxel*) args->self)->rotation);
}
static void Tile_setter_rotation (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
	{
		if (value <  0) value = 0;
		if (value > 24) value = 24;
		((LIVoxVoxel*) args->self)->rotation = value;
	}
}

/* @luadoc
 * ---
 * -- Terrain type of the tile.
 * --
 * -- @name Tile.terrain
 * -- @class table
 */
static void Tile_getter_terrain (LIScrArgs* args)
{
	liscr_args_seti_int (args, ((LIVoxVoxel*) args->self)->type);
}
static void Tile_setter_terrain (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
	{
		if (value < 0x0000) value = 0;
		if (value > 0xFFFF) value = 0xFFFF;
		((LIVoxVoxel*) args->self)->type = value;
	}
}

/*****************************************************************************/

/* @luadoc
 * ---
 * -- Erases a voxel near the given point.
 * --
 * -- Arguments:
 * -- point: Position vector. (required)
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return True if terrain was erased.
 * function Voxel.erase(self, args)
 */
static void Voxel_erase (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector point;

	if (liscr_args_gets_vector (args, "point", &point))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		liscr_args_seti_bool (args, livox_manager_erase_voxel (module->voxels, &point));
	}
}

/* @luadoc
 * ---
 * -- Finds information on a material.
 * --
 * -- Arguments:
 * -- id: Material ID.
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return Material or nil.
 */
static void Voxel_find_material (LIScrArgs* args)
{
	int id;
	LIExtModule* module;
	LIScrData* data;
	LIVoxMaterial* material;

	if (liscr_args_gets_int (args, "id", &id))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		material = livox_manager_find_material (module->voxels, id);
		if (material == NULL)
			return;
		material = livox_material_new_copy (material);
		if (material == NULL)
			return;
		data = liscr_data_new (args->script, material, LIEXT_SCRIPT_MATERIAL, livox_material_free);
		if (data == NULL)
		{
			livox_material_free (material);
			return;
		}
		liscr_args_seti_data (args, data);
		liscr_data_unref (data, NULL);
	}
}

/* @luadoc
 * ---
 * -- Finds the voxel nearest to the given point.
 * --
 * -- Arguments:
 * -- match: Tiles to search for. ("all"/"empty"/"full")
 * -- point: Position vector. (required)
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return Tile and vector, or nil.
 * function Voxel.find_voxel(self, args)
 */
static void Voxel_find_voxel (LIScrArgs* args)
{
	int flags = LIVOX_FIND_ALL;
	const char* tmp;
	LIExtModule* module;
	LIMatVector point;
	LIMatVector result;
	LIScrData* data;
	LIVoxVoxel* voxel;

	if (liscr_args_gets_vector (args, "point", &point))
	{
		/* Search mode. */
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		if (liscr_args_gets_string (args, "match", &tmp))
		{
			if (!strcmp (tmp, "all")) flags = LIVOX_FIND_ALL;
			else if (!strcmp (tmp, "empty")) flags = LIVOX_FIND_EMPTY;
			else if (!strcmp (tmp, "full")) flags = LIVOX_FIND_FULL;
		}

		/* Find voxel. */
		voxel = livox_manager_find_voxel (module->voxels, flags, &point, &result);
		if (voxel == NULL)
			return;

		/* Return values. */
		data = liscr_data_new_alloc (args->script, sizeof (LIVoxVoxel), LIEXT_SCRIPT_TILE);
		if (data == NULL)
			return;
		*((LIVoxVoxel*) data->data) = *voxel;
		liscr_args_seti_data (args, data);
		liscr_args_seti_vector (args, &result);
		liscr_data_unref (data, NULL);
	}
}

/* @luadoc
 * ---
 * -- Inserts a voxel near the given point.
 * --
 * -- Arguments:
 * -- point: Position vector. (required)
 * -- tile: Tile. (required)
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return True if terrain was filled.
 * function Voxel.insert(self, args)
 */
static void Voxel_insert (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector point;
	LIScrData* voxel;

	if (liscr_args_gets_vector (args, "point", &point) &&
	    liscr_args_gets_data (args, "tile", LIEXT_SCRIPT_TILE, &voxel))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		liscr_args_seti_bool (args, livox_manager_insert_voxel (module->voxels, &point, voxel->data));
	}
}

/* @luadoc
 * ---
 * -- Replaces the voxel near the given point.
 * --
 * -- Arguments:
 * -- point: Position vector. (required)
 * -- tile: Tile. (required)
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return True if terrain was replaced.
 * function Voxel.replace(self, args)
 */
static void Voxel_replace (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector point;
	LIScrData* voxel;

	if (liscr_args_gets_vector (args, "point", &point) &&
	    liscr_args_gets_data (args, "tile", LIEXT_SCRIPT_TILE, &voxel))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		liscr_args_seti_bool (args, livox_manager_replace_voxel (module->voxels, &point, voxel->data));
	}
}


/* @luadoc
 * ---
 * -- Rotates a voxel near the given point.
 * --
 * -- Arguments:
 * -- axis: Axis of rotation ("x"/"y"/"z")
 * -- point: Position vector. (required)
 * -- step: Number of steps to rotate.
 * --
 * -- @param self Voxel class.
 * -- @param args Argumennts.
 * -- @return True if a voxel was rotated.
 * function Voxel.rotate(self, args)
 */
static void Voxel_rotate (LIScrArgs* args)
{
	int step = 1;
	int axis = 0;
	const char* tmp;
	LIExtModule* module;
	LIMatVector point;

	if (liscr_args_gets_vector (args, "point", &point))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		if (liscr_args_gets_int (args, "step", &step))
			step %= 4;
		if (liscr_args_gets_string (args, "axis", &tmp))
		{
			if (!strcmp (tmp, "x")) axis = 0;
			else if (!strcmp (tmp, "y")) axis = 1;
			else if (!strcmp (tmp, "z")) axis = 2;
		}
		liscr_args_seti_bool (args, livox_manager_rotate_voxel (module->voxels, &point, axis, step));
	}
}

/* @luadoc
 * ---
 * -- Saves the terrain of the currently loaded sectors.
 * --
 * -- @param self Voxel class.
 * function Voxel.save(self)
 */
static void Voxel_save (LIScrArgs* args)
{
	LIExtModule* module;
	LISerServer* server;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	server = limai_program_find_component (module->program, "server");
	if (server != NULL)
	{
		if (!liext_module_write (module, server->sql))
			lisys_error_report ();
	}
}

/* @luadoc
 * ---
 * -- Fill type for empty sectors.
 * -- @name Server.time
 * -- @class table
 */
static void Voxel_getter_fill (LIScrArgs* args)
{
	LIExtModule* self;
	LIScrData* voxel;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (self->voxels->fill)
	{
		voxel = liscr_data_new_alloc (args->script, sizeof (LIVoxVoxel), LIEXT_SCRIPT_TILE);
		if (voxel != NULL)
		{
			livox_voxel_init (voxel->data, self->voxels->fill);
			liscr_args_seti_data (args, voxel);
			liscr_data_unref (voxel, NULL);
		}
	}
}
static void Voxel_setter_fill (LIScrArgs* args)
{
	LIExtModule* self;
	LIScrData* voxel;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (liscr_args_geti_data (args, 0, LIEXT_SCRIPT_TILE, &voxel))
		livox_manager_set_fill (self->voxels, ((LIVoxVoxel*) voxel->data)->type);
	else
		livox_manager_set_fill (self->voxels, 0);
}

/*****************************************************************************/

void
liext_script_material (LIScrClass* self,
                       void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_MATERIAL, data);
	liscr_class_insert_mvar (self, "friction", Material_getter_friction, NULL);
	liscr_class_insert_mvar (self, "name", Material_getter_name, NULL);
	liscr_class_insert_mvar (self, "model", Material_getter_model, NULL);
}

void
liext_script_tile (LIScrClass* self,
                 void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_TILE, data);
	liscr_class_insert_cfunc (self, "new", Tile_new);
	liscr_class_insert_mvar (self, "damage", Tile_getter_damage, Tile_setter_damage);
	liscr_class_insert_mvar (self, "rotation", Tile_getter_rotation, Tile_setter_rotation);
	liscr_class_insert_mvar (self, "terrain", Tile_getter_terrain, Tile_setter_terrain);
}

void
liext_script_voxel (LIScrClass* self,
                  void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_VOXEL, data);
	liscr_class_insert_cfunc (self, "erase", Voxel_erase);
	liscr_class_insert_cfunc (self, "find_material", Voxel_find_material);
	liscr_class_insert_cfunc (self, "find_voxel", Voxel_find_voxel);
	liscr_class_insert_cfunc (self, "insert", Voxel_insert);
	liscr_class_insert_cfunc (self, "replace", Voxel_replace);
	liscr_class_insert_cfunc (self, "rotate", Voxel_rotate);
	liscr_class_insert_cfunc (self, "save", Voxel_save);
	liscr_class_insert_cvar (self, "fill", Voxel_getter_fill, Voxel_setter_fill);
}

/** @} */
/** @} */
