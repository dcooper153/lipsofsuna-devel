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
 * \addtogroup LIWdg Widget
 * @{
 * \addtogroup LIWdgWidget Widget
 * @{
 */

#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-group.h"

static int
private_new (LIWdgWidget*      self,
             const LIWdgClass* clss,
             LIWdgManager*     manager);

static int
private_init (LIWdgWidget*  self,
              LIWdgManager* manager);

static void
private_free (LIWdgWidget* self);

static int
private_event (LIWdgWidget* self,
               LIWdgEvent*  event);

static void
private_rebuild_style (LIWdgWidget* self);

/*****************************************************************************/

const LIWdgClass*
liwdg_widget_widget ()
{
	static const LIWdgClass clss =
	{
		NULL, "Widget", sizeof (LIWdgWidget),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	return &clss;
}

LIWdgWidget*
liwdg_widget_new (LIWdgManager*     manager,
                  const LIWdgClass* clss)
{
	LIWdgWidget* self;

	self = lisys_calloc (1, clss->size);
	if (self == NULL)
		return 0;
	self->manager = manager;
	self->type = clss;
	if (!private_new (self, clss, manager))
		return 0;

	return self;
}

/**
 * \brief Frees the widget.
 *
 * This function can be called for all inherited classes.
 *
 * \param self Widget.
 */
void
liwdg_widget_free (LIWdgWidget* self)
{
	const LIWdgClass* ptr;

	lical_callbacks_call (self->manager->callbacks, self->manager, "widget-free", lical_marshal_DATA_PTR, self);
	for (ptr = self->type ; ptr != NULL ; ptr = liwdg_class_get_base (ptr))
	{
		if (ptr->free != NULL)
			ptr->free (self);
	}
	lisys_free (self);
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
liwdg_widget_detach (LIWdgWidget* self)
{
	int changed = 0;

	if (self->floating)
	{
		liwdg_manager_remove_window (self->manager, self);
		self->floating = 0;
		changed = 1;
	}
	else if (self->parent != NULL)
	{
		lisys_assert (liwdg_widget_typeis (self->parent, liwdg_widget_container ()));
		liwdg_container_detach_child (LIWDG_CONTAINER (self->parent), self);
		lisys_assert (self->parent == NULL);
		changed = 1;
	}

	return changed;
}

void
liwdg_widget_draw (LIWdgWidget* self)
{
	LIWdgEvent event;

	if (self->visible)
	{
		event.type = LIWDG_EVENT_TYPE_RENDER;
		self->type->event (self, &event);
	}
}

/**
 * \brief Handles an event.
 *
 * \param self Widget.
 * \param event Event.
 * \return Nonzero if passed through unhandled, zero if absorbed by the widget.
 */
int
liwdg_widget_event (LIWdgWidget* self,
                    LIWdgEvent*  event)
{
	return self->type->event (self, event);
}

int
liwdg_widget_insert_callback (LIWdgWidget* self,
                              const char*  type,
                              void*        func,
                              void*        data)
{
	return lical_callbacks_insert (self->manager->callbacks, self, type, 0, func, data, NULL);
}

int
liwdg_widget_insert_callback_full (LIWdgWidget* self,
                                   const char*  type,
                                   int          priority,
                                   void*        func,
                                   void*        data,
                                   LICalHandle* handle)
{
	return lical_callbacks_insert (self->manager->callbacks, self, type, priority, func, data, handle);
}

void
liwdg_widget_move (LIWdgWidget* self,
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
 * \param rect Rectangle or NULL for the allocation of the widget.
 */
void
liwdg_widget_paint (LIWdgWidget* self,
                    LIWdgRect*   rect)
{
	if (self->style == NULL)
		return;
	if (rect == NULL)
		rect = &self->allocation;
	liwdg_style_paint (self->style, rect);
}

/**
 * \brief Calls the custom paint method of the widget.
 * \param self Widget.
 */
void liwdg_widget_paint_custom (
	LIWdgWidget* self)
{
	lical_callbacks_call (self->manager->callbacks, self, "paint", lical_marshal_DATA_PTR, self);
}

/**
 * \brief Translates coordinates from screen space to widget space.
 *
 * Coordinate translation is needed when widgets are inside a scrollable viewport.
 * For example, if a widget wishes to check if screen space pointer coordinates
 * are inside a specific area of the widget, it needs to translate the coordinates
 * first with this function.
 *
 * \param self Widget.
 * \param screenx Coordinates in screen space.
 * \param screeny Coordinates in screen space.
 * \param widgetx Coordinates in widget space.
 * \param widgety Coordinates in widget space.
 */
void
liwdg_widget_translate_coords (LIWdgWidget* self,
                               int          screenx,
                               int          screeny,
                               int*         widgetx,
                               int*         widgety)
{
	*widgetx = screenx;
	*widgety = screeny;
	if (self->parent != NULL)
		liwdg_widget_translate_coords (self->parent, screenx, screeny, widgetx, widgety);
	if (liwdg_widget_typeis (self, liwdg_widget_container ()))
		liwdg_container_translate_coords (LIWDG_CONTAINER (self), screenx, screeny, widgetx, widgety);
}

/**
 * \brief Checks if the widget implements the given class.
 *
 * \param self Widget.
 * \param clss Class.
 * \return Nonzero if implements.
 */
int
liwdg_widget_typeis (const LIWdgWidget* self,
                     const LIWdgClass*  clss)
{
	const LIWdgClass* ptr;

	for (ptr = self->type ; ptr != NULL ; ptr = liwdg_class_get_base (ptr))
	{
		if (clss == ptr)
			return 1;
	}

	return 0;
}

/**
 * \brief Calls the type specific update handler of the widget.
 *
 * \param self Widget.
 * \param secs Seconds since last update.
 */
void
liwdg_widget_update (LIWdgWidget* self,
                     float        secs)
{
	LIWdgEvent event;

	event.type = LIWDG_EVENT_TYPE_UPDATE;
	event.update.secs = secs;
	self->type->event (self, &event);
}

void
liwdg_widget_get_allocation (LIWdgWidget* self,
                             LIWdgRect*   allocation)
{
	*allocation = self->allocation;
}

void
liwdg_widget_set_allocation (LIWdgWidget* self,
                             int          x,
                             int          y,
                             int          w,
                             int          h)
{
	LIWdgEvent event;

	if (self->allocation.x != x ||
	    self->allocation.y != y ||
	    self->allocation.width != w ||
	    self->allocation.height != h)
	{
		self->allocation.x = x;
		self->allocation.y = y;
		self->allocation.width = w;
		self->allocation.height = h;
		event.type = LIWDG_EVENT_TYPE_ALLOCATION;
		liwdg_widget_event (self, &event);
	}
}

int
liwdg_widget_get_behind (LIWdgWidget* self)
{
	return self->behind;
}

void
liwdg_widget_set_behind (LIWdgWidget* self,
                         int          value)
{
	self->behind = value;
	if (self->floating)
	{
		if (value)
		{
			/* Move to bottom. */
			if (self != self->manager->dialogs.bottom)
			{
				if (self->next != NULL)
					self->next->prev = self->prev;
				if (self->prev != NULL)
					self->prev->next = self->next;
				self->prev = self->manager->dialogs.bottom;
				if (self->prev != NULL)
					self->prev->next = self;
				self->manager->dialogs.bottom = self;
			}
		}
		else
		{
			/* Move out of bottom. */
			if (self == self->manager->dialogs.bottom && self->prev != NULL)
			{
				self->prev->next = NULL;
				self->manager->dialogs.bottom = self->prev;
				self->next = self->manager->dialogs.bottom;
				self->prev = self->manager->dialogs.bottom->prev;
				self->manager->dialogs.bottom->prev = self;
			}
		}
	}
}

/**
 * \brief Gets the rectangle of the contents of the widget.
 *
 * The content rectangle is the allocation of the widget minus style paddings.
 *
 * \param self Widget.
 * \param allocation Return location for a rectangle.
 */
void
liwdg_widget_get_content (LIWdgWidget* self,
                          LIWdgRect*   allocation)
{
	allocation->x = self->allocation.x + self->style->pad[1];
	allocation->y = self->allocation.y + self->style->pad[0];
	allocation->width = self->allocation.width - self->style->pad[1] - self->style->pad[2];
	allocation->height = self->allocation.height - self->style->pad[0] - self->style->pad[3];
}

int
liwdg_widget_get_floating (LIWdgWidget* self)
{
	return self->floating;
}

void
liwdg_widget_set_floating (LIWdgWidget* self,
                           int          value)
{
	liwdg_widget_detach (self);
	if (value)
	{
		if (liwdg_manager_insert_window (self->manager, self))
		{
			self->floating = 1;
			self->visible = 1;
		}
	}
	else
	{
		self->floating = 0;
		self->visible = 0;
	}
}

int
liwdg_widget_get_focusable (LIWdgWidget* self)
{
	return self->focusable;
}

void
liwdg_widget_set_focusable (LIWdgWidget* self,
                            int          focusable)
{
	self->focusable = focusable;
}

/**
 * \brief Gets the focus state of the widget.
 *
 * \param self Widget.
 * \return Nonzero if the widget has focus.
 */
int
liwdg_widget_get_focused (LIWdgWidget* self)
{
	return (liwdg_manager_get_focus (self->manager) == self);
}

/**
 * \brief Gives focus to the widget.
 *
 * \param self Widget.
 */
void
liwdg_widget_set_focused (LIWdgWidget* self)
{
	if (liwdg_manager_get_focus (self->manager) != self &&
	    liwdg_widget_get_visible (self))
		liwdg_manager_set_focus (self->manager, self);
}

/**
 * \brief Gets the font provided by the style of the widget.
 *
 * \param self Widget.
 * \return Font or NULL.
 */
LIFntFont*
liwdg_widget_get_font (const LIWdgWidget* self)
{
	if (self->style == NULL)
		return NULL;
	return liwdg_manager_find_font (self->manager, self->style->font);
}

int
liwdg_widget_get_fullscreen (LIWdgWidget* self)
{
	return self->fullscreen;
}

void
liwdg_widget_set_fullscreen (LIWdgWidget* self,
                            int           value)
{
	self->fullscreen = value;
}

int
liwdg_widget_get_grab (const LIWdgWidget* self)
{
	return self->manager->widgets.grab == self;
}

void
liwdg_widget_set_grab (LIWdgWidget* self,
                       int          value)
{
	int cx;
	int cy;

	if (value)
	{
		cx = self->manager->width / 2;
		cy = self->manager->height / 2;
		SDL_ShowCursor (SDL_DISABLE);
		SDL_WarpMouse (cx, cy);
		SDL_WM_GrabInput (SDL_GRAB_OFF);
		self->manager->widgets.grab = self;
	}
	else
	{
		SDL_ShowCursor (SDL_ENABLE);
		SDL_WM_GrabInput (SDL_GRAB_OFF);
		self->manager->widgets.grab = NULL;
	}
}

/**
 * \brief Gets the full size request of the widget.
 *
 * Returns the larger of the user and hard size requests, combined with the
 * style paddings of the widget.
 *
 * \param self Widget.
 * \param request Return location for the size.
 */
void
liwdg_widget_get_request (LIWdgWidget* self,
                          LIWdgSize*   request)
{
	/* Get ordinary request. */
	request->width = self->hardrequest.width;
	request->height = self->hardrequest.height;
	if (self->userrequest.width != -1)
		request->width = LIMAT_MAX (request->width, self->userrequest.width);
	if (self->userrequest.height != -1)
		request->height = LIMAT_MAX (request->height, self->userrequest.height);

	/* Add style paddings. */
	request->width += self->style->pad[1] + self->style->pad[2];
	request->height += self->style->pad[0] + self->style->pad[3];
}

/**
 * \brief Sets or unsets the user overridden size request of the widget.
 *
 * \param self Widget.
 * \param w Width or -1 to unset.
 * \param h Height or -1 to unset.
 */
void
liwdg_widget_set_request (LIWdgWidget* self,
                          int          w,
                          int          h)
{
	if (self->userrequest.width != w ||
	    self->userrequest.height != h)
	{
		self->userrequest.width = w;
		self->userrequest.height = h;
		if (self->parent != NULL)
			liwdg_container_child_request (LIWDG_CONTAINER (self->parent), self);
	}
}

/**
 * \brief Sets the internal size request of the widget.
 *
 * \param self Widget.
 * \param w Width.
 * \param h Height.
 */
void
liwdg_widget_set_request_internal (LIWdgWidget* self,
                                   int          w,
                                   int          h)
{
	if (self->hardrequest.width != w ||
	    self->hardrequest.height != h)
	{
		self->hardrequest.width = w;
		self->hardrequest.height = h;
		if (self->parent != NULL)
			liwdg_container_child_request (LIWDG_CONTAINER (self->parent), self);
	}
}

LIWdgWidget*
liwdg_widget_get_root (LIWdgWidget* self)
{
	LIWdgWidget* widget;

	for (widget = self ; widget->parent != NULL ; widget = widget->parent) { }
	return widget;
}

void
liwdg_widget_set_state (LIWdgWidget* self,
                        const char*  state)
{
	char* tmp;

	/* Early exit. */
	if ((self->state_name == NULL && state == NULL) ||
	    (self->state_name != NULL && state != NULL && !strcmp (state, self->state_name)))
		return;

	/* Store state name. */
	if (state != NULL)
	{
		tmp = listr_dup (state);
		if (tmp != NULL)
		{
			lisys_free (self->state_name);
			self->state_name = tmp;
		}
	}
	else
	{
		lisys_free (self->state_name);
		self->state_name = NULL;
	}

	/* Rebuild style. */
	private_rebuild_style (self);
}

LIWdgStyle*
liwdg_widget_get_style (LIWdgWidget* self)
{
	return self->style;
}

void
liwdg_widget_set_style (LIWdgWidget* self,
                        const char*  style)
{
	char* tmp;

	/* Early exit. */
	if ((self->style_name == NULL && style == NULL) ||
	    (self->style_name != NULL && style != NULL && !strcmp (style, self->style_name)))
		return;

	/* Store style name. */
	if (style != NULL)
	{
		tmp = listr_dup (style);
		if (tmp != NULL)
		{
			lisys_free (self->style_name);
			self->style_name = tmp;
		}
	}
	else
	{
		lisys_free (self->style_name);
		self->style_name = NULL;
	}

	/* Rebuild style. */
	private_rebuild_style (self);
}

int
liwdg_widget_get_temporary (LIWdgWidget* self)
{
	return self->temporary;
}

void
liwdg_widget_set_temporary (LIWdgWidget* self,
                            int          value)
{
	self->temporary = value;
}

void*
liwdg_widget_get_userdata (LIWdgWidget* self)
{
	return self->userdata;
}

void
liwdg_widget_set_userdata (LIWdgWidget* self,
                           void*        value)
{
	self->userdata = value;
}

int
liwdg_widget_get_visible (LIWdgWidget* self)
{
	return self->visible;
}

void
liwdg_widget_set_visible (LIWdgWidget* self,
                          int          visible)
{
	self->visible = (visible != 0);
	if (self->parent != NULL)
		liwdg_container_child_request (LIWDG_CONTAINER (self->parent), self);
	if (self->floating)
		liwdg_manager_remove_window (self->manager, self);
}

/*****************************************************************************/

static int
private_new (LIWdgWidget*      self,
             const LIWdgClass* clss,
             LIWdgManager*     manager)
{
	const LIWdgClass* base;

	/* Initialization. */
	base = liwdg_class_get_base (clss);
	if (base != NULL)
	{
		if (!private_new (self, base, manager))
			return 0;
	}
	else
	{
		lisys_assert (clss == liwdg_widget_widget ());
	}
	if (clss->init == NULL)
		return 1;
	if (clss->init (self, manager))
		return 1;

	/* Error recovery. */
	if (base != NULL)
		self->type = base;
	else
		self->type = liwdg_widget_widget ();
	liwdg_widget_free (self);

	return 0;
}

static int
private_init (LIWdgWidget*  self,
              LIWdgManager* manager)
{
	self->style = &self->manager->styles->fallback;
	self->userrequest.width = -1;
	self->userrequest.height = -1;
	self->manager = manager;
	self->visible = 1;
	return 1;
}

static void
private_free (LIWdgWidget* self)
{
	lisys_free (self->style_name);
	lisys_free (self->state_name);
}

static int
private_event (LIWdgWidget* self,
               LIWdgEvent*  event)
{
	return 1;
}

static void
private_rebuild_style (LIWdgWidget* self)
{
	LIWdgEvent event;
	LIWdgStyle* style;

	/* Find style. */
	style = liwdg_manager_find_style (self->manager, self->style_name, self->state_name);
	if (style == NULL)
		style = &self->manager->styles->fallback;

	/* Set new style and request. */
	if (self->style != style)
	{
		self->style = style;
		event.type = LIWDG_EVENT_TYPE_STYLE;
		self->type->event (self, &event);
		if (self->parent != NULL)
			liwdg_container_child_request (LIWDG_CONTAINER (self->parent), self);
	}
}

/** @} */
/** @} */
