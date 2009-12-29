/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliscr Script
 * @{
 */

#include <render/lips-render.h>
#include "lips-client.h"

static void
private_render (liwdgRender* self,
                void*        data)
{
	licliModule* module;
	liengSelectionIter iter;
	limatFrustum frustum;
	limatMatrix modelview;
	limatMatrix projection;
	lirndContext context;
	lirndObject* object;
	liwdgRect rect;

	/* Set 3D mode. */
	module = data;
	liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
	lialg_camera_set_viewport (module->camera, rect.x, rect.y, rect.width, rect.height);
	lialg_camera_get_frustum (module->camera, &frustum);
	lialg_camera_get_modelview (module->camera, &modelview);
	lialg_camera_get_projection (module->camera, &projection);
	lirnd_context_init (&context, module->scene);
	lirnd_context_set_modelview (&context, &modelview);
	lirnd_context_set_projection (&context, &projection);
	lirnd_context_set_frustum (&context, &frustum);

	/* Draw selection. */
	if (module->network != NULL)
	{
		glDisable (GL_LIGHTING);
		glDisable (GL_DEPTH_TEST);
		glDisable (GL_CULL_FACE);
		glDisable (GL_TEXTURE_2D);
		glDepthMask (GL_FALSE);
		glColor3f (1.0f, 0.0f, 0.0f);
		LIENG_FOREACH_SELECTION (iter, module->engine)
		{
			object = lirnd_scene_find_object (module->scene, iter.object->id);
			if (object != NULL)
				lirnd_draw_bounds (&context, object, NULL);
		}
		lirnd_context_unbind (&context);
	}

	/* Render custom 3D scene. */
	lieng_engine_call (module->engine, LICLI_CALLBACK_RENDER_3D, self);

	/* Set 2D mode. */
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, rect.width, 0, rect.height, -100.0f, 100.0f);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	/* Render custom 2D scene. */
	lieng_engine_call (module->engine, LICLI_CALLBACK_RENDER_2D, self);
}

static void
private_update (liwdgRender* self,
                void*        data)
{
	licliModule* module;
	limatMatrix modelview;
	limatMatrix projection;

	module = data;
	if (module->network != NULL)
	{
		lialg_camera_get_modelview (module->camera, &modelview);
		lialg_camera_get_projection (module->camera, &projection);
		liwdg_render_set_modelview (LIWDG_RENDER (self), &modelview);
		liwdg_render_set_projection (LIWDG_RENDER (self), &projection);
	}
}

/*****************************************************************************/

/* @luadoc
 * module "Core.Client.Scene"
 * ---
 * -- Display the game scene.
 * -- @name Button
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new scene widget.
 * --
 * -- @param self Scene class.
 * -- @param args Optional arguments.
 * -- @return New scene widget.
 * function Scene.new(self, args)
 */
static void Scene_new (liscrArgs* args)
{
	licliModule* module;
	liscrData* data;
	liwdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_SCENE);
	self = liwdg_render_new (module->widgets, module->scene);
	if (self == NULL)
		return;
	LIWDG_RENDER (self)->custom_update_func = private_update;
	LIWDG_RENDER (self)->custom_update_data = module;
	LIWDG_RENDER (self)->custom_render_func = private_render;
	LIWDG_RENDER (self)->custom_render_data = module;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LICLI_SCRIPT_SCENE, licli_script_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Pick an object from the scene.
 * --
 * -- Arguments:
 * -- x: X coordinate.
 * -- y: Y coordinate.
 * --
 * -- @param self Scene.
 * -- @param args Arguments.
 * -- @return Vector and object, or vector and nil when hit terrain.
 * function Scene.pick(self, args)
 */
static void Scene_pick (liscrArgs* args)
{
	int x;
	int y;
	licliModule* module;
	liengObject* object;
	lirndSelection result;

	module = LIWDG_RENDER (args->self)->custom_render_data;
	module->client->video.SDL_GetMouseState (&x, &y);
	liscr_args_gets_int (args, "x", &x);
	liscr_args_gets_int (args, "y", &y);

	/* Pick object from scene. */
	if (!liwdg_render_pick (args->self, &result, x, module->window->mode.height - y - 1))
		return;
	object = lieng_engine_find_object (module->engine, result.object);
	liscr_args_seti_vector (args, &result.point);
	liscr_args_seti_data (args, (object != NULL)? object->script : NULL);
}

/*****************************************************************************/

void
licliSceneScript (liscrClass* self,
                  void*       data)
{
	liscr_class_inherit (self, licliGroupScript, data);
	liscr_class_set_userdata (self, LICLI_SCRIPT_SCENE, data);
	liscr_class_insert_cfunc (self, "new", Scene_new);
	liscr_class_insert_mfunc (self, "pick", Scene_pick);
}

/** @} */
/** @} */
