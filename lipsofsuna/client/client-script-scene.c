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
 * -- @param table Optional table of parameters.
 * -- @return New scene widget.
 * function Scene.new(self, table)
 */
static int
Scene_new (lua_State* lua)
{
	licliModule* module;
	liscrData* self;
	liscrScript* script;
	liwdgWidget* widget;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_SCENE);

	/* Allocate widget. */
	widget = liwdg_render_new (module->widgets, module->scene);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	LIWDG_RENDER (widget)->custom_update_func = private_update;
	LIWDG_RENDER (widget)->custom_update_data = module;
	LIWDG_RENDER (widget)->custom_render_func = private_render;
	LIWDG_RENDER (widget)->custom_render_data = module;

	/* Allocate userdata. */
	self = liscr_data_new (script, widget, LICLI_SCRIPT_SCENE);
	if (self == NULL)
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
	}

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liwdg_widget_set_userdata (widget, self);
	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);

	return 1;
}

/* @luadoc
 * ---
 * -- Pick an object from the scene.
 * --
 * -- @param self Scene.
 * -- @param x Optional X coordinate, default is cursor position.
 * -- @param y Optional Y coordinate, default is cursor position.
 * -- @return Object or nil, vector or nil.
 * function Scene.pick(self, x, y)
 */
static int
Scene_pick (lua_State* lua)
{
	int x;
	int y;
	licliModule* module;
	liengObject* object;
	lirndSelection result;
	liscrData* self;
	liscrData* vector;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_SCENE);
	module = LIWDG_RENDER (self->data)->custom_render_data;
	module->client->video.SDL_GetMouseState (&x, &y);
	if (lua_gettop (lua) >= 2)
		x = luaL_checknumber (lua, 2);
	if (lua_gettop (lua) >= 3)
		y = luaL_checknumber (lua, 3);

	/* Pick objects from the scene. */
	if (!liwdg_render_pick (self->data, &result, x, module->window->mode.height - y - 1))
		return 0;

	/* Find the picked object. */
	object = lieng_engine_find_object (module->engine, result.object);
	if (object == NULL || object->script == NULL)
		lua_pushnil (lua);
	else
		liscr_pushdata (lua, object->script);
	vector = liscr_vector_new (module->script, &result.point);
	if (vector != NULL)
		liscr_pushdata (lua, vector);

	return 2;
}

/*****************************************************************************/

void
licliSceneScript (liscrClass* self,
                  void*       data)
{
	liscr_class_inherit (self, licliGroupScript, data);
	liscr_class_set_userdata (self, LICLI_SCRIPT_SCENE, data);
	liscr_class_insert_func (self, "new", Scene_new);
	liscr_class_insert_func (self, "pick", Scene_pick);
}

/** @} */
/** @} */
