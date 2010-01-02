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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvNpc Npc
 * @{
 */

#include <script/lips-script.h>
#include <server/lips-server.h>
#include "ext-module.h"
#include "ext-npc.h"

/* @luadoc
 * module "Extension.Server.Npc"
 * ---
 * -- Create intelligent non-player characters.
 * -- @name Npc
 * -- @class table
 */

/* @luadoc
 * ---
 * -- @brief Finds the NPC logic for an object.
 * --
 * -- Arguments:
 * -- object: Object.
 * --
 * -- @param self Npc class.
 * -- @param args Arguments.
 * -- @return Npc or nil.
 * function Npc.find(self, args)
 */
static void Npc_find (liscrArgs* args)
{
	liextModule* module;
	liextNpc* npc;
	liscrData* data;

	if (liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &data))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NPC);
		npc = liext_module_find_npc (module, data->data);
		if (npc != NULL)
			liscr_args_seti_data (args, npc->script);
	}
}

/* @luadoc
 * ---
 * -- Creates a new non-player character logic.
 * --
 * -- @param self Npc class.
 * -- @param args Arguments.
 * -- @return New NPC.
 * function Npc.new(self, args)
 */
static void Npc_new (liscrArgs* args)
{
	liextModule* module;
	liextNpc* self;
	liscrData* data;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NPC);
	self = liext_npc_new (module);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_NPC, liext_npc_free);
	if (data == NULL)
	{
		liext_npc_free (self);
		return;
	}
	self->script = data;
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Solves the path to a point for the NPC logic.
 * --
 * -- Arguments:
 * -- point: Point vector. (required)
 * --
 * -- @paran self Npc.
 * -- @param args Arguments.
 * -- @return Path or nil.
 * function Npc.solve_path(self, args)
 */
static void Npc_solve_path (liscrArgs* args)
{
	liaiPath* tmp;
	liextNpc* self;
	limatVector vector;
	liscrData* path;

	if (!liscr_args_gets_vector (args, "point", &vector))
		return;
	self = args->self;

	/* Solve path. */
	if (self->owner == NULL)
		return;
	tmp = liext_module_solve_path (self->module, self->owner, &vector);
	if (tmp == NULL)
		return;

	/* Create path object. */
	path = liscr_data_new (args->script, tmp, LISCR_SCRIPT_PATH, liai_path_free);
	if (path != NULL)
	{
		liscr_args_seti_data (args, path);
		liscr_data_unref (path, NULL);
		return;
	}
}

/* @luadoc
 * ---
 * -- Alertness flag.
 * -- @name Npc.alert
 * -- @class table
 */
static void Npc_getter_alert (liscrArgs* args)
{
	liscr_args_seti_bool (args, ((liextNpc*) args->self)->alert);
}
static void Npc_setter_alert (liscrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		((liextNpc*) args->self)->alert = value;
}

/* @luadoc
 * ---
 * -- Called when the controlled object is near the target.
 * --
 * -- Arguments passed to the callback: npc, object, target.
 * --
 * -- @name Npc.attack_cb
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Controlled object.
 * -- @name Npc.owner
 * -- @class table
 */
static void Npc_getter_owner (liscrArgs* args)
{
	if (((liextNpc*) args->self)->owner != NULL)
		liscr_args_seti_data (args, ((liextNpc*) args->self)->owner->script);
}
static void Npc_setter_owner (liscrArgs* args)
{
	liscrData* data;

	if (liscr_args_geti_data (args, 0, LIEXT_SCRIPT_NPC, &data))
		liext_npc_set_owner (args->self, data->data);
	else
		liext_npc_set_owner (args->self, NULL);
}

/* @luadoc
 * ---
 * -- The path for the object to traverse.
 * -- @name Npc.path
 * -- @class table
 */
static void Npc_getter_path (liscrArgs* args)
{
	if (((liextNpc*) args->self)->path != NULL)
		liscr_args_seti_data (args, ((liextNpc*) args->self)->path);
}
static void Npc_setter_path (liscrArgs* args)
{
	liscrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_PATH, &data))
		liext_npc_set_path (args->self, data);
	else
		liext_npc_set_path (args->self, NULL);
}

/* @luadoc
 * ---
 * -- Target scanning radius.
 * -- @name Npc.radius
 * -- @class table
 */
static void Npc_getter_radius (liscrArgs* args)
{
	liscr_args_seti_float (args, ((liextNpc*) args->self)->radius);
}
static void Npc_setter_radius (liscrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		((liextNpc*) args->self)->radius = value;
}

/* @luadoc
 * ---
 * -- Refresh delay.
 * -- @name Npc.refresh
 * -- @class table
 */
static void Npc_getter_refresh (liscrArgs* args)
{
	liscr_args_seti_float (args, ((liextNpc*) args->self)->refresh);
}
static void Npc_setter_refresh (liscrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		((liextNpc*) args->self)->refresh = value;
}

/* @luadoc
 * ---
 * -- Targeted object.
 * -- @name Npc.target
 * -- @class table
 */
static void Npc_getter_target (liscrArgs* args)
{
	if (((liextNpc*) args->self)->target != NULL)
		liscr_args_seti_data (args, ((liextNpc*) args->self)->target->script);
}
static void Npc_setter_target (liscrArgs* args)
{
	liscrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_OBJECT, &data))
		liext_npc_set_target (args->self, data->data);
	else
		liext_npc_set_target (args->self, NULL);
}

/*****************************************************************************/

void
liextNpcScript (liscrClass* self,
                void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_NPC, data);
	liscr_class_insert_cfunc (self, "find", Npc_find);
	liscr_class_insert_cfunc (self, "new", Npc_new);
	liscr_class_insert_mfunc (self, "solve_path", Npc_solve_path);
	liscr_class_insert_mvar (self, "alert", Npc_getter_alert, Npc_setter_alert);
	liscr_class_insert_mvar (self, "owner", Npc_getter_owner, Npc_setter_owner);
	liscr_class_insert_mvar (self, "path", Npc_getter_path, Npc_setter_path);
	liscr_class_insert_mvar (self, "radius", Npc_getter_radius, Npc_setter_radius);
	liscr_class_insert_mvar (self, "refresh", Npc_getter_refresh, Npc_setter_refresh);
	liscr_class_insert_mvar (self, "target", Npc_getter_target, Npc_setter_target);
}

/** @} */
/** @} */
/** @} */
