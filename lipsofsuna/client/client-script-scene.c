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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliscr Script
 * @{
 */

#include <lipsofsuna/render.h>
#include <lipsofsuna/client.h>

static void
private_render (LIWdgRender* self,
                void*        data)
{
	LICliClient* client;
	LIEngSelectionIter iter;
	LIMatFrustum frustum;
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenContext context;
	LIRenObject* object;
	LIWdgRect rect;

	/* Set 3D mode. */
	client = data;
	liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
	lialg_camera_set_viewport (client->camera, rect.x, rect.y, rect.width, rect.height);
	lialg_camera_get_frustum (client->camera, &frustum);
	lialg_camera_get_modelview (client->camera, &modelview);
	lialg_camera_get_projection (client->camera, &projection);
	liren_context_init (&context, client->scene);
	liren_context_set_modelview (&context, &modelview);
	liren_context_set_projection (&context, &projection);
	liren_context_set_frustum (&context, &frustum);

	/* Draw selection. */
	if (client->network != NULL)
	{
		glDisable (GL_LIGHTING);
		glDisable (GL_DEPTH_TEST);
		glDisable (GL_CULL_FACE);
		glDisable (GL_TEXTURE_2D);
		glDepthMask (GL_FALSE);
		glColor3f (1.0f, 0.0f, 0.0f);
		LIENG_FOREACH_SELECTION (iter, client->engine)
		{
			object = liren_scene_find_object (client->scene, iter.object->id);
			if (object != NULL)
				liren_draw_bounds (&context, object, NULL);
		}
		liren_context_unbind (&context);
	}

	/* Render custom 3D scene. */
	lical_callbacks_call (client->callbacks, client->engine, "render-3d", lical_marshal_DATA_PTR, self);

	/* Set 2D mode. */
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, rect.width, 0, rect.height, -100.0f, 100.0f);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	/* Render custom 2D scene. */
	lical_callbacks_call (client->callbacks, client->engine, "render-2d", lical_marshal_DATA_PTR, self);
}

static void
private_update (LIWdgRender* self,
                void*        data)
{
	LICliClient* client;
	LIMatMatrix modelview;
	LIMatMatrix projection;

	client = data;
	if (client->network != NULL)
	{
		lialg_camera_get_modelview (client->camera, &modelview);
		lialg_camera_get_projection (client->camera, &projection);
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
static void Scene_new (LIScrArgs* args)
{
	LICliClient* client;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_SCENE);
	self = liwdg_render_new (client->widgets, client->scene);
	if (self == NULL)
		return;
	LIWDG_RENDER (self)->custom_update_func = private_update;
	LIWDG_RENDER (self)->custom_update_data = client;
	LIWDG_RENDER (self)->custom_render_func = private_render;
	LIWDG_RENDER (self)->custom_render_data = client;

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
static void Scene_pick (LIScrArgs* args)
{
	int x;
	int y;
	LICliClient* client;
	LIEngObject* object;
	LIRenSelection result;

	client = LIWDG_RENDER (args->self)->custom_render_data;
	client->video.SDL_GetMouseState (&x, &y);
	liscr_args_gets_int (args, "x", &x);
	liscr_args_gets_int (args, "y", &y);

	/* Pick object from scene. */
	if (!liwdg_render_pick (args->self, &result, x, client->window->mode.height - y - 1))
		return;
	object = lieng_engine_find_object (client->engine, result.object);
	liscr_args_seti_vector (args, &result.point);
	liscr_args_seti_data (args, (object != NULL)? object->script : NULL);
}

/*****************************************************************************/

void
licli_script_scene (LIScrClass* self,
                  void*       data)
{
	liscr_class_inherit (self, licli_script_group, data);
	liscr_class_set_userdata (self, LICLI_SCRIPT_SCENE, data);
	liscr_class_insert_cfunc (self, "new", Scene_new);
	liscr_class_insert_mfunc (self, "pick", Scene_pick);
}

/** @} */
/** @} */
