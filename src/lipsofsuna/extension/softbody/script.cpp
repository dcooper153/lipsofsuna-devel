/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIExtSoftbody Softbody
 * @{
 */

#include "module.h"
#include "softbody.hpp"

static void Softbody_new (LIScrArgs* args)
{
	LIExtSoftbody* self;
	LIExtSoftbodyModule* module;
	LIScrData* data;

	module = (LIExtSoftbodyModule*) liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOFTBODY);
	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &data))
		return;

	/* Allocate self. */
	self = new LIExtSoftbody (module->physics, module->render, (const LIMdlModel*) liscr_data_get_data (data));
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_SOFTBODY, (LIScrGCFunc) liext_softbody_free);
	if (data == NULL)
	{
		liext_softbody_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Softbody_update (LIScrArgs* args)
{
	float v;
	LIExtSoftbody* self = (LIExtSoftbody*) args->self;

	if (!liscr_args_geti_float (args, 0, &v))
		return;
	self->update (v);
}

static void Softbody_set_collision_group (LIScrArgs* args)
{
	int v;
	LIExtSoftbody* self = (LIExtSoftbody*) args->self;

	if (!liscr_args_geti_int (args, 0, &v))
		return;
	self->set_collision_group (v);
}

static void Softbody_set_collision_mask (LIScrArgs* args)
{
	int v;
	LIExtSoftbody* self = (LIExtSoftbody*) args->self;

	if (!liscr_args_geti_int (args, 0, &v))
		return;
	self->set_collision_mask (v);
}

static void Softbody_set_position (LIScrArgs* args)
{
	float x;
	float y;
	float z;
	LIExtSoftbody* self = (LIExtSoftbody*) args->self;

	if (!liscr_args_geti_float (args, 0, &x) ||
	    !liscr_args_geti_float (args, 1, &y) ||
	    !liscr_args_geti_float (args, 2, &z))
		return;
	self->set_position (x, y, z);
}

static void Softbody_set_render_queue (LIScrArgs* args)
{
	const char* v;
	LIExtSoftbody* self = (LIExtSoftbody*) args->self;

	if (!liscr_args_geti_string (args, 0, &v))
		return;
	self->set_render_queue (v);
}

static void Softbody_set_rotation (LIScrArgs* args)
{
	float x;
	float y;
	float z;
	float w;
	LIExtSoftbody* self = (LIExtSoftbody*) args->self;

	if (!liscr_args_geti_float (args, 0, &x) ||
	    !liscr_args_geti_float (args, 1, &y) ||
	    !liscr_args_geti_float (args, 2, &z) ||
	    !liscr_args_geti_float (args, 3, &w))
		return;
	self->set_rotation (x, y, z, w);
}

static void Softbody_set_visible (LIScrArgs* args)
{
	int v;
	LIExtSoftbody* self = (LIExtSoftbody*) args->self;

	if (!liscr_args_geti_bool (args, 0, &v))
		return;
	self->set_visible (v);
}

/*****************************************************************************/

void liext_script_softbody (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOFTBODY, "softbody_new", Softbody_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOFTBODY, "softbody_update", Softbody_update);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOFTBODY, "softbody_set_collision_group", Softbody_set_collision_group);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOFTBODY, "softbody_set_collision_mask", Softbody_set_collision_mask);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOFTBODY, "softbody_set_position", Softbody_set_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOFTBODY, "softbody_set_render_queue", Softbody_set_render_queue);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOFTBODY, "softbody_set_rotation", Softbody_set_rotation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOFTBODY, "softbody_set_visible", Softbody_set_visible);
}

/** @} */
/** @} */
