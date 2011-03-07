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
 * \addtogroup LIExtPhysics Physics
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "core/physics"
 * --- Physics simulation support.
 * -- @name Physics
 * -- @class table
 */

/* @luadoc
 * --- Performs a ray cast test.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>collision_group: Collision group.</li>
 * --   <li>collision_mask: Collision mask.</li>
 * --   <li>ignore: Object to ignore.</li>
 * --   <li>src: Start point vector in world space.</li>
 * --   <li>dst: End point vector in world space.</li></ul>
 * -- @return Table with point, normal, object and tile. Nil if no collision occurred.
 * function Physics.cast_ray(self, args)
 */
static void Physics_cast_ray (LIScrArgs* args)
{
	int ignore = 0;
	int group = LIPHY_DEFAULT_COLLISION_GROUP;
	int mask = LIPHY_DEFAULT_COLLISION_MASK;
	LIMatVector start;
	LIMatVector end;
	LIEngObject* hitobj;
	LIExtModule* module;
	LIMatVector vector;
	LIPhyCollision result;
	LIPhyObject* ignores[1];
	LIScrData* data;

	/* Handle arguments. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_PHYSICS);
	if (!liscr_args_gets_vector (args, "src", &start) ||
	    !liscr_args_gets_vector (args, "dst", &end))
		return;
	liscr_args_gets_int (args, "collision_group", &group);
	liscr_args_gets_int (args, "collision_mask", &mask);
	if (liscr_args_gets_data (args, "ignore", LISCR_SCRIPT_OBJECT, &data))
	{
		hitobj = liscr_data_get_data (data);
		ignores[0] = liphy_physics_find_object (module->physics, hitobj->id);
		if (ignores[0] != NULL)
			ignore = 1;
	}

	if (liphy_physics_cast_ray (module->physics, &start, &end, group, mask, ignores, ignore, &result))
	{
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
		liscr_args_sets_vector (args, "point", &result.point);
		liscr_args_sets_vector (args, "normal", &result.normal);
		if (result.object != NULL)
		{
			hitobj = liphy_object_get_userdata (result.object);
			if (hitobj != NULL && hitobj->script != NULL)
				liscr_args_sets_data (args, "object", hitobj->script);
		}
		if (result.terrain != NULL)
		{
			liphy_terrain_get_tile (result.terrain, result.terrain_index, &vector);
			liscr_args_sets_vector (args, "tile", &vector);
		}
	}
}

/* @luadoc
 * --- Performs a sphere cast test.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>collision_group: Collision group.</li>
 * --   <li>collision_mask: Collision mask.</li>
 * --   <li>ignore: Object to ignore.</li>
 * --   <li>radius: Radius in world units.</li>
 * --   <li>src: Start point vector in world space.</li>
 * --   <li>dst: End point vector in world space.</li></ul>
 * -- @return Table with point, normal, object and tile. Nil if no collision occurred.
 * function Physics.cast_sphere(self, args)
 */
static void Physics_cast_sphere (LIScrArgs* args)
{
	int ignore = 0;
	int group = LIPHY_DEFAULT_COLLISION_GROUP;
	int mask = LIPHY_DEFAULT_COLLISION_MASK;
	float radius = 0.5f;
	LIMatVector start;
	LIMatVector end;
	LIEngObject* hitobj;
	LIExtModule* module;
	LIMatVector vector;
	LIPhyCollision result;
	LIPhyObject* ignores[1];
	LIScrData* data;

	/* Handle arguments. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_PHYSICS);
	if (!liscr_args_gets_vector (args, "src", &start) ||
	    !liscr_args_gets_vector (args, "dst", &end))
		return;
	liscr_args_gets_int (args, "collision_group", &group);
	liscr_args_gets_int (args, "collision_mask", &mask);
	if (liscr_args_gets_float (args, "radius", &radius))
		radius = LIMAT_MAX (0.02f, radius);
	if (liscr_args_gets_data (args, "ignore", LISCR_SCRIPT_OBJECT, &data))
	{
		hitobj = liscr_data_get_data (data);
		ignores[0] = liphy_physics_find_object (module->physics, hitobj->id);
		if (ignores[0] != NULL)
			ignore = 1;
	}

	if (liphy_physics_cast_ray (module->physics, &start, &end, group, mask, ignores, ignore, &result))
	{
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
		liscr_args_sets_vector (args, "point", &result.point);
		liscr_args_sets_vector (args, "normal", &result.normal);
		if (result.object != NULL)
		{
			hitobj = liphy_object_get_userdata (result.object);
			if (hitobj != NULL && hitobj->script != NULL)
				liscr_args_sets_data (args, "object", hitobj->script);
		}
		if (result.terrain != NULL)
		{
			liphy_terrain_get_tile (result.terrain, result.terrain_index, &vector);
			liscr_args_sets_vector (args, "tile", &vector);
		}
	}
}

/* @luadoc
 * --- Physics simulation toggle.
 * -- @name Physics.enable_simulation
 * -- @class table
 */
static void Physics_getter_enable_simulation (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_PHYSICS);
	liscr_args_seti_bool (args, module->simulate);
}
static void Physics_setter_enable_simulation (LIScrArgs* args)
{
	int value;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_PHYSICS);
	if (liscr_args_geti_bool (args, 0, &value))
		module->simulate = value;
}

/*****************************************************************************/

void liext_script_physics (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_PHYSICS, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "cast_ray", Physics_cast_ray);
	liscr_class_insert_cfunc (self, "cast_sphere", Physics_cast_sphere);
	liscr_class_insert_cvar (self, "enable_simulation", Physics_getter_enable_simulation, Physics_setter_enable_simulation);
}

/** @} */
/** @} */
