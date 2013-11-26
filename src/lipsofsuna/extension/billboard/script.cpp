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
 * \addtogroup LIExtBillboard Billboard
 * @{
 */

#include "module.h"
#include "billboard.hpp"

static void Billboard_new (LIScrArgs* args)
{
	LIExtBillboard* self;
	LIExtBillboardModule* module;
	LIScrData* data;

	/* Allocate self. */
	module = (LIExtBillboardModule*) liscr_script_get_userdata (args->script, LIEXT_SCRIPT_BILLBOARD);
	self = new LIExtBillboard (module->render);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_BILLBOARD, (LIScrGCFunc) liext_billboard_free);
	if (data == NULL)
	{
		liext_billboard_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Billboard_add (LIScrArgs* args)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	LIExtBillboard* self = (LIExtBillboard*) args->self;

	liscr_args_geti_float (args, 0, &x);
	liscr_args_geti_float (args, 1, &y);
	liscr_args_geti_float (args, 2, &z);
	self->billboards->createBillboard (x, y, z);
}

static void Billboard_clear (LIScrArgs* args)
{
	LIExtBillboard* self = (LIExtBillboard*) args->self;

	self->billboards->clear ();
}

static void Billboard_set_material (LIScrArgs* args)
{
	const char* name;
	LIExtBillboard* self = (LIExtBillboard*) args->self;

	if (!liscr_args_geti_string (args, 0, &name))
		return;
	self->billboards->setMaterialName (name);
}

static void Billboard_set_position (LIScrArgs* args)
{
	float x;
	float y;
	float z;
	LIExtBillboard* self = (LIExtBillboard*) args->self;

	if (!liscr_args_geti_float (args, 0, &x) ||
	    !liscr_args_geti_float (args, 1, &y) ||
	    !liscr_args_geti_float (args, 2, &z))
		return;
	self->node->setPosition (x, y, z);
}

static void Billboard_set_render_queue (LIScrArgs* args)
{
	const char* v;
	LIExtBillboard* self = (LIExtBillboard*) args->self;

	if (!liscr_args_geti_string (args, 0, &v))
		return;
	self->set_render_queue (v);
}

static void Billboard_set_rotation (LIScrArgs* args)
{
	float x;
	float y;
	float z;
	float w;
	LIExtBillboard* self = (LIExtBillboard*) args->self;

	if (!liscr_args_geti_float (args, 0, &x) ||
	    !liscr_args_geti_float (args, 1, &y) ||
	    !liscr_args_geti_float (args, 2, &z) ||
	    !liscr_args_geti_float (args, 3, &w))
		return;
	self->node->setOrientation (w, x, y, z);
}

static void Billboard_set_size (LIScrArgs* args)
{
	float w;
	float h;
	LIExtBillboard* self = (LIExtBillboard*) args->self;

	if (!liscr_args_geti_float (args, 0, &w))
		return;
	if (!liscr_args_geti_float (args, 1, &h))
		return;
	self->billboards->setDefaultWidth (w);
	self->billboards->setDefaultHeight (h);
}

static void Billboard_set_visible (LIScrArgs* args)
{
	int v;
	LIExtBillboard* self = (LIExtBillboard*) args->self;

	if (!liscr_args_geti_bool (args, 0, &v))
		return;
	self->node->setVisible (v);
}

/*****************************************************************************/

void liext_script_billboard (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_BILLBOARD, "billboard_new", Billboard_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_BILLBOARD, "billboard_add", Billboard_add);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_BILLBOARD, "billboard_clear", Billboard_clear);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_BILLBOARD, "billboard_set_material", Billboard_set_material);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_BILLBOARD, "billboard_set_position", Billboard_set_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_BILLBOARD, "billboard_set_render_queue", Billboard_set_render_queue);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_BILLBOARD, "billboard_set_rotation", Billboard_set_rotation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_BILLBOARD, "billboard_set_size", Billboard_set_size);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_BILLBOARD, "billboard_set_visible", Billboard_set_visible);
}

/** @} */
/** @} */
