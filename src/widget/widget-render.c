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
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Render", sizeof (liwdgRender),
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

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			return lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_PRESSED, self);
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			glMatrixMode (GL_PROJECTION);
			glPushMatrix ();
			glMatrixMode (GL_MODELVIEW);
			glPushMatrix ();
			glPushAttrib (GL_VIEWPORT_BIT | GL_ENABLE_BIT);
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
			glViewport (rect.x, rect.y, rect.width, rect.height);
			glScissor (rect.x, rect.y, rect.width, rect.height);
			glEnable (GL_SCISSOR_TEST);
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
			return 1;
	}

	return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
}

/** @} */
/** @} */
