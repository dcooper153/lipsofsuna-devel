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
 * \addtogroup liwdgWidget Widget
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <class/lips-class.h>
#include "widget.h"
#include "widget-group.h"
#include "widget-window.h"

static int
private_init (liwdgWidget*  self,
              liwdgManager* manager);

static void
private_free (liwdgWidget* self);

static int
private_event (liwdgWidget* self,
               liwdgEvent*  event);

void
private_paint_row (liwdgWidget* self,
                   float        px,
                   float        py,
                   float*       tx,
                   float*       ty,
                   float*       w,
                   float        h,
                   int          repeat);

const liwdgWidgetClass liwdgWidgetType =
{
	LI_CLASS_BASE_STATIC, NULL, "Widget", sizeof (liwdgWidget),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event,
};

/*****************************************************************************/

/**
 * \brief Frees the widget.
 *
 * This function can be called for all inherited classes.
 *
 * \param self Widget.
 */
void
liwdg_widget_free (liwdgWidget* self)
{
	assert (self->manager->focus.keyboard != self);
	assert (self->manager->focus.mouse != self);

	li_instance_free (self, self->type);
	free (self);
}

/**
 * \brief Unparents the widget and removes from the dialog list.
 *
 * If the widget is a child widget of another widget, it is removed from the
 * parent container. If it is the root widget of the widget manager or a dialog,
 * it is removed from the manager.
 *
 * \param self Widget.
 * \return Nonzero if was detached from something.
 */
int
liwdg_widget_detach (liwdgWidget* self)
{
	int changed = 0;

	switch (self->state)
	{
		/* Remove from parent container. */
		case LIWDG_WIDGET_STATE_DETACHED:
			if (self->parent != NULL)
			{
				assert (li_instance_typeis (self->parent, &liwdgGroupType));
				liwdg_group_detach_child (LIWDG_GROUP (self->parent), self);
				changed = 1;
			}
			break;

		/* Remove from popup stack. */
		case LIWDG_WIDGET_STATE_POPUP:
			liwdg_manager_remove_popup (self->manager, self);
			changed = 1;
			break;

		/* Remove from manager root. */
		case LIWDG_WIDGET_STATE_ROOT:
			assert (liwdg_manager_get_root (self->manager) == self);
			liwdg_manager_set_root (self->manager, NULL);
			changed = 1;
			break;

		/* Remove from window stack. */
		case LIWDG_WIDGET_STATE_WINDOW:
			liwdg_manager_remove_window (self->manager, self);
			changed = 1;
			break;
	}
	self->state = LIWDG_WIDGET_STATE_DETACHED;
	liwdg_manager_fix_focus (self->manager);

	return changed;
}

/**
 * \brief Handles an event.
 *
 * \param self Widget.
 * \param event Event.
 * \return Nonzero if passed through unhandled, zero if absorbed by the widget.
 */
int
liwdg_widget_event (liwdgWidget* self,
                    liwdgEvent*  event)
{
	return self->type->event (self, event);
}

licalHandle
liwdg_widget_insert_callback (liwdgWidget* self,
                              licalType    type,
                              int          priority,
                              void*        call,
                              void*        data)
{
	return lical_callbacks_insert_callback (self->callbacks, type, priority, call, data);
}

void
liwdg_widget_move (liwdgWidget* self,
                   int          x,
                   int          y)
{
	liwdg_widget_set_allocation (self, x, y,
		self->allocation.width,
		self->allocation.height);
}

/**
 * \brief Paints widget graphics.
 *
 * \param self Widget.
 * \param style Style identifier.
 * \param rect Rectangle or NULL for the allocation of the widget.
 */
void
liwdg_widget_paint (liwdgWidget* self,
                    const char*  style,
                    liwdgRect*   rect)
{
	int y;
	int px;
	int py;
	int repeatx;
	int repeaty;
	float w[3];
	float h[3];
	float tx[4];
	float ty[4];
	liwdgSubimage* sub;

	/* Get widget subimage. */
	if (rect == NULL)
		rect = &self->allocation;
	sub = lialg_strdic_find (self->manager->subimgs, style);
	if (sub == NULL)
		return;

	/* Calculate repeat counts. */
	w[0] = sub->w[0];
	w[1] = LI_MAX (1, sub->w[1]);
	w[2] = sub->w[2];
	h[0] = sub->h[0];
	h[1] = LI_MAX (1, sub->h[1]);
	h[2] = sub->h[2];
	repeatx = LI_MAX (0, rect->width - w[0] - w[2]);
	repeaty = LI_MAX (0, rect->height - h[0] - h[2]);
	repeatx = (int) floor ((float) repeatx / w[1]);
	repeaty = (int) floor ((float) repeaty / h[1]);

	/* Calculate texture coordinates. */
	tx[0] = (float)(sub->x) / sub->texture->width;
	tx[1] = (float)(sub->x + sub->w[0]) / sub->texture->width;
	tx[2] = (float)(sub->x + sub->w[0] + sub->w[1]) / sub->texture->width;
	tx[3] = (float)(sub->x + sub->w[0] + sub->w[1] + sub->w[2]) / sub->texture->width;
	ty[3] = (float)(sub->y) / sub->texture->height;
	ty[2] = (float)(sub->y + sub->h[2]) / sub->texture->height;
	ty[1] = (float)(sub->y + sub->h[2] + sub->h[1]) / sub->texture->height;
	ty[0] = (float)(sub->y + sub->h[2] + sub->h[1] + sub->h[0]) / sub->texture->height;

	/* Draw the grid. */
	glBindTexture (GL_TEXTURE_2D, sub->texture->texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glColor3f (1.0f, 1.0f, 1.0f);
	px = rect->x;
	py = rect->y;
	private_paint_row (self, px, py, tx, ty + 0, w, h[0], repeatx);
	py += h[0];
	for (y = 0 ; y < repeaty ; y++)
	{
		private_paint_row (self, px, py, tx, ty + 1, w, h[1], repeatx);
		py += h[1];
	}
	private_paint_row (self, px, py, tx, ty + 2, w, h[2], repeatx);
}

void
liwdg_widget_update (liwdgWidget* self,
                     float        secs)
{
	liwdgEvent event;

	event.type = LIWDG_EVENT_TYPE_UPDATE;
	event.update.secs = secs;
	self->type->event (self, &event);
}

int
liwdg_widget_register_callback (liwdgWidget* self,
                                licalType    type,
                                licalMarshal marshal)
{
	return lical_callbacks_insert_type (self->callbacks, type, marshal);
}

void
liwdg_widget_remove_callback (liwdgWidget* self,
                              licalType    type,
                              licalHandle  handle)
{
	lical_callbacks_remove_callback (self->callbacks, type, handle);
}

void
liwdg_widget_render (liwdgWidget* self)
{
	liwdgEvent event;

	if (self->visible)
	{
		event.type = LIWDG_EVENT_TYPE_RENDER;
		self->type->event (self, &event);
	}
}

void
liwdg_widget_get_allocation (liwdgWidget* self,
                             liwdgRect*   allocation)
{
	*allocation = self->allocation;
}

void
liwdg_widget_set_allocation (liwdgWidget* self,
                             int          x,
                             int          y,
                             int          w,
                             int          h)
{
	liwdgEvent event;

	self->allocation.x = x;
	self->allocation.y = y;
	self->allocation.width = w;
	self->allocation.height = h;
	event.type = LIWDG_EVENT_TYPE_ALLOCATION;
	liwdg_widget_event (self, &event);
}

/**
 * \brief Gets the mouse focus state of the widget.
 *
 * \param self Widget.
 * \return Nonzero if the widget has mouse focus.
 */
int
liwdg_widget_get_focus_mouse (liwdgWidget* self)
{
	return (liwdg_manager_get_focus_mouse (self->manager) == self);
}

/**
 * \brief Gives mouse focus to the widget.
 *
 * \param self Widget.
 */
void
liwdg_widget_set_focus_mouse (liwdgWidget* self)
{
	liwdg_manager_set_focus_mouse (self->manager, self);
}

/**
 * \brief Gets the keyboard focus state of the widget.
 *
 * \param self Widget.
 * \return Nonzero if the widget has keyboard focus.
 */
int
liwdg_widget_get_focus_keyboard (liwdgWidget* self)
{
	return (liwdg_manager_get_focus_keyboard (self->manager) == self);
}

/**
 * \brief Gives keyboard focus to the widget.
 *
 * \param self Widget.
 */
void
liwdg_widget_set_focus_keyboard (liwdgWidget* self)
{
	liwdg_manager_set_focus_keyboard (self->manager, self);
}

int
liwdg_widget_get_focusable (liwdgWidget* self)
{
	return self->focusable;
}

void
liwdg_widget_set_focusable (liwdgWidget* self,
                            int          focusable)
{
	self->focusable = focusable;
}

void
liwdg_widget_get_request (liwdgWidget* self,
                          liwdgSize*   request)
{
	*request = self->request;
}

void
liwdg_widget_set_request (liwdgWidget* self,
                          int          w,
                          int          h)
{
	self->request.width = w;
	self->request.height = h;
	if (self->parent != NULL)
		liwdg_group_child_request (LIWDG_GROUP (self->parent), self);
}

liwdgWidget*
liwdg_widget_get_root (liwdgWidget* self)
{
	liwdgWidget* widget;

	for (widget = self ; widget->parent != NULL ; widget = widget->parent) { }
	return widget;
}

void
liwdg_widget_get_style_allocation (liwdgWidget* self,
                                   const char*  style,
                                   liwdgRect*   allocation)
{
	liwdgSubimage* sub;

	/* Get widget subimage. */
	sub = lialg_strdic_find (self->manager->subimgs, style);
	if (sub == NULL)
	{
		*allocation = self->allocation;
		return;
	}

	/* Subtract paddings from the allocation. */
	allocation->x = self->allocation.x + sub->pad[1];
	allocation->y = self->allocation.y + sub->pad[3];
	allocation->width = self->allocation.width - sub->pad[1] - sub->pad[2];
	allocation->height = self->allocation.height - sub->pad[3] - sub->pad[0];
}

void
liwdg_widget_get_style_request (liwdgWidget* self,
                                const char*  style,
                                liwdgSize*   rect)
{
	int w = self->request.width;
	int h = self->request.height;
	int pw[3];
	int ph[3];
	int repeatx;
	int repeaty;
	liwdgSubimage* sub;

	/* Get widget subimage. */
	sub = lialg_strdic_find (self->manager->subimgs, style);
	if (sub == NULL)
	{
		rect->width = w;
		rect->height = h;
		return;
	}

	/* Calculate repeat counts. */
	pw[0] = sub->w[0];
	pw[1] = LI_MAX (1, sub->w[1]);
	pw[2] = sub->w[2];
	ph[0] = sub->h[0];
	ph[1] = LI_MAX (1, sub->h[1]);
	ph[2] = sub->h[2];
	repeatx = LI_MAX (0, w - pw[0] - pw[2] + sub->pad[1] + sub->pad[2]);
	repeaty = LI_MAX (0, h - ph[0] - ph[2] + sub->pad[0] + sub->pad[3]);
	repeatx = (int) ceil ((float) repeatx / pw[1]);
	repeaty = (int) ceil ((float) repeaty / ph[1]);

	/* Calculate suitable request. */
	rect->width = pw[0] + repeatx * pw[1] + pw[2];
	rect->height = ph[0] + repeaty * ph[1] + ph[2];
}

void
liwdg_widget_set_style_request (liwdgWidget* self,
                                int          w,
                                int          h,
                                const char*  style)
{
	liwdgSize size;

	/* Set request suitable for the style. */
	self->request.width = w;
	self->request.height = h;
	liwdg_widget_get_style_request (self, style, &size);
	liwdg_widget_set_request (self, size.width, size.height);
}

void*
liwdg_widget_get_userdata (liwdgWidget* self)
{
	return self->userdata;
}

void
liwdg_widget_set_userdata (liwdgWidget* self,
                           void*        value)
{
	self->userdata = value;
}

int
liwdg_widget_get_visible (liwdgWidget* self)
{
	return self->visible;
}

void
liwdg_widget_set_visible (liwdgWidget* self,
                          int          visible)
{
	self->visible = (visible != 0);
	if (self->parent != NULL)
		liwdg_group_child_request (LIWDG_GROUP (self->parent), self);
	if (self->state == LIWDG_WIDGET_STATE_POPUP)
		liwdg_manager_remove_popup (self->manager, self);
	if (!visible)
		liwdg_manager_fix_focus (self->manager);
}

/*****************************************************************************/

static int
private_init (liwdgWidget*  self,
              liwdgManager* manager)
{
	self->manager = manager;
	self->visible = 1;
	self->callbacks = lical_callbacks_new ();
	if (self->callbacks == NULL)
		return 0;
	return 1;
}

static void
private_free (liwdgWidget* self)
{
	lical_callbacks_free (self->callbacks);
}

static int
private_event (liwdgWidget* self,
               liwdgEvent*  event)
{
	if (event->type == LIWDG_EVENT_TYPE_CLOSE)
	{
		assert (self->state == LIWDG_WIDGET_STATE_POPUP);
		liwdg_manager_remove_popup (self->manager, self);
		return 1;
	}

	return 0;
}

void
private_paint_row (liwdgWidget* self,
                   float        px,
                   float        py,
                   float*       tx,
                   float*       ty,
                   float*       w,
                   float        h,
                   int          repeat)
{
	int x;

	/* Draw left border. */
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[0], ty[0]); glVertex2f (px, py);
	glTexCoord2f (tx[1], ty[0]); glVertex2f (px + w[0], py);
	glTexCoord2f (tx[0], ty[1]); glVertex2f (px, py + h);
	glTexCoord2f (tx[1], ty[1]); glVertex2f (px + w[0], py + h);
	glEnd ();
	px += w[0];

	/* Draw fill. */
	for (x = 0 ; x < repeat ; x++)
	{
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (tx[1], ty[0]); glVertex2f (px, py);
		glTexCoord2f (tx[2], ty[0]); glVertex2f (px + w[1], py);
		glTexCoord2f (tx[1], ty[1]); glVertex2f (px, py + h);
		glTexCoord2f (tx[2], ty[1]); glVertex2f (px + w[1], py + h);
		glEnd ();
		px += w[1];
	}

	/* Draw right border. */
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[2], ty[0]); glVertex2f (px, py);
	glTexCoord2f (tx[3], ty[0]); glVertex2f (px + w[2], py);
	glTexCoord2f (tx[2], ty[1]); glVertex2f (px, py + h);
	glTexCoord2f (tx[3], ty[1]); glVertex2f (px + w[2], py + h);
	glEnd ();
}

/** @} */
/** @} */
