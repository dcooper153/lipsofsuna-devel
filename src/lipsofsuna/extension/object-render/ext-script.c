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
 * \addtogroup LIExtObjectRender ObjectRender
 * @{
 */

#include "ext-module.h"

static void Object_deform_mesh (LIScrArgs* args)
{
	LIExtModule* module;
	LIEngObject* object;

	/* Get render object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Deform the mesh. */
	liren_render_object_deform (module->render, object->id);
}

static void Object_particle_animation (LIScrArgs* args)
{
	int loop = 1;
	float start = 0.0f;
	LIExtModule* module;
	LIEngObject* object;

	/* Get render object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Get arguments. */
	liscr_args_gets_bool (args, "loop", &loop);
	liscr_args_gets_float (args, "time", &start);

	/* Deform the mesh. */
	liren_render_object_particle_animation (module->render, object->id, start, loop);
}

static void Object_set_effect (LIScrArgs* args)
{
	float params[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const char* shader = NULL;
	LIExtModule* module;
	LIEngObject* object;

	/* Get render object. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_OBJECT);
	object = args->self;

	/* Get arguments. */
	if (!liscr_args_geti_string (args, 0, &shader))
		liscr_args_gets_string (args, "shader", &shader);
	liscr_args_gets_floatv (args, "params", 4, params);

	/* Deform the mesh. */
	liren_render_object_set_effect (module->render, object->id, shader, params);
}

/*****************************************************************************/

void liext_script_render_object (
	LIScrScript* self)
{
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_deform_mesh", Object_deform_mesh);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_particle_animation", Object_particle_animation);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_effect", Object_set_effect);
}

/** @} */
/** @} */
