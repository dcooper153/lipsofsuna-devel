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
 * \addtogroup LIExtSpawner Spawner
 * @{
 */

#include <lipsofsuna/main.h>
#include <lipsofsuna/server.h>
#include "ext-module.h"
#include "ext-spawner.h"

/* @luadoc
 * module "Extension.Server.Spawner"
 * ---
 * -- Spawn objects periodically.
 * -- @name Spawner
 * -- @class table
 */

/* @luadoc
 * ---
 * -- @brief Finds the spawner logic for an object.
 * --
 * -- Arguments:
 * -- object: Object.
 * --
 * -- @param self Spawner class.
 * -- @param args Arguments.
 * -- @return Spawner or nil.
 * function Spawner.find(self, args)
 */
static void Spawner_find (LIScrArgs* args)
{
	LIExtModule* module;
	LIExtSpawner* spawner;
	LIScrData* object;

	if (liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &object))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SPAWNER);
		spawner = liext_module_find_spawner (module, object->data);
		if (spawner != NULL)
			liscr_args_seti_data (args, spawner->script);
	}
}

/* @luadoc
 * ---
 * -- Creates a new spawner logic.
 * --
 * -- @param self Spawner class.
 * -- @param args Arguments.
 * -- @return New spawner.
 * function Spawner.new(self, args)
 */
static void Spawner_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIExtSpawner* self;
	LIScrData* data;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SPAWNER);
	self = liext_spawner_new (module);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_SPAWNER, liext_spawner_free);
	if (self == NULL)
	{
		liext_spawner_free (self);
		return;
	}
	self->script = data;
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Spawn delay.
 * -- @name Spawner.delay
 * -- @class table
 */
static void Spawner_getter_delay (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIExtSpawner*) args->self)->delay);
}
static void Spawner_setter_delay (LIScrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		((LIExtSpawner*) args->self)->delay = value;
}

/* @luadoc
 * ---
 * -- Maximum number of spawned objects.
 * --
 * -- If the current number of spawned objects is higher than the limit,
 * -- the spawner will disown objects until the count is equal to the limit.
 * -- The order of disowning is undefined.
 * --
 * -- @name Spawner.limit
 * -- @class table
 */
static void Spawner_getter_limit (LIScrArgs* args)
{
	liscr_args_seti_int (args, ((LIExtSpawner*) args->self)->limit);
}
static void Spawner_setter_limit (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value) && value >= 0)
		liext_spawner_set_limit (args->self, value);
}

/* @luadoc
 * ---
 * -- Controlled object.
 * -- @name Spawner.object
 * -- @class table
 */
static void Spawner_getter_owner (LIScrArgs* args)
{
	if (((LIExtSpawner*) args->self)->owner != NULL)
		liscr_args_seti_data (args, ((LIExtSpawner*) args->self)->owner->script);
}
static void Spawner_setter_owner (LIScrArgs* args)
{
	LIScrData* object;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_OBJECT, &object))
		liext_spawner_set_owner (args->self, object->data);
	else
		liext_spawner_set_owner (args->self, NULL);
}

/* @luadoc
 * ---
 * -- Spawn function.
 * --
 * -- The object controlled by the spawner is passed to the function as an
 * -- argument whenever there's a need to spawn an object. The function is
 * -- expected to return a new object or nil if nothing should be spawned.
 * --
 * -- @name Spawner.callback
 * -- @class table
 */

/*****************************************************************************/

void
liext_script_spawner (LIScrClass* self,
                    void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SPAWNER, data);
	liscr_class_insert_cfunc (self, "find", Spawner_find);
	liscr_class_insert_cfunc (self, "new", Spawner_new);
	liscr_class_insert_mvar (self, "delay", Spawner_getter_delay, Spawner_setter_delay);
	liscr_class_insert_mvar (self, "limit", Spawner_getter_limit, Spawner_setter_limit);
	liscr_class_insert_mvar (self, "owner", Spawner_getter_owner, Spawner_setter_owner);
}

/** @} */
/** @} */
