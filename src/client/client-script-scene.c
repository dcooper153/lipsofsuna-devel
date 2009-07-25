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

	module = data;
	if (module->network != NULL)
	{
		glDisable (GL_LIGHTING);
		glDisable (GL_DEPTH_TEST);
		glDisable (GL_CULL_FACE);
		glDisable (GL_TEXTURE_2D);
		glDepthMask (GL_FALSE);
		glColor3f (1.0f, 0.0f, 0.0f);
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();
		lieng_camera_get_frustum (module->camera, &frustum);
		lieng_camera_get_modelview (module->camera, &modelview);
		lieng_camera_get_projection (module->camera, &projection);
		lirnd_context_init (&context, module->engine->scene);
		lirnd_context_set_modelview (&context, &modelview);
		lirnd_context_set_projection (&context, &projection);
		lirnd_context_set_frustum (&context, &frustum);
		LIENG_FOREACH_SELECTION (iter, module->engine)
		{
			if (iter.object->render != NULL)
				lirnd_draw_bounds (&context, iter.object->render, NULL);
		}
		lirnd_context_unbind (&context);
	}
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
		lieng_camera_get_modelview (module->camera, &modelview);
		lieng_camera_get_projection (module->camera, &projection);
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
	widget = liwdg_render_new (module->widgets, module->engine->scene);
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

/*****************************************************************************/

void
licliSceneScript (liscrClass* self,
                  void*       data)
{
	liscr_class_inherit (self, licliGroupScript, data);
	liscr_class_set_userdata (self, LICLI_SCRIPT_SCENE, data);
	liscr_class_insert_func (self, "new", Scene_new);
}

/** @} */
/** @} */
