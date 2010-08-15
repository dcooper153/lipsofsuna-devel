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
 * function Scene.draw_begin(self)
 */
void Scene_draw_begin (LIScrArgs* args)
{
	GLint viewport[4];
	LIAlgCamera* camera;
	LIExtModule* module;
	LIMatFrustum frustum;
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	camera = module->client->camera;
	scene = module->client->scene;

	glGetIntegerv (GL_VIEWPORT, viewport);
	glDepthMask (GL_TRUE);
	glClear (GL_DEPTH_BUFFER_BIT);
	lialg_camera_set_viewport (module->client->camera, viewport[0], viewport[1], viewport[2], viewport[3]);
	lialg_camera_get_modelview (camera, &modelview);
	lialg_camera_get_projection (camera, &projection);
	limat_frustum_init (&frustum, &modelview, &projection);
	liren_deferred_resize (module->deferred, viewport[2], viewport[3]);
	liren_scene_render_begin (scene, module->deferred, &modelview, &projection, &frustum);
}

/* @luadoc
 * ---
 * -- Draws the opaque faces of the scene to the post-processing buffer.
 * --
 * -- @param self Scene class.
 * function Scene.draw_deferred_opaque(self)
 */
void Scene_draw_deferred_opaque (LIScrArgs* args)
{
	LIExtModule* module;
	LIRenScene* scene;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	scene = module->client->scene;
	liren_scene_render_deferred_opaque (scene, 0, 0.0f);
}

/* @luadoc
 * ---
 * -- Ends scene rendering and draws the output to the framebuffer.
 * --
 * -- @param self Scene class.
 * function Scene.draw_begin(self)
 */
void Scene_draw_end (LIScrArgs* args)
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
void Scene_draw_forward_transparent (LIScrArgs* args)
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
void Scene_draw_post_process (LIScrArgs* args)
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
 * --   <li>rect: Viewport of the scene.</li>
 * --   <li>x: X coordinate.</li>
 * --   <li>y: Y coordinate.</li></ul>
 * -- @return Vector and object, or vector and nil when hit terrain.
 * function Scene.pick(clss, args)
 */
static void Scene_pick (LIScrArgs* args)
{
	int x;
	int y;
	int rect[4];
	LIAlgCamera* camera;
	LIExtModule* module;
	LIEngObject* object;
	LIMatFrustum frustum;
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenSelection result;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCENE);
	camera = module->client->camera;
	rect[0] = 0.0f;
	rect[1] = 0.0f;
	rect[2] = module->client->window->mode.width;
	rect[3] = module->client->window->mode.height;
	module->client->video.SDL_GetMouseState (&x, &y);
	liscr_args_gets_int (args, "x", &x);
	liscr_args_gets_int (args, "y", &y);
	liscr_args_gets_intv (args, "rect", 4, rect);

	/* Pick objects from the scene. */
	lialg_camera_get_modelview (camera, &modelview);
	lialg_camera_get_projection (camera, &projection);
	limat_frustum_init (&frustum, &modelview, &projection);
	y = module->client->window->mode.height - y - 1;
	if (!liren_scene_pick (module->client->scene, &modelview, &projection, &frustum, rect, x, y, 10, &result))
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
	liscr_class_insert_cfunc (self, "draw_deferred_opaque", Scene_draw_deferred_opaque);
	liscr_class_insert_cfunc (self, "draw_end", Scene_draw_end);
	liscr_class_insert_cfunc (self, "draw_forward_transparent", Scene_draw_forward_transparent);
	liscr_class_insert_cfunc (self, "draw_post_process", Scene_draw_post_process);
	liscr_class_insert_cfunc (self, "pick", Scene_pick);
}

/** @} */
/** @} */
