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

/* @luadoc
 * module "core/render"
 * ---
 * -- Display the game scene.
 * -- @name Scene
 * -- @class table
 */

/* @luadoc
 * --- Creates a new scene rendering buffer.
 * --
 * -- @param clss Scene class.
 * -- @param args Arguments.
 * function Scene.new(clss, args)
 */
static void Scene_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenDeferred* self;
	LIScrData* data;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	self = liren_deferred_new (module->client->render, 32, 32);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, args->clss, liren_deferred_free);
	if (data == NULL)
	{
		liren_deferred_free (self);
		return;
	}
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Begins scene rendering.
 * --
 * -- @param self Scene.
 * -- @param args Arguments.<ul>
 * --   <li>modelview: Modelview matrix.</li>
 * --   <li>projection: Projection matrix.</li>
 * --   <li>viewport: Viewport array.</li></ul>
 * function Scene.draw_begin(self, args)
 */
static void Scene_draw_begin (LIScrArgs* args)
{
	GLint viewport[4];
	LIExtModule* module;
	LIMatFrustum frustum;
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;

	/* Get arguments. */
	modelview = limat_matrix_identity ();
	projection = limat_matrix_identity ();
	liscr_args_gets_floatv (args, "modelview", 16, modelview.m);
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
	glDepthMask (GL_TRUE);
	glClear (GL_DEPTH_BUFFER_BIT);
	limat_frustum_init (&frustum, &modelview, &projection);
	liren_deferred_resize (args->self, viewport[2], viewport[3]);
	liren_scene_render_begin (scene, args->self, &modelview, &projection, &frustum);
}

/* @luadoc
 * ---
 * -- Begins the deferred rendering pass.
 * --
 * -- @param self Scene.
 * function Scene.draw_deferred_begin(self)
 */
static void Scene_draw_deferred_begin (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liren_scene_render_deferred_begin (scene);
}

/* @luadoc
 * ---
 * -- Ends the deferred rendering pass.<br/>
 * -- Draws lit fragments to the post-processing buffer using the lights of the
 * -- scene and the data in the deferred rendering G-buffer.
 * -- @param self Scene.
 * function Scene.draw_deferred_end(self)
 */
static void Scene_draw_deferred_end (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liren_scene_render_deferred_end (scene);
}

/* @luadoc
 * ---
 * -- Draws opaque faces to the deferred rendering G-buffer.
 * -- @param self Scene.
 * -- @param args Arguments.<ul>
 * --   <li>alpha: True to draw transparent as if they were opaque.</li></ul>
 * function Scene.draw_deferred_opaque(self, args)
 */
static void Scene_draw_deferred_opaque (LIScrArgs* args)
{
	int alpha = 0;
	LIExtModule* module;
	LIRenScene* scene;

	liscr_args_gets_int (args, "alpha", &alpha);
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liren_scene_render_deferred_opaque (scene, alpha);
}

/* @luadoc
 * ---
 * -- Ends scene rendering and draws the output to the framebuffer.
 * --
 * -- @param self Scene.
 * function Scene.draw_begin(self)
 */
static void Scene_draw_end (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liren_scene_render_end (scene);
	glPopAttrib ();
	glEnable (GL_BLEND);
	glEnable (GL_TEXTURE_2D);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable (GL_CULL_FACE);
	glDisable (GL_DEPTH_TEST);
	glDepthMask (GL_FALSE);
}

/* @luadoc
 * ---
 * -- Draws the transparent faces of the scene to the post-processing buffer.
 * --
 * -- @param self Scene.
 * function Scene.draw_forward_transparent(self)
 */
static void Scene_draw_forward_transparent (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liren_scene_render_forward_transparent (scene);
}

/* @luadoc
 * ---
 * -- Applies post-processing shaders to the output buffer.
 * --
 * -- @param self Scene.
 * -- @param args Arguments.<ul>
 * --   <li>shader: Post-processing shader name.</li></ul>
 * function Scene.draw_post_process(self, args)
 */
static void Scene_draw_post_process (LIScrArgs* args)
{
	const char* shader = "postprocess0";
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liscr_args_gets_string (args, "shader", &shader);

	liren_scene_render_postproc (scene, shader);
}

/*****************************************************************************/

void liext_script_scene (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SCENE, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "new", Scene_new);
	liscr_class_insert_mfunc (self, "draw_begin", Scene_draw_begin);
	liscr_class_insert_mfunc (self, "draw_deferred_begin", Scene_draw_deferred_begin);
	liscr_class_insert_mfunc (self, "draw_deferred_end", Scene_draw_deferred_end);
	liscr_class_insert_mfunc (self, "draw_deferred_opaque", Scene_draw_deferred_opaque);
	liscr_class_insert_mfunc (self, "draw_end", Scene_draw_end);
	liscr_class_insert_mfunc (self, "draw_forward_transparent", Scene_draw_forward_transparent);
	liscr_class_insert_mfunc (self, "draw_post_process", Scene_draw_post_process);
}

/** @} */
/** @} */
