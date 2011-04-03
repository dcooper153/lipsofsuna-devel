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
 * \addtogroup LIExtWidgets Widgets
 * @{
 */

#include <lipsofsuna/render.h>
#include "ext-module.h"

static void Scene_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenDeferred* self;
	LIScrData* data;

	/* Allocate self. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SCENE);
	self = liren_deferred_new (module->client->render, 32, 32, 1, 0);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_SCENE, liren_deferred_free);
	if (data == NULL)
	{
		liren_deferred_free (self);
		return;
	}
	liscr_args_seti_data (args, data);
	liscr_data_unref (data);
}

static void Scene_draw_begin (LIScrArgs* args)
{
	int hdr = 0;
	int multisamples = 1;
	GLint viewport[4];
	LIExtModule* module;
	LIMatFrustum frustum;
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenScene* scene;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;

	/* Get arguments. */
	modelview = limat_matrix_identity ();
	projection = limat_matrix_identity ();
	liscr_args_gets_bool (args, "hdr", &hdr);
	liscr_args_gets_floatv (args, "modelview", 16, modelview.m);
	if (liscr_args_gets_int (args, "multisamples", &multisamples))
		multisamples = LIMAT_MAX (1, multisamples);
	liscr_args_gets_floatv (args, "projection", 16, projection.m);
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = module->client->window->mode.width;
	viewport[3] = module->client->window->mode.height;
	liscr_args_gets_intv (args, "viewport", 4, viewport);
	viewport[0] = LIMAT_MAX (0, viewport[0]);
	viewport[1] = LIMAT_MAX (0, viewport[1]);
	viewport[1] = module->client->window->mode.height - viewport[1] - viewport[3];
	viewport[2] = LIMAT_MAX (2, viewport[2]);
	viewport[3] = LIMAT_MAX (2, viewport[3]);

	/* Initialize rendering mode. */
	glPushAttrib (GL_VIEWPORT_BIT);
	glViewport (viewport[0], viewport[1], viewport[2], viewport[3]);
	limat_frustum_init (&frustum, &modelview, &projection);
	liren_deferred_resize (args->self, viewport[2], viewport[3], multisamples, hdr);
	liren_scene_render_begin (scene, args->self, &modelview, &projection, &frustum);
}

static void Scene_draw_end (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liren_scene_render_end (scene);
	glPopAttrib ();
}

static void Scene_draw_pass (LIScrArgs* args)
{
	int pass = 1;
	int sorting = 0;
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liscr_args_gets_int (args, "pass", &pass);
	liscr_args_gets_bool (args, "sorting", &sorting);
	if (pass < 1 || pass > LIREN_SHADER_PASS_COUNT)
		return;

	liren_scene_render_pass (scene, pass - 1, sorting);
}

static void Scene_draw_post_process (LIScrArgs* args)
{
	int mipmaps = 0;
	const char* shader = "postprocess0";
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liscr_args_gets_bool (args, "mipmaps", &mipmaps);
	liscr_args_gets_string (args, "shader", &shader);

	liren_scene_render_postproc (scene, shader, mipmaps);
}

/*****************************************************************************/

void liext_script_scene (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SCENE, "scene_new", Scene_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SCENE, "scene_draw_begin", Scene_draw_begin);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SCENE, "scene_draw_end", Scene_draw_end);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SCENE, "scene_draw_pass", Scene_draw_pass);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SCENE, "scene_draw_post_process", Scene_draw_post_process);
}

/** @} */
/** @} */
