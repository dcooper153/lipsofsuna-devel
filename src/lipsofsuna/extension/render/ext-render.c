/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIExtWidgets Widgets
 * @{
 */

#include <lipsofsuna/render.h>
#include "ext-module.h"

static void Render_project (LIScrArgs* args)
{
	LIMatVector value;
	LIMatVector result;
	LIExtModule* module;

	if (liscr_args_geti_vector (args, 0, &value))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER);
		liren_render_project (module->client->render, &value, &result);
		liscr_args_seti_vector (args, &result);
	}
}

static void Render_set_camera_far (LIScrArgs* args)
{
	float value;
	LIExtModule* module;

	if (liscr_args_geti_float (args, 0, &value))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER);
		liren_render_set_camera_far (module->client->render, value);
	}
}

static void Render_set_camera_near (LIScrArgs* args)
{
	float value;
	LIExtModule* module;

	if (liscr_args_geti_float (args, 0, &value))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER);
		liren_render_set_camera_near (module->client->render, value);
	}
}

static void Render_set_camera_transform (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector position;
	LIMatQuaternion rotation;
	LIMatTransform transform;

	if (liscr_args_geti_vector (args, 0, &position) &&
	    liscr_args_geti_quaternion (args, 1, &rotation))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER);
		transform = limat_transform_init (position, rotation);
		liren_render_set_camera_transform (module->client->render, &transform);
	}
}

static void Render_get_anisotrophy (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER);
	liscr_args_seti_int (args, liren_render_get_anisotropy (module->client->render));
}
static void Render_set_anisotrophy (LIScrArgs* args)
{
	int value;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER);
	if (liscr_args_geti_int (args, 0, &value))
	{
		value = LIMAT_MAX (0, value);
		liren_render_set_anisotropy (module->client->render, value);
	}
}

static void Render_set_scene_ambient (LIScrArgs* args)
{
	int i;
	float value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER);
	for (i = 0 ; i < 4 ; i++)
		liscr_args_geti_float (args, i, value + i);
	liren_render_set_scene_ambient (module->client->render, value);
}

static void Render_set_skybox (LIScrArgs* args)
{
	const char* value;
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER);
	if (liscr_args_geti_string (args, 0, &value))
		liren_render_set_skybox (module->client->render, value);
}

/*****************************************************************************/

void liext_script_render (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER, "render_project", Render_project);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER, "render_set_camera_far", Render_set_camera_far);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER, "render_set_camera_near", Render_set_camera_near);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER, "render_set_camera_transform", Render_set_camera_transform);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER, "render_get_anisotrophy", Render_get_anisotrophy);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER, "render_set_anisotrophy", Render_set_anisotrophy);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER, "render_set_scene_ambient", Render_set_scene_ambient);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER, "render_set_skybox", Render_set_skybox);
}

/** @} */
/** @} */
