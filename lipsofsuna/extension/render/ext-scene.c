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
 * module "Extension.Render"
 * ---
 * -- Display the game scene.
 * -- @name Scene
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Begins scene rendering.
 * --
 * -- @param self Scene class.
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
	viewport[2] = LIMAT_MAX (2, viewport[2]);
	viewport[3] = LIMAT_MAX (2, viewport[3]);

	/* Initialize rendering mode. */
	glDepthMask (GL_TRUE);
	glClear (GL_DEPTH_BUFFER_BIT);
	limat_frustum_init (&frustum, &modelview, &projection);
	liren_deferred_resize (module->deferred, viewport[2], viewport[3]);
	liren_scene_render_begin (scene, module->deferred, &modelview, &projection, &frustum);
}

/* @luadoc
 * ---
 * -- Begins the deferred rendering pass.
 * --
 * -- @param self Scene class.
 * -- @param args Arguments.<ul>
 * --   <li>alphatest: True to draw transparent faces using alpha test.</li>
 * --   <li>threshold: Alpha test threshold.</li></ul>
 * function Scene.draw_deferred_begin(self, args)
 */
static void Scene_draw_deferred_begin (LIScrArgs* args)
{
	int alphatest = 0;
	float threshold = 0.5f;
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liscr_args_gets_int (args, "alphatest", &alphatest);
	liscr_args_gets_float (args, "threshold", &threshold);
	liren_scene_render_deferred_begin (scene, alphatest, threshold);
}

/* @luadoc
 * ---
 * -- Ends the deferred rendering pass.<br/>
 * -- Draws lit fragments to the post-processing buffer using the lights of the
 * -- scene and the data in the deferred rendering G-buffer.
 * -- @param self Scene class.
 * -- @param args Arguments.<ul>
 * --   <li>alphatest: True to draw transparent faces using alpha test.</li>
 * --   <li>threshold: Alpha test threshold.</li></ul>
 * function Scene.draw_deferred_begin(self, args)
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
 * -- @param self Scene class.
 * function Scene.draw_deferred_opaque(self)
 */
static void Scene_draw_deferred_opaque (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liren_scene_render_deferred_opaque (scene);
}

/* @luadoc
 * ---
 * -- Ends scene rendering and draws the output to the framebuffer.
 * --
 * -- @param self Scene class.
 * function Scene.draw_begin(self)
 */
static void Scene_draw_end (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liren_scene_render_end (scene);
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
 * -- @param self Scene class.
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
 * -- @param self Scene class.
 * function Scene.draw_post_process(self)
 */
static void Scene_draw_post_process (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liren_scene_render_postproc (scene);
}

/* @luadoc
 * --- Pick an object from the scene.
 * -- 
 * -- @param self Scene class.
 * -- @param args Arguments.<ul>
 * --   <li>modelview: Modelview matrix.</li>
 * --   <li>projection: Projection matrix.</li>
 * --   <li>viewport: Viewport array.</li>
 * --   <li>x: X coordinate.</li>
 * --   <li>y: Y coordinate.</li></ul>
 * -- @return Vector and object, or vector and nil when hit terrain.
 * function Scene.pick(clss, args)
 */
static void Scene_pick (LIScrArgs* args)
{
	int x;
	int y;
	int viewport[4];
	LIExtModule* module;
	LIEngObject* object;
	LIMatFrustum frustum;
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenSelection result;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);

	/* Get arguments. */
	modelview = limat_matrix_identity ();
	projection = limat_matrix_identity ();
	liscr_args_gets_floatv (args, "modelview", 16, modelview.m);
	liscr_args_gets_floatv (args, "projection", 16, projection.m);
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = module->client->window->mode.width;
	viewport[2] = module->client->window->mode.height;
	liscr_args_gets_intv (args, "viewport", 4, viewport);
	viewport[0] = LIMAT_MAX (0, viewport[0]);
	viewport[1] = LIMAT_MAX (0, viewport[1]);
	viewport[2] = LIMAT_MAX (2, viewport[2]);
	viewport[3] = LIMAT_MAX (2, viewport[3]);
	module->client->video.SDL_GetMouseState (&x, &y);
	liscr_args_gets_int (args, "x", &x);
	liscr_args_gets_int (args, "y", &y);

	/* Pick objects from the scene. */
	limat_frustum_init (&frustum, &modelview, &projection);
	y = module->client->window->mode.height - y - 1;
	if (!liren_scene_pick (module->client->scene, &modelview, &projection, &frustum, viewport, x, y, 10, &result))
		return;

	/* Return the picked point and object. */
	object = lieng_engine_find_object (module->program->engine, result.object);
	liscr_args_seti_vector (args, &result.point);
	liscr_args_seti_data (args, (object != NULL)? object->script : NULL);
}

/*****************************************************************************/

void liext_script_scene (
	LIScrClass* self,
	void*       data)
{
	liscr_class_inherit (self, liscr_script_data, data);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SCENE, data);
	liscr_class_insert_cfunc (self, "draw_begin", Scene_draw_begin);
	liscr_class_insert_cfunc (self, "draw_deferred_begin", Scene_draw_deferred_begin);
	liscr_class_insert_cfunc (self, "draw_deferred_end", Scene_draw_deferred_end);
	liscr_class_insert_cfunc (self, "draw_deferred_opaque", Scene_draw_deferred_opaque);
	liscr_class_insert_cfunc (self, "draw_end", Scene_draw_end);
	liscr_class_insert_cfunc (self, "draw_forward_transparent", Scene_draw_forward_transparent);
	liscr_class_insert_cfunc (self, "draw_post_process", Scene_draw_post_process);
	liscr_class_insert_cfunc (self, "pick", Scene_pick);
}

/** @} */
/** @} */
