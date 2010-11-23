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
 * \addtogroup LIWdgManager Manager
 * @{
 */

#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-manager.h"

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

static void
private_attach_window (LIWdgManager* self,
                       LIWdgWidget*  widget);

static void
private_detach_window (LIWdgManager* self,
                       LIWdgWidget*  widget);

static LIWdgWidget*
private_find_window (LIWdgManager* self,
                     int           x,
                     int           y,
                     int*          match);

static void
private_focus_window (LIWdgManager* self,
                      LIWdgWidget*  window);

static int
private_cycle_window (LIWdgManager* self,
                      int           next);

static void
private_resize_window (LIWdgManager* self,
                       LIWdgWidget*  window);

static int
private_load_config (LIWdgManager* self,
                     const char*   root);

/*****************************************************************************/

/**
 * \brief Creates a new widget manager.
 * \param render Renderer.
 * \param callbacks Callback manager.
 * \param root Client data directory root.
 * \return New widget manager or NULL.
 */
LIWdgManager* liwdg_manager_new (
	LIRenRender*    render,
	LICalCallbacks* callbacks,
	const char*     root)
{
	LIWdgManager* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIWdgManager));
	if (self == NULL)
		return NULL;
	self->callbacks = callbacks;
	self->width = 640;
	self->height = 480;
	self->render = render;
	self->context = liren_render_get_context (render);
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
liwdg_manager_free (LIWdgManager* self)
{
	lisys_assert (self->dialogs.bottom == NULL);
	lisys_assert (self->dialogs.top == NULL);

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
liwdg_manager_alloc_widgets (LIWdgManager* self,
                                           ...)
{
	int fail;
	va_list args;
	LIWdgWidget* ptr;
	LIWdgWidget** pptr;

	/* Check for errors. */
	fail = 0;
	va_start (args, self);
	while (1)
	{
		pptr = va_arg (args, LIWdgWidget**);
		if (pptr == NULL)
			break;
		ptr = va_arg (args, LIWdgWidget*);
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
			pptr = va_arg (args, LIWdgWidget**);
			if (pptr == NULL)
				break;
			ptr = va_arg (args, LIWdgWidget*);
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
		pptr = va_arg (args, LIWdgWidget**);
		if (pptr == NULL)
			break;
		ptr = va_arg (args, LIWdgWidget*);
		lisys_assert (ptr != NULL);
		*pptr = ptr;
	}
	va_end (args);

	return 1;
}

void
liwdg_manager_cycle_focus (LIWdgManager* self,
                           int           next)
{
	LIWdgWidget* tmp;
	LIWdgWidget* widget;
	LIWdgWidget* focus;

	/* Ensure toplevel focus. */
	focus = liwdg_manager_get_focus (self);
	if (focus == NULL)
	{
		liwdg_manager_cycle_window_focus (self, next);
		return;
	}

	/* Focus next or previous widget. */
	for (widget = focus ; widget->parent != NULL ; widget = widget->parent)
	{
		tmp = liwdg_widget_cycle_focus (widget->parent, widget, next);
		if (tmp != NULL)
		{
			liwdg_manager_set_focus (self, tmp);
			return;
		}
	}

	/* Focus first or last widget. */
	tmp = liwdg_widget_cycle_focus (widget, NULL, next);
	if (tmp != NULL)
		liwdg_manager_set_focus (self, tmp);
}

void
liwdg_manager_cycle_window_focus (LIWdgManager* self,
                                  int           next)
{
	private_cycle_window (self, next);
}

LIFntFont* liwdg_manager_find_font (
	LIWdgManager* self,
	const char*   name)
{
	return lialg_strdic_find (self->styles->fonts, name);
}

LIImgTexture* liwdg_manager_find_image (
	LIWdgManager* self,
	const char*   name)
{
	return liwdg_styles_load_image (self->styles, name);
}

/**
 * \brief Finds a widget by screen position.
 * \param self Widget manager.
 * \param x Screen X coordinate.
 * \param y Screen Y coordinate.
 * \return Widget or NULL.
 */
LIWdgWidget* liwdg_manager_find_widget_by_point (
	LIWdgManager* self,
	int           x,
	int           y)
{
	int match;
	LIWdgWidget* widget;
	LIWdgWidget* child;

	/* Find window. */
	widget = private_find_window (self, x, y, &match);
	if (widget == NULL)
		return NULL;

	/* Find widget. */
	while (1)
	{
		child = liwdg_widget_child_at (widget, x, y);
		if (child == NULL)
			break;
		widget = child;
	}

	return widget;
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
LIWdgWidget*
liwdg_manager_find_window_by_point (LIWdgManager* self,
                                    int           x,
                                    int           y)
{
	int match;

	return private_find_window (self, x, y, &match);
}

/**
 * \brief Handles an event.
 *
 * \param self Widget manager.
 * \param event Event.
 * \return Nonzero if handled, zero if passed through.
 */
int
liwdg_manager_event (LIWdgManager* self,
                     LIWdgEvent*   event)
{
	int x;
	int y;
	int match;
	LIWdgRect rect;
	LIWdgWidget* dialog;
	LIWdgWidget* widget;

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
				widget = private_find_window (self, x, y, &match);
				break;
			case LIWDG_EVENT_TYPE_MOTION:
				x = event->motion.x;
				y = event->motion.y;
				widget = private_find_window (self, x - event->motion.dx, y - event->motion.dy, &match);
				break;
			default:
				return 1;
		}
		if (widget != NULL)
			liwdg_widget_move (widget, x - self->drag.startx, y - self->drag.starty);
		else
			self->drag.active = 0;
		return 1;
	}

	/* Hide temporary widgets that were missed. */
	if (event->type == LIWDG_EVENT_TYPE_BUTTON_PRESS)
	{
		x = event->button.x;
		y = event->button.y;
		for (dialog = self->dialogs.top ; dialog != NULL ; dialog = dialog->next)
		{
			if (dialog->temporary)
			{
				liwdg_widget_get_allocation (dialog, &rect);
				if (rect.x > x || x >= rect.x + rect.width ||
				    rect.y > y || y >= rect.y + rect.height)
					liwdg_widget_set_floating (dialog, 0);
			}
		}
	}

	/* Get target widget. */
	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_KEY_PRESS:
		case LIWDG_EVENT_TYPE_KEY_RELEASE:
			widget = liwdg_manager_get_focus (self);
			match = LIWDG_MATCH_INSIDE;
			break;
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			widget = private_find_window (self, event->button.x, event->button.y, &match);
			break;
		case LIWDG_EVENT_TYPE_MOTION:
			widget = private_find_window (self, event->motion.x, event->motion.y, &match);
			break;
		default:
			widget = NULL;
			match = LIWDG_MATCH_INSIDE;
			lisys_assert (0);
			break;
	}
	if (widget == NULL)
		return 0;

	/* Window events. */
	if (!liwdg_widget_event (widget, event))
		return 1;

	/* Window dragging. */
	if (event->type == LIWDG_EVENT_TYPE_BUTTON_PRESS && !widget->fullscreen)
	{
		self->drag.active = 1;
		self->drag.startx = event->button.x - widget->allocation.x;
		self->drag.starty = event->button.y - widget->allocation.y;
		private_focus_window (self, widget);
		return 1;
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
liwdg_manager_event_sdl (LIWdgManager* self,
                         SDL_Event*    event)
{
	LIWdgEvent evt;

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
			evt.motion.dy = event->motion.yrel;
			evt.motion.buttons = event->motion.state;
			break;
		default:
			return 0;
	}

	return liwdg_manager_event (self, &evt);
}

int
liwdg_manager_insert_window (LIWdgManager* self,
                             LIWdgWidget*  widget)
{
	LIWdgSize size;

	liwdg_widget_detach (widget);
	widget->floating = 1;
	private_attach_window (self, widget);

	liwdg_widget_get_request (widget, &size);
	size.width = (self->width - size.width) / 2;
	size.height = (self->height - size.height) / 2;
	liwdg_widget_move (widget, size.width, size.height);
	private_resize_window (self, widget);

	return 1;
}

int
liwdg_manager_remove_window (LIWdgManager* self,
                             LIWdgWidget*  widget)
{
	lisys_assert (widget->floating);
	lisys_assert (widget->prev != NULL || widget == self->dialogs.top);
	lisys_assert (widget->next != NULL || widget == self->dialogs.bottom);

	/* Make sure that the update loop doesn't break. */
	if (self->widgets.iter == widget)
		self->widgets.iter = widget->next;

	/* Remove from stack. */
	private_detach_window (self, widget);
	widget->floating = 0;

	return 1;
}

void
liwdg_manager_render (LIWdgManager* self)
{
	LIMatMatrix matrix;
	LIWdgWidget* widget;

	/* Find the widget shader. */
	self->shader = liren_render_find_shader (self->render, "widget");
	if (self->shader == NULL)
		return;

	/* Initialize render mode. */
	matrix = limat_matrix_ortho (0.0f, self->width, self->height, 0.0f, -100.0f, 100.0f);
	liren_context_init (self->context);
	liren_context_set_blend (self->context, 1, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	liren_context_set_cull (self->context, 0, GL_CCW);
	liren_context_set_depth (self->context, 0, 0, GL_LEQUAL);
	liren_context_set_shader (self->context, LIREN_SHADER_PASS_FORWARD0, self->shader);
	liren_context_bind (self->context);

	/* Setup viewport. */
	glViewport (0, 0, self->width, self->height);

	/* Render widgets. */
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);
	for (widget = self->dialogs.bottom ; widget != NULL ; widget = widget->prev)
	{
		if (liwdg_widget_get_visible (widget))
			liwdg_widget_draw (widget);
	}
}

void
liwdg_manager_update (LIWdgManager* self,
                      float         secs)
{
	int x;
	int y;
	int cx;
	int cy;
	int buttons;
	LIWdgEvent event;
	LIWdgWidget* widget;

	if (self->widgets.grab != NULL)
	{
		cx = self->width / 2;
		cy = self->height / 2;
		buttons = SDL_GetMouseState (&x, &y);
		if (x != cx || y != cy)
		{
			SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
			SDL_WarpMouse (cx, cy);
			SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
		}

		/* Cursor delta events. */
		if (x != cx || y != cy)
		{
			event.type = LIWDG_EVENT_TYPE_MOTION;
			event.motion.x = cx;
			event.motion.y = cy;
			event.motion.dx = x - cx;
			event.motion.dy = y - cy;
			event.motion.buttons = buttons;
			liwdg_widget_event (self->widgets.grab, &event);
		}
	}
	for (widget = self->dialogs.top ; widget != NULL ; widget = self->widgets.iter)
	{
		self->widgets.iter = widget->next;
		private_resize_window (self, widget);
	}
}

LIWdgWidget*
liwdg_manager_get_focus (LIWdgManager* self)
{
	LIWdgWidget* widget;

	widget = liwdg_manager_find_widget_by_point (self, self->pointer.x, self->pointer.y);
	if (widget != NULL && widget->focusable && widget->visible)
		return widget;

	return NULL;
}

void
liwdg_manager_set_focus (LIWdgManager* self,
                         LIWdgWidget*  widget)
{
	if (liwdg_manager_get_focus (self) != widget)
	{
		SDL_WarpMouse (
			widget->allocation.x + widget->allocation.width / 2,
			widget->allocation.y + widget->allocation.height / 2);
	}
}

/**
 * \brief Gets the projection matrix used for rendering widgets.
 *
 * \param self Widget manager.
 * \param matrix Return location for the projection matrix.
 */
void
liwdg_manager_get_projection (LIWdgManager* self,
                              LIMatMatrix*  matrix)
{
	*matrix = self->projection;
}

/**
 * \brief Gets the screen size.
 *
 * \param self Widget manager.
 * \param width Return location for the width or NULL.
 * \param height Return location for the height or NULL.
 */
void
liwdg_manager_get_size (LIWdgManager* self,
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
liwdg_manager_set_size (LIWdgManager* self,
                        int           width,
                        int           height)
{
	/* Update projection matrix. */
	self->width = width;
	self->height = height;
	self->projection = limat_matrix_ortho (0.0f, width, height, 0.0f, -100.0f, 100.0f);
}

/*****************************************************************************/

static void
private_attach_window (LIWdgManager* self,
                       LIWdgWidget*  widget)
{
	widget->prev = NULL;
	widget->next = self->dialogs.top;
	if (self->dialogs.top != NULL)
		self->dialogs.top->prev = widget;
	else
		self->dialogs.bottom = widget;
	self->dialogs.top = widget;
}

static void
private_detach_window (LIWdgManager* self,
                       LIWdgWidget*  widget)
{
	if (widget->next != NULL)
		widget->next->prev = widget->prev;
	else
		self->dialogs.bottom = widget->prev;
	if (widget->prev != NULL)
		widget->prev->next = widget->next;
	else
		self->dialogs.top = widget->next;
}

static LIWdgWidget*
private_find_window (LIWdgManager* self,
                     int           x,
                     int           y,
                     int*          match)
{
	LIWdgRect rect;
	LIWdgWidget* widget;

	for (widget = self->dialogs.top ; widget != NULL ; widget = widget->next)
	{
		lisys_assert (liwdg_widget_get_visible (widget));
		liwdg_widget_get_allocation (widget, &rect);

		if (rect.x <= x && x < rect.x + rect.width &&
			rect.y <= y && y < rect.y + rect.height)
		{
			*match = LIWDG_MATCH_INSIDE;
			return widget;
		}
	}

	return NULL;
}

static void
private_focus_window (LIWdgManager* self,
                      LIWdgWidget*  window)
{
	if (!window->behind)
	{
		private_detach_window (self, window);
		private_attach_window (self, window);
	}
}

static int
private_cycle_window (LIWdgManager* self,
                      int           next)
{
	LIWdgWidget* tmp;
	LIWdgWidget* start;
	LIWdgWidget* widget;

	/* Find focused window. */
	widget = liwdg_manager_get_focus (self);
	if (widget != NULL)
	{
		while (widget->parent != NULL)
			widget = widget->parent;
	}

	/* Find first window to try. */
	if (widget != NULL)
	{
		if (next)
			start = widget->next;
		else
			start = widget->prev;
		if (start == NULL)
			return 1;
	}
	else
		start = NULL;
	if (start == NULL)
	{
		if (next)
			start = self->dialogs.top;
		else
			start = self->dialogs.bottom;
		if (start == NULL)
			return 0;
	}

	/* Search for focusable window. */
	for (widget = start ;; )
	{
		/* Check if this one is it. */
		if (liwdg_widget_get_visible (widget))
		{
			if (liwdg_widget_get_focusable (widget))
			{
				private_focus_window (self, widget);
				liwdg_manager_set_focus (self, widget);
				return 1;
			}
			else
			{
				tmp = liwdg_widget_cycle_focus (widget, NULL, next);
				if (tmp != NULL)
				{
					private_focus_window (self, widget);
					liwdg_manager_set_focus (self, tmp);
					return 1;
				}
			}
		}

		/* Else try next window. */
		if (next)
			widget = widget->next;
		else
			widget = widget->prev;
		if (widget == NULL)
		{
			if (next)
				widget = self->dialogs.top;
			else
				widget = self->dialogs.bottom;
		}

		/* Give up if search wrapped. */
		if (widget == start)
			return 0;
	}
}

static void
private_resize_window (LIWdgManager* self,
                       LIWdgWidget*  window)
{
	LIWdgRect rect;
	LIWdgSize size;

	if (window->fullscreen)
	{
		size.width = self->width;
		size.height = self->height;
		liwdg_widget_get_allocation (window, &rect);
		if (rect.x != 0 || rect.y != 0 || rect.width != size.width || rect.height != size.height)
			liwdg_widget_set_allocation (window, 0, 0, size.width, size.height);
	}
	else
	{
		liwdg_widget_get_request (window, &size);
		liwdg_widget_get_allocation (window, &rect);
		if (rect.width != size.width || rect.height != size.height)
			liwdg_widget_set_allocation (window, rect.x, rect.y, size.width, size.height);
	}
}

static int
private_load_config (LIWdgManager* self,
                     const char*   root)
{
	self->styles = liwdg_styles_new (self, root);
	if (self->styles == NULL)
		return 0;

	return 1;
}

/** @} */
/** @} */
