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
 * \addtogroup liwdgManager Manager
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "widget-group.h"
#include "widget-manager.h"

#define BORDERW 3 // FIXME
#define BORDERH 3 // FIXME
#define TITLEBARH 16 // FIXME

enum
{
	LIWDG_MATCH_BOTTOM,
	LIWDG_MATCH_BOTTOMLEFT,
	LIWDG_MATCH_BOTTOMRIGHT,
	LIWDG_MATCH_INSIDE,
	LIWDG_MATCH_LEFT,
	LIWDG_MATCH_RIGHT,
	LIWDG_MATCH_TITLEBAR,
	LIWDG_MATCH_TOP,
	LIWDG_MATCH_TOPLEFT,
	LIWDG_MATCH_TOPRIGHT,
};

liwdgWidget*
private_find_window (liwdgManager* self,
                     int           x,
                     int           y,
                     int*          match);

static int
private_focus_root (liwdgManager* self);

static int
private_focus_window (liwdgManager* self,
                      int           next);

static int
private_load_config (liwdgManager* self,
                     const char*   root);

/*****************************************************************************/

/**
 * \brief Creates a new widget manager.
 *
 * \param video Video callbacks.
 * \param root Client data directory root.
 * \return New widget manager or NULL.
 */
liwdgManager*
liwdg_manager_new (lividCalls* video,
                   const char* root)
{
	liwdgManager* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liwdgManager));
	if (self == NULL)
		return NULL;
	self->width = 640;
	self->height = 480;
	self->video = *video;
	self->projection = limat_matrix_identity ();

	/* Load config and resources. */
	if (!private_load_config (self, root))
	{
		liwdg_manager_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Freed the widget manager.
 *
 * \param self Widget manager.
 */
void
liwdg_manager_free (liwdgManager* self)
{
	assert (self->widgets.dialogs == NULL);
	assert (self->widgets.root == NULL);
	assert (self->widgets.active == NULL);

	if (self->styles != NULL)
		liwdg_styles_free (self->styles);
	lisys_free (self);
}

/**
 * \brief Stores allocated widgets to pointers.
 *
 * \param self Widget manager.
 * \param ... List of pointer-to-pointer and pointer pairs terminated by NULL.
 */
int
liwdg_manager_alloc_widgets (liwdgManager* self,
                                           ...)
{
	int fail;
	va_list args;
	liwdgWidget* ptr;
	liwdgWidget** pptr;

	/* Check for errors. */
	fail = 0;
	va_start (args, self);
	while (1)
	{
		pptr = va_arg (args, liwdgWidget**);
		if (pptr == NULL)
			break;
		ptr = va_arg (args, liwdgWidget*);
		if (ptr == NULL)
		{
			fail = 1;
			break;
		}
	}
	va_end (args);
	if (fail)
	{
		va_start (args, self);
		while (1)
		{
			pptr = va_arg (args, liwdgWidget**);
			if (pptr == NULL)
				break;
			ptr = va_arg (args, liwdgWidget*);
			if (ptr != NULL)
				liwdg_widget_free (ptr);
		}
		va_end (args);
		return 0;
	}

	/* Copy widgets to target variables. */
	va_start (args, self);
	while (1)
	{
		pptr = va_arg (args, liwdgWidget**);
		if (pptr == NULL)
			break;
		ptr = va_arg (args, liwdgWidget*);
		assert (ptr != NULL);
		*pptr = ptr;
	}
	va_end (args);

	return 1;
}

void
liwdg_manager_cycle_focus (liwdgManager* self,
                           int           next)
{
	liwdgWidget* tmp;
	liwdgWidget* widget;

	/* Ensure toplevel focus. */
	if (self->focus.keyboard == NULL)
	{
		liwdg_manager_cycle_window_focus (self, next);
		return;
	}

	/* Focus next or previous widget. */
	for (widget = self->focus.keyboard ; widget->parent != NULL ; widget = widget->parent)
	{
		tmp = liwdg_container_cycle_focus (LIWDG_CONTAINER (widget->parent), widget, next);
		if (tmp != NULL)
		{
			liwdg_manager_set_focus_keyboard (self, tmp);
			return;
		}
	}

	/* Focus first or last widget. */
	tmp = liwdg_container_cycle_focus (LIWDG_CONTAINER (widget), NULL, next);
	if (tmp != NULL)
		liwdg_manager_set_focus_keyboard (self, tmp);
}

void
liwdg_manager_cycle_window_focus (liwdgManager* self,
                                  int           next)
{
	if (!private_focus_window (self, next))
	{
		self->focus.keyboard = NULL;
		private_focus_window (self, next);
	}
}

lifntFont*
liwdg_manager_find_font (liwdgManager* self,
                         const char*   name)
{
	return lialg_strdic_find (self->styles->fonts, name);
}

liwdgStyle*
liwdg_manager_find_style (liwdgManager* self,
                          const char*   name)
{
	return lialg_strdic_find (self->styles->subimgs, name);
}

/**
 * \brief Finds a window by screen position.
 *
 * The root widget is considered a dialog widget as well and
 * is returned if no other widget matched.
 *
 * \param self Widget manager.
 * \param x Screen X coordinate.
 * \param y Screen Y coordinate.
 * \return Widget or NULL.
 */
liwdgWidget*
liwdg_manager_find_window_by_point (liwdgManager* self,
                                    int           x,
                                    int           y)
{
	int match;

	return private_find_window (self, x, y, &match);
}

/**
 * \brief Makes sure that the focused widgets are visible.
 *
 * \param self Widget manager.
 */
void
liwdg_manager_fix_focus (liwdgManager* self)
{
	liwdgWidget* widget;

	for (widget = self->widgets.grab ; widget != NULL ; widget = widget->parent)
	{
		if (!widget->visible)
		{
			if (liwdg_widget_get_grab (self->widgets.grab))
				liwdg_widget_set_grab (self->widgets.grab, 0);
			break;
		}
	}
	for (widget = self->focus.keyboard ; widget != NULL ; widget = widget->parent)
	{
		if (!widget->visible)
		{
			liwdg_manager_set_focus_keyboard (self, NULL);
			break;
		}
		if (widget->parent == NULL && widget->state == LIWDG_WIDGET_STATE_DETACHED)
			liwdg_manager_set_focus_keyboard (self, NULL);
	}
	for (widget = self->focus.mouse ; widget != NULL ; widget = widget->parent)
	{
		if (!widget->visible)
		{
			liwdg_manager_set_focus_mouse (self, NULL);
			break;
		}
		if (widget->parent == NULL && widget->state == LIWDG_WIDGET_STATE_DETACHED)
			liwdg_manager_set_focus_mouse (self, NULL);
	}
}

/**
 * \brief Handles an event.
 *
 * \param self Widget manager.
 * \param event Event.
 * \return Nonzero if handled, zero if passed through.
 */
int
liwdg_manager_event (liwdgManager* self,
                     liwdgEvent*   event)
{
	int x;
	int y;
	int match;
	liwdgEvent popup;
	liwdgRect rect;
	liwdgWidget* widget;

	/* Maintain pointer position. */
	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			self->pointer.x = event->button.x;
			self->pointer.y = event->button.y;
			break;
		case LIWDG_EVENT_TYPE_MOTION:
			self->pointer.x = event->motion.x;
			self->pointer.y = event->motion.y;
			break;
	}

	/* Handle grabs. */
	if (self->widgets.grab != NULL)
	{
		widget = self->widgets.grab;
		if (event->type == LIWDG_EVENT_TYPE_KEY_PRESS && event->key.keycode == SDLK_ESCAPE)
		{
			liwdg_widget_set_grab (self->widgets.grab, 0);
			return 1;
		}
		if (event->type == LIWDG_EVENT_TYPE_MOTION)
			return 1;
		liwdg_widget_event (widget, event);
		return 1;
	}

	/* Handle popups. */
	if (self->widgets.popups != NULL)
	{
		for (widget = self->widgets.popups ; widget->next != NULL ; widget = widget->next) {}
		if (event->type == LIWDG_EVENT_TYPE_BUTTON_PRESS)
		{
			liwdg_widget_get_allocation (widget, &rect);
			if (event->button.x < rect.x || rect.x + rect.width <= event->button.x ||
				event->button.y < rect.y || rect.y + rect.height <= event->button.y)
			{
				popup.type = LIWDG_EVENT_TYPE_CLOSE;
				liwdg_widget_event (widget, &popup);
				return 1;
			}
		}
		liwdg_widget_event (widget, event);
		return 1;
	}

	/* Handle active drags. */
	if (self->drag.active)
	{
		switch (event->type)
		{
			case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
				if (event->button.button == 1)
					self->drag.active = 0;
				x = event->button.x;
				y = event->button.y;
				break;
			case LIWDG_EVENT_TYPE_MOTION:
				x = event->motion.x;
				y = event->motion.y;
				break;
			default:
				return 1;
		}
		widget = self->focus.mouse;
		if (widget != NULL)
			liwdg_widget_move (widget, x - self->drag.startx, y - self->drag.starty);
		else
			self->drag.active = 0;
		return 1;
	}

	/* Get target widget. */
	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_KEY_PRESS:
		case LIWDG_EVENT_TYPE_KEY_RELEASE:
			widget = self->focus.keyboard;
			match = LIWDG_MATCH_INSIDE;
			break;
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			widget = private_find_window (self, event->button.x, event->button.y, &match);
			liwdg_manager_set_focus_mouse (self, widget);
			if (widget != NULL && liwdg_widget_get_focusable (widget))
				liwdg_manager_set_focus_keyboard (self, widget);
			else
				liwdg_manager_set_focus_keyboard (self, NULL);
			break;
		case LIWDG_EVENT_TYPE_MOTION:
			widget = private_find_window (self, event->motion.x, event->motion.y, &match);
			liwdg_manager_set_focus_mouse (self, widget);
			break;
		default:
			widget = NULL;
			match = LIWDG_MATCH_INSIDE;
			assert (0);
			break;
	}
	if (widget == NULL)
		return 0;

	/* Window events. */
	if (match == LIWDG_MATCH_INSIDE)
	{
		if (!liwdg_widget_event (widget, event))
			return 1;
	}

	/* Manager events. */
	if (event->type == LIWDG_EVENT_TYPE_BUTTON_PRESS)
	{
		/* TODO: Shading windows. */

		/* Moving windows. */
		if (match == LIWDG_MATCH_TITLEBAR && event->button.button == 1)
		{
			self->drag.active = 1;
			self->drag.startx = event->button.x - widget->allocation.x;
			self->drag.starty = event->button.y - widget->allocation.y;
			return 1;
		}

		/* Click to focus. */
		if (liwdg_widget_get_focusable (widget))
		{
			liwdg_manager_set_focus_keyboard (self, widget);
			return 1;
		}
	}

	return 0;
}

/**
 * \brief Handles an SDL event.
 *
 * \param self Widget manager.
 * \param event Event.
 * \return Nonzero if handled, zero if passed through.
 */
int
liwdg_manager_event_sdl (liwdgManager* self,
                         SDL_Event*    event)
{
	liwdgEvent evt;

	switch (event->type)
	{
		case SDL_KEYDOWN:
			evt.type = LIWDG_EVENT_TYPE_KEY_PRESS;
			evt.key.keycode = event->key.keysym.sym;
			evt.key.unicode = event->key.keysym.unicode;
			evt.key.modifiers = event->key.keysym.mod;
			break;
		case SDL_KEYUP:
			evt.type = LIWDG_EVENT_TYPE_KEY_RELEASE;
			evt.key.keycode = event->key.keysym.sym;
			evt.key.unicode = event->key.keysym.unicode;
			evt.key.modifiers = event->key.keysym.mod;
			break;
		case SDL_MOUSEBUTTONDOWN:
			evt.type = LIWDG_EVENT_TYPE_BUTTON_PRESS;
			evt.button.x = event->button.x;
			evt.button.y = event->button.y;
			evt.button.button = event->button.button;
			break;
		case SDL_MOUSEBUTTONUP:
			evt.type = LIWDG_EVENT_TYPE_BUTTON_RELEASE;
			evt.button.x = event->button.x;
			evt.button.y = event->button.y;
			evt.button.button = event->button.button;
			break;
		case SDL_MOUSEMOTION:
			evt.type = LIWDG_EVENT_TYPE_MOTION;
			evt.motion.x = event->motion.x;
			evt.motion.y = event->motion.y;
			evt.motion.dx = event->motion.xrel;
			evt.motion.dy = -event->motion.yrel;
			evt.motion.buttons = event->motion.state;
			break;
		default:
			return 0;
	}

	return liwdg_manager_event (self, &evt);
}

int
liwdg_manager_insert_popup (liwdgManager* self,
                            liwdgWidget*  widget)
{
	assert (widget->state == LIWDG_WIDGET_STATE_DETACHED);

	widget->prev = NULL;
	widget->next = self->widgets.popups;
	if (self->widgets.popups != NULL)
		self->widgets.popups->prev = widget;
	self->widgets.popups = widget;
	widget->state = LIWDG_WIDGET_STATE_POPUP;

	return 1;
}

int
liwdg_manager_insert_window (liwdgManager* self,
                             liwdgWidget*  widget)
{
	liwdgSize size;

	assert (widget->state == LIWDG_WIDGET_STATE_DETACHED);

	widget->prev = NULL;
	widget->next = self->widgets.dialogs;
	if (self->widgets.dialogs != NULL)
		self->widgets.dialogs->prev = widget;
	else
		self->widgets.active = widget;
	self->widgets.dialogs = widget;
	widget->state = LIWDG_WIDGET_STATE_WINDOW;

	static int x=32;// FIXME
	static int y=32;
	liwdg_widget_get_request (widget, &size);
	size.width = LI_MAX (1, self->width - size.width);
	size.height = LI_MAX (1, self->height - size.height);
	liwdg_widget_move (widget, x % size.width, y % size.height);
	x += 64;
	y += 32;

	return 1;
}

int
liwdg_manager_remove_popup (liwdgManager* self,
                            liwdgWidget*  widget)
{
	assert (widget->prev != NULL || widget == self->widgets.popups);
	assert (widget->state == LIWDG_WIDGET_STATE_POPUP);

	/* Clear invalid focus. */
	liwdg_manager_fix_focus (self);

	/* Make sure that the update loop doesn't break. */
	if (self->widgets.iter == widget)
		self->widgets.iter = widget->next;

	/* Remove from stack. */
	if (widget->next != NULL)
		widget->next->prev = widget->prev;
	if (widget->prev != NULL)
		widget->prev->next = widget->next;
	else
		self->widgets.popups = widget->next;
	widget->state = LIWDG_WIDGET_STATE_DETACHED;

	return 1;
}

int
liwdg_manager_remove_window (liwdgManager* self,
                             liwdgWidget*  widget)
{
	assert (widget->prev != NULL || widget == self->widgets.dialogs);
	assert (widget->next != NULL || widget == self->widgets.active);
	assert (widget->state == LIWDG_WIDGET_STATE_WINDOW);

	/* Clear invalid focus. */
	liwdg_manager_fix_focus (self);

	/* Make sure that the update loop doesn't break. */
	if (self->widgets.iter == widget)
		self->widgets.iter = widget->next;

	/* Remove from stack. */
	if (widget->next != NULL)
		widget->next->prev = widget->prev;
	else
		self->widgets.active = widget->prev;
	if (widget->prev != NULL)
		widget->prev->next = widget->next;
	else
		self->widgets.dialogs = widget->next;
	widget->state = LIWDG_WIDGET_STATE_DETACHED;

	return 1;
}

void
liwdg_manager_render (liwdgManager* self)
{
	liwdgWidget* widget;

	/* Setup viewport. */
	glViewport (0, 0, self->width, self->height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, self->width, self->height, 0, -100.0f, 100.0f);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	/* Setup render mode. */
	glEnable (GL_BLEND);
	glEnable (GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glDisable (GL_LIGHTING);
	glDepthMask (GL_FALSE);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Render widgets. */
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);
	if (self->widgets.root != NULL)
	{
		if (liwdg_widget_get_visible (self->widgets.root))
			liwdg_widget_render (self->widgets.root);
	}
	for (widget = self->widgets.dialogs ; widget != NULL ; widget = widget->next)
	{
		if (liwdg_widget_get_visible (widget))
			liwdg_widget_render (widget);
	}
	for (widget = self->widgets.popups ; widget != NULL ; widget = widget->next)
	{
		if (liwdg_widget_get_visible (widget))
			liwdg_widget_render (widget);
	}
}

void
liwdg_manager_update (liwdgManager* self,
                      float         secs)
{
	int x;
	int y;
	int cx;
	int cy;
	int buttons;
	liwdgEvent event;
	liwdgRect rect;
	liwdgSize size;
	liwdgWidget* widget;

	if (self->widgets.grab != NULL)
	{
		cx = self->width / 2;
		cy = self->height / 2;
		buttons = self->video.SDL_GetMouseState (&x, &y);
		self->video.SDL_WarpMouse (cx, cy);

		/* Cursor delta events. */
		if (x != cx || y != cy)
		{
			event.type = LIWDG_EVENT_TYPE_MOTION;
			event.motion.x = cx;
			event.motion.y = cy;
			event.motion.dx = x - cx;
			event.motion.dy = (self->height - y - 1) - cy;
			event.motion.buttons = buttons;
			liwdg_widget_event (self->widgets.grab, &event);
		}
	}
	if (self->widgets.root != NULL)
		liwdg_widget_update (self->widgets.root, secs);
	for (widget = self->widgets.dialogs ; widget != NULL ; widget = self->widgets.iter)
	{
		self->widgets.iter = widget->next;
		liwdg_widget_get_allocation (widget, &rect);
		liwdg_widget_get_request (widget, &size);
		if (rect.width != size.width || rect.height != size.height)
			liwdg_widget_set_allocation (widget, rect.x, rect.y, size.width, size.height);
		liwdg_widget_update (widget, secs);
	}
	for (widget = self->widgets.popups ; widget != NULL ; widget = self->widgets.iter)
	{
		self->widgets.iter = widget->next;
		liwdg_widget_get_allocation (widget, &rect);
		liwdg_widget_get_request (widget, &size);
		if (rect.width != size.width || rect.height != size.height)
			liwdg_widget_set_allocation (widget, rect.x, rect.y, size.width, size.height);
		liwdg_widget_update (widget, secs);
	}
}

liwdgWidget*
liwdg_manager_get_focus_keyboard (liwdgManager* self)
{
	return self->focus.keyboard;
}

void
liwdg_manager_set_focus_keyboard (liwdgManager* self,
                                  liwdgWidget*  widget)
{
	liwdgEvent event;
	liwdgWidget* focus;

	focus = self->focus.keyboard;
	if (focus == widget)
		return;
	self->focus.keyboard = widget;
	if (focus != NULL)
	{
		event.type = LIWDG_EVENT_TYPE_FOCUS_LOSE;
		event.focus.mouse = 0;
		liwdg_widget_event (focus, &event);
	}
	if (self->focus.keyboard != NULL)
	{
		event.type = LIWDG_EVENT_TYPE_FOCUS_GAIN;
		event.focus.mouse = 0;
		liwdg_widget_event (self->focus.keyboard, &event);
	}
}

liwdgWidget*
liwdg_manager_get_focus_mouse (liwdgManager* self)
{
	return self->focus.mouse;
}

void
liwdg_manager_set_focus_mouse (liwdgManager* self,
                               liwdgWidget*  widget)
{
	liwdgEvent event;
	liwdgWidget* focus;

	focus = self->focus.mouse;
	if (focus == widget)
		return;
	self->focus.mouse = widget;
	if (focus != NULL)
	{
		event.type = LIWDG_EVENT_TYPE_FOCUS_LOSE;
		event.focus.mouse = 1;
		liwdg_widget_event (focus, &event);
	}
	if (self->focus.mouse != NULL)
	{
		event.type = LIWDG_EVENT_TYPE_FOCUS_GAIN;
		event.focus.mouse = 1;
		liwdg_widget_event (self->focus.mouse, &event);
	}
}

/**
 * \brief Gets the projection matrix used for rendering widgets.
 *
 * \param self Widget manager.
 * \param matrix Return location for the projection matrix.
 */
void
liwdg_manager_get_projection (liwdgManager* self,
                              limatMatrix*  matrix)
{
	*matrix = self->projection;
}

/**
 * \brief Gets the root widget.
 *
 * \param self Widget manager.
 * \return Widget owned by the widget manager or NULL.
 */
liwdgWidget*
liwdg_manager_get_root (liwdgManager* self)
{
	return self->widgets.root;
}

/**
 * \brief Sets the root widget.
 *
 * \param self Widget manager.
 * \param widget Widget or NULL.
 */
void
liwdg_manager_set_root (liwdgManager* self,
                        liwdgWidget*  widget)
{
	if (self->widgets.root == widget)
		return;

	/* Clear invalid focus. */
	liwdg_manager_fix_focus (self);

	/* Replace old root. */
	if (self->widgets.root != NULL)
	{
		assert (self->widgets.root->state == LIWDG_WIDGET_STATE_ROOT);
		self->widgets.root->transparent = 0;
		self->widgets.root->state = LIWDG_WIDGET_STATE_DETACHED;
	}
	self->widgets.root = widget;
	if (self->widgets.root != NULL)
	{
		assert (self->widgets.root->state == LIWDG_WIDGET_STATE_DETACHED);
		self->widgets.root->transparent = 1;
		self->widgets.root->state = LIWDG_WIDGET_STATE_ROOT;
	}

	/* Set the allocation. */
	if (self->widgets.root != NULL)
		liwdg_widget_set_allocation (self->widgets.root, 0, 0, self->width, self->height);
}

/**
 * \brief Gets the screen size.
 *
 * \param self Widget manager.
 * \param width Return location for the width or NULL.
 * \param height Return location for the height or NULL.
 */
void
liwdg_manager_get_size (liwdgManager* self,
                        int*          width,
                        int*          height)
{
	if (width != NULL)
		*width = self->width;
	if (height != NULL)
		*height = self->height;
}

/**
 * \brief Sets the root window size.
 *
 * \param self Widget manager.
 * \param width Width in pixels.
 * \param height Height in pixels.
 */
void
liwdg_manager_set_size (liwdgManager* self,
                        int           width,
                        int           height)
{
	self->width = width;
	self->height = height;
	self->projection = limat_matrix_ortho (0.0f, width, height, 0.0f, -100.0f, 100.0f);
	if (self->widgets.root != NULL)
		liwdg_widget_set_allocation (self->widgets.root, 0, 0, width, height);
}

/*****************************************************************************/

liwdgWidget*
private_find_window (liwdgManager* self,
                     int           x,
                     int           y,
                     int*          match)
{
	liwdgRect rect;
	liwdgWidget* widget;

	if (self->widgets.dialogs != NULL)
	{
		for (widget = self->widgets.active ; widget != NULL ; widget = widget->prev)
		{
			if (!liwdg_widget_get_visible (widget))
				continue;

			/* Check if inside content frame. */
			liwdg_widget_get_content (widget, &rect);
			if (rect.x <= x && x < rect.x + rect.width &&
				rect.y <= y && y < rect.y + rect.height)
			{
				*match = LIWDG_MATCH_INSIDE;
				return widget;
			}

			/* Check if inside at all. */
			liwdg_widget_get_allocation (widget, &rect);
			if (x < rect.x || rect.x + rect.width <= x ||
				y < rect.y || rect.y + rect.height <= y)
				continue;

			/* Check if inside titlebar. */
			if (rect.x + BORDERW <= x && x < rect.x + rect.width - BORDERW &&
				rect.y + BORDERH <= y && y < rect.y + BORDERH + TITLEBARH)
			{
				*match = LIWDG_MATCH_TITLEBAR;
				return widget;
			}

			/* Check if on corners. */
			if (x < rect.x + BORDERW && y < rect.y + BORDERH)
			{
				*match = LIWDG_MATCH_TOPLEFT;
				return widget;
			}
			if (x >= rect.x + rect.width - BORDERW && y < rect.y + BORDERH)
			{
				*match = LIWDG_MATCH_TOPRIGHT;
				return widget;
			}
			if (x < rect.x + BORDERW && y >= rect.y + rect.height - BORDERH)
			{
				*match = LIWDG_MATCH_BOTTOMLEFT;
				return widget;
			}
			if (x >= rect.x + rect.width - BORDERW && y >= rect.y + rect.height - BORDERH)
			{
				*match = LIWDG_MATCH_BOTTOMRIGHT;
				return widget;
			}

			/* Check if on borders. */
			if (y < rect.y + BORDERH)
			{
				*match = LIWDG_MATCH_TOP;
				return widget;
			}
			if (y >= rect.y + rect.height - BORDERH)
			{
				*match = LIWDG_MATCH_BOTTOM;
				return widget;
			}
			if (x < rect.x + BORDERW)
			{
				*match = LIWDG_MATCH_LEFT;
				return widget;
			}
			if (x >= rect.x + rect.width - BORDERW)
			{
				*match = LIWDG_MATCH_RIGHT;
				return widget;
			}
		}
	}
	if (self->widgets.root != NULL)
	{
		if (liwdg_widget_get_visible (self->widgets.root))
		{
			*match = LIWDG_MATCH_INSIDE;
			return self->widgets.root;
		}
	}

	return NULL;
}

static int
private_focus_root (liwdgManager* self)
{
	liwdgWidget* tmp;
	liwdgWidget* widget;

	widget = self->widgets.root;
	if (widget == NULL)
		return 0;
	if (!liwdg_widget_get_visible (widget))
		return 0;
	if (liwdg_widget_typeis (widget, &liwdgContainerType))
	{
		tmp = liwdg_container_cycle_focus (LIWDG_CONTAINER (widget), NULL, 1);
		if (tmp != NULL)
		{
			liwdg_manager_set_focus_keyboard (self, tmp);
			return 1;
		}
	}
	else if (liwdg_widget_get_focusable (widget))
	{
		liwdg_manager_set_focus_keyboard (self, widget);
		return 1;
	}

	return 0;
}

static int
private_focus_window (liwdgManager* self,
                      int           next)
{
	liwdgWidget* tmp;
	liwdgWidget* widget;

	/* Find current toplevel. */
	if (self->focus.keyboard != NULL)
	{
		widget = self->focus.keyboard;
		while (widget->parent != NULL)
			widget = widget->parent;
	}
	else
	{
		if (private_focus_root (self))
			return 1;
		if (self->widgets.dialogs == NULL)
			return 0;
		widget = NULL;
	}

	/* Find next toplevel. */
	if (widget == NULL || widget == self->widgets.root)
	{
		widget = self->widgets.dialogs;
		if (!next)
		{
			while (widget->next != NULL)
				widget = widget->next;
		}
	}
	else
	{
		if (next)
			widget = widget->next;
		else
			widget = widget->prev;
	}

	/* Find next or previous window. */
	while (widget != NULL)
	{
		if (liwdg_widget_get_visible (widget))
		{
			if (liwdg_widget_typeis (widget, &liwdgContainerType))
			{
				tmp = liwdg_container_cycle_focus (LIWDG_CONTAINER (widget), NULL, next);
				if (tmp != NULL)
				{
					liwdg_manager_set_focus_keyboard (self, tmp);
					return 1;
				}
			}
			else if (liwdg_widget_get_focusable (widget))
			{
				liwdg_manager_set_focus_keyboard (self, widget);
				return 1;
			}
		}
		if (next)
			widget = widget->next;
		else
			widget = widget->prev;
	}

	/* Default to root window. */
	return private_focus_root (self);
}

static int
private_load_config (liwdgManager* self,
                     const char*   root)
{
	self->styles = liwdg_styles_new (self, root);
	if (self->styles == NULL)
		return 0;

	return 1;
}

/** @} */
/** @} */
