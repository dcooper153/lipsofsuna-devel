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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup LIWdgRender Render
 * @{
 */

#include "widget-render.h"
#include "widget-manager.h"

static int
private_init (LIWdgRender*   self,
              LIWdgManager* manager);

static void
private_free (LIWdgRender* self);

static int
private_event (LIWdgRender* self,
               LIWdgEvent* event);

/****************************************************************************/

const LIWdgClass*
liwdg_widget_render ()
{
	static const LIWdgClass clss =
	{
		liwdg_widget_group, "Render", sizeof (LIWdgRender),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	return &clss;
}

/**
 * \brief Creates a new scene renderer widget.
 *
 * \param manager Widget manager.
 * \param scene Scene.
 * \return New widget or NULL.
 */
LIWdgWidget*
liwdg_render_new (LIWdgManager* manager,
                  LIRenScene*   scene)
{
	LIWdgWidget* self;

	self = liwdg_widget_new (manager, liwdg_widget_render ());
	if (self == NULL)
		return NULL;
	LIWDG_RENDER (self)->scene = scene;
	LIWDG_RENDER (self)->deferred = liren_deferred_new (scene->render, 32, 32);
	if (LIWDG_RENDER (self)->deferred == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}

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
liwdg_render_pick (LIWdgRender*    self,
                   LIRenSelection* result,
                   int             x,
                   int             y)
{
	int viewport[4];
	LIMatFrustum frustum;
	LIWdgRect rect;

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
	y = LIWDG_WIDGET (self)->manager->height - y - 1;
	if (!liren_scene_pick (self->scene, &self->modelview, &self->projection, &frustum, viewport, x, y, 10, result))
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
liwdg_render_set_modelview (LIWdgRender*       self,
                            const LIMatMatrix* value)
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
liwdg_render_set_projection (LIWdgRender*       self,
                             const LIMatMatrix* value)
{
	self->projection = *value;
}

/****************************************************************************/

static int
private_init (LIWdgRender*  self,
              LIWdgManager* manager)
{
	liwdg_widget_set_style (LIWDG_WIDGET (self), "render");
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self), 128, 128);
	self->modelview = limat_matrix_identity ();
	self->projection = limat_matrix_perspective (45.0f, 1.0f, 1.0f, 100.0f);

	return 1;
}

static void
private_free (LIWdgRender* self)
{
	if (self->deferred != NULL)
		liren_deferred_free (self->deferred);
}

static int
private_event (LIWdgRender* self,
               LIWdgEvent*  event)
{
	int i;
	LIMatFrustum frustum;
	LIWdgManager* manager;
	LIWdgRect rect;
	LIWdgStyle* style;
	LIWdgWidget* child;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_ALLOCATION:
			if (self->deferred != NULL)
			{
				liren_deferred_resize (self->deferred,
					LIWDG_WIDGET (self)->allocation.width,
					LIWDG_WIDGET (self)->allocation.height);
			}
			break;
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			if (!lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "pressed", lical_marshal_DATA_PTR, self))
				return 0;
			break;
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			break;
		case LIWDG_EVENT_TYPE_RENDER:
			/* Draw base. */
			manager = LIWDG_WIDGET (self)->manager;
			style = liwdg_widget_get_style (LIWDG_WIDGET (self));
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			/* Draw scene. */
			glMatrixMode (GL_PROJECTION);
			glPushMatrix ();
			glMatrixMode (GL_MODELVIEW);
			glPushMatrix ();
			glPushAttrib (GL_VIEWPORT_BIT | GL_ENABLE_BIT);
			glViewport (rect.x, manager->height - rect.y - rect.height, rect.width, rect.height);
			glScissor (rect.x, manager->height - rect.y - rect.height, rect.width, rect.height);
			glEnable (GL_SCISSOR_TEST);
			glDepthMask (GL_TRUE);
			glClear (GL_DEPTH_BUFFER_BIT);
			if (self->scene != NULL)
			{
				limat_frustum_init (&frustum, &self->modelview, &self->projection);
				liren_scene_render_begin (self->scene, self->deferred, &self->modelview, &self->projection, &frustum);
				liren_scene_render_deferred_opaque (self->scene, 0, 0.0f);
				liren_scene_render_forward_transparent (self->scene);
				liren_scene_render_postproc (self->scene);
				liren_scene_render_end (self->scene);
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
			glDisable (GL_DEPTH_TEST);
			glDepthMask (GL_FALSE);
			/* Draw children. */
			for (i = 0 ; i < LIWDG_GROUP (self)->width * LIWDG_GROUP (self)->height ; i++)
			{
				child = LIWDG_GROUP (self)->cells[i].child;
				if (child != NULL)
					liwdg_widget_draw (child);
			}
			return 1;
		case LIWDG_EVENT_TYPE_UPDATE:
			if (self->custom_update_func != NULL)
				self->custom_update_func (self, self->custom_update_data);
			break;
	}

	return liwdg_widget_group ()->event (LIWDG_WIDGET (self), event);
}

/** @} */
/** @} */
