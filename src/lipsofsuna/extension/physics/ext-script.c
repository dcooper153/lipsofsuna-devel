/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExtPhysics Physics
 * @{
 */

#include "ext-module.h"
#include "lipsofsuna/extension/physics-object/ext-module.h"

static int private_build_ignore_list (
	LIExtPhysicsModule*  module,
	LIScrArgs*    args,
	int           index,
	LIPhyObject** result,
	int           limit)
{
	int i;
	int count = 0;
	LIScrData* data;

	/* Add an individual object to the ignore list. */
	if (liscr_args_geti_data (args, index, LIEXT_SCRIPT_PHYSICS_OBJECT, &data))
	{
		result[0] = liscr_data_get_data (data);
		if (result[0] != NULL)
			return 1;
		return 0;
	}

	/* Add multiple objects to the ignore list. */
	if (liscr_args_geti_table (args, index))
	{
		for (i = 1 ; i <= limit ; i++)
		{
			lua_pushnumber (args->lua, i);
			lua_gettable (args->lua, -2);
			data = liscr_isdata (args->lua, -1, LIEXT_SCRIPT_PHYSICS_OBJECT);
			if (data != NULL)
			{
				result[count] = liscr_data_get_data (data);
				if (result[count] != NULL)
					count++;
			}
			lua_pop (args->lua, 1);
			if (data == NULL)
				break;
		}
		lua_pop (args->lua, 1);
	}

	return count;
}

/*****************************************************************************/

static void Physics_cast_ray (LIScrArgs* args)
{
	int ignore;
	int group = LIPHY_DEFAULT_COLLISION_GROUP;
	int mask = LIPHY_DEFAULT_COLLISION_MASK;
	LIMatVector start;
	LIMatVector end;
	LIExtPhysicsModule* module;
	LIMatVector vector;
	LIPhyCollision result;
	LIPhyObject* ignores[100];

	/* Handle arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS);
	if (!liscr_args_geti_vector (args, 0, &start) ||
	    !liscr_args_geti_vector (args, 1, &end))
		return;
	liscr_args_geti_int (args, 2, &mask);
	ignore = private_build_ignore_list (module, args, 3, ignores, sizeof (ignores) / sizeof (*ignores));

	if (liphy_physics_cast_ray (module->physics, &start, &end, group, mask, ignores, ignore, &result))
	{
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
		liscr_args_sets_vector (args, "point", &result.point);
		liscr_args_sets_vector (args, "normal", &result.normal);
		if (result.object != NULL)
		{
			liscr_args_sets_int (args, "object", liphy_object_get_external_id (result.object));
		}
		if (result.terrain != NULL)
		{
			vector.x = result.terrain_tile[0];
			vector.y = result.terrain_tile[1];
			vector.z = result.terrain_tile[2];
			liscr_args_sets_vector (args, "tile", &vector);
		}
	}
}

static void Physics_cast_sphere (LIScrArgs* args)
{
	int ignore;
	int group = LIPHY_DEFAULT_COLLISION_GROUP;
	int mask = LIPHY_DEFAULT_COLLISION_MASK;
	float radius = 0.5f;
	LIMatVector start;
	LIMatVector end;
	LIExtPhysicsModule* module;
	LIMatVector vector;
	LIPhyCollision result;
	LIPhyObject* ignores[100];

	/* Handle arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS);
	if (!liscr_args_geti_vector (args, 0, &start) ||
	    !liscr_args_geti_vector (args, 1, &end))
		return;
	if (liscr_args_geti_float (args, 2, &radius))
		radius = LIMAT_MAX (0.02f, radius);
	liscr_args_geti_int (args, 3, &mask);
	ignore = private_build_ignore_list (module, args, 4, ignores, sizeof (ignores) / sizeof (*ignores));

	if (liphy_physics_cast_ray (module->physics, &start, &end, group, mask, ignores, ignore, &result))
	{
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
		liscr_args_sets_vector (args, "point", &result.point);
		liscr_args_sets_vector (args, "normal", &result.normal);
		if (result.object != NULL)
		{
			liscr_args_sets_int (args, "object", liphy_object_get_external_id (result.object));
		}
		if (result.terrain != NULL)
		{
			vector.x = result.terrain_tile[0];
			vector.y = result.terrain_tile[1];
			vector.z = result.terrain_tile[2];
			liscr_args_sets_vector (args, "tile", &vector);
		}
	}
}

static void Physics_get_enable_simulation (LIScrArgs* args)
{
	LIExtPhysicsModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS);
	liscr_args_seti_bool (args, module->simulate);
}
static void Physics_set_enable_simulation (LIScrArgs* args)
{
	int value;
	LIExtPhysicsModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS);
	if (liscr_args_geti_bool (args, 0, &value))
		module->simulate = value;
}

/*****************************************************************************/

void liext_script_physics (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_PHYSICS, "physics_cast_ray", Physics_cast_ray);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_PHYSICS, "physics_cast_sphere", Physics_cast_sphere);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_PHYSICS, "physics_get_enable_simulation", Physics_get_enable_simulation);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_PHYSICS, "physics_set_enable_simulation", Physics_set_enable_simulation);
}

/** @} */
/** @} */
