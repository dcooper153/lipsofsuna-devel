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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup liwdgRender Render
 * @{
 */

#include "widget-render.h"
#include "widget-manager.h"

static int
private_init (liwdgRender*   self,
              liwdgManager* manager);

static void
private_free (liwdgRender* self);

static int
private_event (liwdgRender* self,
               liwdgEvent* event);

const liwdgClass liwdgRenderType =
{
	LIWDG_BASE_STATIC, &liwdgGroupType, "Render", sizeof (liwdgRender),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/****************************************************************************/

/**
 * \brief Creates a new scene renderer widget.
 *
 * \param manager Widget manager.
 * \param scene Scene.
 * \return New widget or NULL.
 */
liwdgWidget*
liwdg_render_new (liwdgManager* manager,
                  lirndScene*   scene)
{
	liwdgWidget* self;

	self = liwdg_widget_new (manager, &liwdgRenderType);
	if (self == NULL)
		return NULL;
	LIWDG_RENDER (self)->scene = scene;

	return self;
}

/**
 * \brief Picks the scene of the widget.
 *
 * \param self Renderer widget.
 * \param result Return location for the picking result.
 * \param x Point in screen space.
 * \param y Point in screen space.
 * \return Nonzero if found something.
 */
int
liwdg_render_pick (liwdgRender*    self,
                   lirndSelection* result,
                   int             x,
                   int             y)
{
	int viewport[4];
	limatFrustum frustum;
	liwdgRect rect;

	/* Check if inside the allocation. */
	liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
	if (x < rect.x || x >= rect.x + rect.width ||
	    y < rect.y || y >= rect.y + rect.height)
		return 0;

	/* Pick objects from the scene. */
	if (self->scene == NULL)
		return 0;
	limat_frustum_init (&frustum, &self->modelview, &self->projection);
	viewport[0] = rect.x;
	viewport[1] = rect.y;
	viewport[2] = rect.width;
	viewport[3] = rect.height;
	if (!lirnd_scene_pick (self->scene, &self->modelview, &self->projection, &frustum, viewport, x, y, 5, result))
		return 0;

	return 1;
}

/**
 * \brief Sets the modelview matrix of the rendered scene.
 *
 * \param self Renderer widget.
 * \param value Matrix.
 */
void
liwdg_render_set_modelview (liwdgRender*       self,
                            const limatMatrix* value)
{
	self->modelview = *value;
}

/**
 * \brief Sets the projection matrix of the rendered scene.
 *
 * \param self Renderer widget.
 * \param value Matrix.
 */
void
liwdg_render_set_projection (liwdgRender*       self,
                             const limatMatrix* value)
{
	self->projection = *value;
}

/****************************************************************************/

static int
private_init (liwdgRender*  self,
              liwdgManager* manager)
{
	if (!liwdg_widget_register_callback (LIWDG_WIDGET (self), LIWDG_CALLBACK_PRESSED, lical_marshal_DATA_PTR))
		return 0;
	liwdg_widget_set_style_request (LIWDG_WIDGET (self), 128, 128, "render");
	self->modelview = limat_matrix_identity ();
	self->projection = limat_matrix_perspective (45.0f, 1.0f, 1.0f, 100.0f);

	return 1;
}

static void
private_free (liwdgRender* self)
{
}

static int
private_event (liwdgRender* self,
               liwdgEvent*  event)
{
	limatFrustum frustum;
	liwdgRect rect;
	liwdgStyle* style;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			if (!lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_PRESSED, self))
				return 0;
			break;
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			break;
		case LIWDG_EVENT_TYPE_RENDER:
			/* Draw base. */
			style = liwdg_widget_get_style (LIWDG_WIDGET (self), "render");
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "render", &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), "render", NULL);
			/* Draw scene. */
			glMatrixMode (GL_PROJECTION);
			glPushMatrix ();
			glMatrixMode (GL_MODELVIEW);
			glPushMatrix ();
			glPushAttrib (GL_VIEWPORT_BIT | GL_ENABLE_BIT);
			glViewport (rect.x, rect.y, rect.width, rect.height);
			glScissor (rect.x, rect.y, rect.width, rect.height);
			glEnable (GL_SCISSOR_TEST);
			glDepthMask (GL_TRUE);
			glClear (GL_DEPTH_BUFFER_BIT);
			if (self->scene != NULL)
			{
				limat_frustum_init (&frustum, &self->modelview, &self->projection);
				lirnd_scene_render (self->scene, &self->modelview, &self->projection, &frustum);
			}
			if (self->custom_render_func != NULL)
			{
				glMatrixMode (GL_PROJECTION);
				glLoadMatrixf (self->projection.m);
				glMatrixMode (GL_MODELVIEW);
				glLoadMatrixf (self->modelview.m);
				self->custom_render_func (self, self->custom_render_data);
			}
			glDisable (GL_SCISSOR_TEST);
			glPopAttrib ();
			glMatrixMode (GL_PROJECTION);
			glPopMatrix ();
			glMatrixMode (GL_MODELVIEW);
			glPopMatrix ();
			glEnable (GL_BLEND);
			glEnable (GL_TEXTURE_2D);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable (GL_CULL_FACE);
			glDisable (GL_LIGHTING);
			glDisable (GL_DEPTH_TEST);
			glDepthMask (GL_FALSE);
			break;
		case LIWDG_EVENT_TYPE_UPDATE:
			if (self->custom_update_func != NULL)
				self->custom_update_func (self, self->custom_update_data);
			break;
	}

	return liwdgGroupType.event (LIWDG_WIDGET (self), event);
}

/** @} */
/** @} */
