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

#include <stdio.h>
#include <stdlib.h>
#include <string/lips-string.h>
#include <system/lips-system.h>
#include "widget-group.h"
#include "widget-manager.h"

#define BORDERW 10 // FIXME
#define BORDERH 10 // FIXME
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
private_load_config (liwdgManager* self,
                     const char*   root);

static lifntFont*
private_load_font (liwdgManager* self,
                   const char*   root,
                   const char*   name,
                   int           size);

static int
private_load_texture (liwdgManager*  self,
                      liimgTexture** texture,
                      const char*    root,
                      const char*    name);

static int
private_focus_next (liwdgManager* self,
                    liwdgWidget*  start);

static int
private_focus_prev (liwdgManager* self,
                    liwdgWidget*  start);

static liwdgWidget*
private_get_next_dialog (liwdgManager* self,
                         liwdgWidget*  curr);

static liwdgWidget*
private_get_prev_dialog (liwdgManager* self,
                         liwdgWidget*  curr);

static liwdgWidget*
private_get_next_focusable (liwdgManager* self,
                            liwdgGroup*   group,
                            liwdgWidget*  curr);

static liwdgWidget*
private_get_prev_focusable (liwdgManager* self,
                            liwdgGroup*   group,
                            liwdgWidget*  curr);

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
	self = calloc (1, sizeof (liwdgManager));
	if (self == NULL)
		return NULL;
	self->width = 640;
	self->height = 480;
	self->video = *video;
	self->projection = limat_matrix_identity ();

	/* Load config and resources. */
	self->fonts = lialg_strdic_new ();
	self->images = lialg_strdic_new ();
	self->subimgs = lialg_strdic_new ();
	if (self->fonts == NULL ||
	    self->images == NULL ||
	    self->subimgs == NULL)
	{
		liwdg_manager_free (self);
		return NULL;
	}
	if (!private_load_config (self, root))
		printf ("WARNING: %s\n", lisys_error_get_string ());

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
	lialgStrdicIter iter;

	assert (self->widgets.dialogs == NULL);
	assert (self->widgets.root == NULL);
	assert (self->widgets.active == NULL);

	/* Free resources. */
	if (self->fonts != NULL)
	{
		LI_FOREACH_STRDIC (iter, self->fonts)
			lifnt_font_free (iter.value);
		lialg_strdic_free (self->fonts);
	}
	if (self->images != NULL)
	{
		LI_FOREACH_STRDIC (iter, self->images)
			liimg_texture_free (iter.value);
		lialg_strdic_free (self->images);
	}
	if (self->subimgs != NULL)
	{
		LI_FOREACH_STRDIC (iter, self->subimgs)
			free (iter.value);
		lialg_strdic_free (self->subimgs);
	}

	free (self);
}

lifntFont*
liwdg_manager_find_font (liwdgManager* self,
                         const char*   name)
{
	return lialg_strdic_find (self->fonts, name);
}

liwdgSubimage*
liwdg_manager_find_style (liwdgManager* self,
                          const char*   name)
{
	return lialg_strdic_find (self->subimgs, name);
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

	/* Cycle focus. */
	if (event->type == LIWDG_EVENT_TYPE_KEY_PRESS &&
	    event->key.keycode == SDLK_TAB)
	{
		if (event->key.modifiers & KMOD_SHIFT)
			liwdg_manager_focus_prev (self);
		else
			liwdg_manager_focus_next (self);
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
	int h;
	liwdgEvent evt;

	h = self->video.SDL_GetVideoSurface()->h - 1;

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
			evt.button.y = h - event->button.y;
			evt.button.button = event->button.button;
			break;
		case SDL_MOUSEBUTTONUP:
			evt.type = LIWDG_EVENT_TYPE_BUTTON_RELEASE;
			evt.button.x = event->button.x;
			evt.button.y = h - event->button.y;
			evt.button.button = event->button.button;
			break;
		case SDL_MOUSEMOTION:
			evt.type = LIWDG_EVENT_TYPE_MOTION;
			evt.motion.x = event->motion.x;
			evt.motion.y = h - event->motion.y;
			evt.motion.dx = event->motion.xrel;
			evt.motion.dy = -event->motion.yrel;
			evt.motion.buttons = event->motion.state;
			break;
		default:
			return 0;
	}

	return liwdg_manager_event (self, &evt);
}

void
liwdg_manager_focus_next (liwdgManager* self)
{
	if (!private_focus_next (self, self->focus.keyboard))
		private_focus_next (self, NULL);
}

void
liwdg_manager_focus_prev (liwdgManager* self)
{
	if (!private_focus_prev (self, self->focus.keyboard))
		private_focus_prev (self, NULL);
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

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
			liwdg_widget_get_allocation (widget, &rect);

			/* Check if inside frame. */
			if (rect.x <= x && x < rect.x + rect.width &&
				rect.y <= y && y < rect.y + rect.height)
			{
				*match = LIWDG_MATCH_INSIDE;
				return widget;
			}

			/* Check if inside titlebar. */
			if (rect.x <= x && x < rect.x + rect.width &&
				rect.y + rect.height <= y && y < rect.y + rect.height + TITLEBARH)
			{
				*match = LIWDG_MATCH_TITLEBAR;
				return widget;
			}

			/* Check if on corners. */
			if (rect.x - BORDERW <= x && x < rect.x &&
				rect.y + rect.height + TITLEBARH <= y && y < rect.y + rect.height + TITLEBARH + BORDERH)
			{
				*match = LIWDG_MATCH_TOPLEFT;
				return widget;
			}
			if (rect.x + rect.width <= x && x < rect.x + rect.width + BORDERW &&
				rect.y + rect.height + TITLEBARH <= y && y < rect.y + rect.height + TITLEBARH + BORDERH)
			{
				*match = LIWDG_MATCH_TOPRIGHT;
				return widget;
			}
			if (rect.x - BORDERW <= x && x < rect.x &&
				rect.y - BORDERH <= y && y < rect.y)
			{
				*match = LIWDG_MATCH_BOTTOMLEFT;
				return widget;
			}
			if (rect.x + rect.width <= x && x < rect.x + rect.width + BORDERW &&
				rect.y - BORDERH <= y && y < rect.y)
			{
				*match = LIWDG_MATCH_BOTTOMRIGHT;
				return widget;
			}

			/* Check if on borders. */
			if (rect.x <= x && x < rect.x + rect.width &&
				rect.y + rect.height + TITLEBARH <= y && y < rect.y + rect.height + TITLEBARH + BORDERH)
			{
				*match = LIWDG_MATCH_TOP;
				return widget;
			}
			if (rect.x <= x && x < rect.x + rect.width &&
				rect.y - BORDERH <= y && y < rect.y)
			{
				*match = LIWDG_MATCH_BOTTOM;
				return widget;
			}
			if (rect.x - BORDERW <= x && x < rect.x &&
				rect.y <= y && y < rect.y + rect.height + TITLEBARH)
			{
				*match = LIWDG_MATCH_LEFT;
				return widget;
			}
			if (rect.x + rect.width <= x && x < rect.x + rect.width + BORDERW &&
				rect.y <= y && y < rect.y + rect.height + TITLEBARH)
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
private_load_config (liwdgManager* self,
                     const char*   root)
{
	int size;
	char* file;
	char* path;
	char* field;
	liReader* reader;
	lifntFont* font;
	liimgTexture* image;
	liwdgSubimage* subimg;

	path = lisys_path_concat (root, "config", "widgets.cfg", NULL);
	if (path == NULL)
		return 0;
	reader = li_reader_new_from_file (path);
	free (path);
	if (reader == NULL)
	{
		if (lisys_error_get (NULL) != EIO)
			return 0;
		return 1;
	}

	li_reader_skip_chars (reader, " \t\n");
	while (!li_reader_check_end (reader))
	{
		if (!li_reader_get_text (reader, " ", &field))
			goto error;
		if (!strcmp (field, "widget"))
		{
			free (field);
			file = NULL;
			field = NULL;
			subimg = calloc (1, sizeof (liwdgSubimage));
			if (subimg == NULL)
				goto error;
			if (!li_reader_get_text (reader, " ", &field) ||
			    !li_reader_get_text (reader, " ", &file) ||
			    !li_reader_get_text_int (reader, &subimg->x) ||
			    !li_reader_skip_chars (reader, " \t") ||
			    !li_reader_get_text_int (reader, &subimg->y) ||
			    !li_reader_skip_chars (reader, " \t") ||
			    !li_reader_get_text_int (reader, subimg->w + 0) ||
			    !li_reader_skip_chars (reader, " \t") ||
			    !li_reader_get_text_int (reader, subimg->w + 1) ||
			    !li_reader_skip_chars (reader, " \t") ||
			    !li_reader_get_text_int (reader, subimg->w + 2) ||
			    !li_reader_skip_chars (reader, " \t") ||
			    !li_reader_get_text_int (reader, subimg->h + 2) ||
			    !li_reader_skip_chars (reader, " \t") ||
			    !li_reader_get_text_int (reader, subimg->h + 1) ||
			    !li_reader_skip_chars (reader, " \t") ||
			    !li_reader_get_text_int (reader, subimg->h + 0) ||
			    !li_reader_skip_chars (reader, " \t") ||
			    !li_reader_get_text_int (reader, subimg->pad + 0) ||
			    !li_reader_skip_chars (reader, " \t") ||
			    !li_reader_get_text_int (reader, subimg->pad + 1) ||
			    !li_reader_skip_chars (reader, " \t") ||
			    !li_reader_get_text_int (reader, subimg->pad + 2) ||
			    !li_reader_skip_chars (reader, " \t") ||
			    !li_reader_get_text_int (reader, subimg->pad + 3))
			{
				lisys_error_set (EINVAL, "invalid widget definition");
				free (subimg);
				free (file);
				free (field);
				goto error;
			}
			image = lialg_strdic_find (self->images, file);
			if (image == NULL)
			{
				if (!private_load_texture (self, &image, root, file))
				{
					free (subimg);
					free (file);
					free (field);
					goto error;
				}
				if (!lialg_strdic_insert (self->images, file, image))
				{
					liimg_texture_free (image);
					free (subimg);
					free (file);
					free (field);
					goto error;
				}
			}
			free (file);
			subimg->texture = image;
			if (!lialg_strdic_insert (self->subimgs, field, subimg))
			{
				free (subimg);
				free (field);
				goto error;
			}
			free (field);
		}
		else if (!strcmp (field, "font"))
		{
			free (field);
			file = NULL;
			field = NULL;
			if (!li_reader_get_text (reader, " ", &field) ||
			    !li_reader_get_text (reader, " ", &file) ||
			    !li_reader_get_text_int (reader, &size))
			{
				lisys_error_set (EINVAL, "invalid font definition");
				free (field);
				free (file);
				goto error;
			}
			font = private_load_font (self, root, file, size);
			if (font == NULL)
			{
				free (file);
				free (field);
				goto error;
			}
			if (!lialg_strdic_insert (self->fonts, field, font))
			{
				lifnt_font_free (font);
				free (file);
				free (field);
				goto error;
			}
			free (field);
			free (file);
		}
		else
		{
			free (field);
			goto error;
		}
		li_reader_skip_chars (reader, " \t\n");
	}
	li_reader_free (reader);

	return 1;

error:
	li_reader_free (reader);
	return 0;
}

static lifntFont*
private_load_font (liwdgManager* self,
                   const char*   root,
                   const char*   name,
                   int           size)
{
	char* path;
	lifntFont* font;

	path = lisys_path_concat (root, "fonts", name, NULL);
	if (path == NULL)
		return NULL;
	font = lifnt_font_new (&self->video, path, size);
	free (path);

	return font;
}

static int
private_load_texture (liwdgManager*  self,
                      liimgTexture** texture,
                      const char*    root,
                      const char*    name)
{
	char* path;

	path = lisys_path_concat (root, "graphics", name, NULL);
	if (path == NULL)
		return 0;
	*texture = liimg_texture_new_from_file (path);
	free (path);
	if (*texture == NULL)
		return 0;
	return 1;
}

static int
private_focus_next (liwdgManager* self,
                    liwdgWidget*  start)
{
	liwdgWidget* tmp;
	liwdgWidget* widget;

	/* Choose the starting position. */
	if (start == NULL)
	{
		/* Get the first dialog. */
		start = private_get_next_dialog (self, NULL);
		if (start == NULL)
			return 0;

		/* Focus it if focusable. */
		if (liwdg_widget_get_focusable (start))
		{
			liwdg_manager_set_focus_keyboard (self, start);
			return 1;
		}

		/* Focus the first focusable child. */
		if (liwdg_widget_typeis (start, &liwdgGroupType))
		{
			tmp = private_get_next_focusable (self, LIWDG_GROUP (start), NULL);
			if (tmp != NULL)
			{
				liwdg_manager_set_focus_keyboard (self, tmp);
				return 1;
			}
		}
	}

	/* Find the next focusable widget. */
	while (1)
	{
		/* Loop through children. */
		/* Does nothing if we started from an unfocusable dialog. */
		for (widget = start ; widget->parent != NULL ; widget = widget->parent)
		{
			tmp = private_get_next_focusable (self, LIWDG_GROUP (widget->parent), widget);
			if (tmp != NULL)
			{
				liwdg_manager_set_focus_keyboard (self, tmp);
				return 1;
			}
		}

		/* Get the next dialog. */
		start = private_get_next_dialog (self, widget);
		if (start == NULL)
			return 0;

		/* Focus it if focusable. */
		if (liwdg_widget_get_focusable (start))
		{
			liwdg_manager_set_focus_keyboard (self, start);
			return 1;
		}

		/* Focus the first focusable child. */
		if (liwdg_widget_typeis (start, &liwdgGroupType))
		{
			tmp = private_get_next_focusable (self, LIWDG_GROUP (start), NULL);
			if (tmp != NULL)
			{
				liwdg_manager_set_focus_keyboard (self, tmp);
				return 1;
			}
		}
	}
}

static int
private_focus_prev (liwdgManager* self,
                    liwdgWidget*  start)
{
	liwdgWidget* tmp;
	liwdgWidget* widget;

	/* Choose the starting position. */
	if (start == NULL)
	{
		/* Get the last dialog. */
		start = private_get_prev_dialog (self, NULL);
		if (start == NULL)
			return 0;

		/* Focus it if focusable. */
		if (liwdg_widget_get_focusable (start))
		{
			liwdg_manager_set_focus_keyboard (self, start);
			return 1;
		}

		/* Focus the last focusable child. */
		if (liwdg_widget_typeis (start, &liwdgGroupType))
		{
			tmp = private_get_prev_focusable (self, LIWDG_GROUP (start), NULL);
			if (tmp != NULL)
			{
				liwdg_manager_set_focus_keyboard (self, tmp);
				return 1;
			}
		}
	}

	/* Find the next focusable widget. */
	while (1)
	{
		/* Loop through children. */
		/* Does nothing if we started from an unfocusable dialog. */
		for (widget = start ; widget->parent != NULL; widget = widget->parent)
		{
			tmp = private_get_prev_focusable (self, LIWDG_GROUP (widget->parent), widget);
			if (tmp != NULL)
			{
				liwdg_manager_set_focus_keyboard (self, tmp);
				return 1;
			}
		}

		/* Get the previous dialog. */
		start = private_get_prev_dialog (self, widget);
		if (start == NULL)
			return 0;

		/* Focus it if focusable. */
		if (liwdg_widget_get_focusable (start))
		{
			liwdg_manager_set_focus_keyboard (self, start);
			return 1;
		}

		/* Focus the last focusable child. */
		if (liwdg_widget_typeis (start, &liwdgGroupType))
		{
			tmp = private_get_prev_focusable (self, LIWDG_GROUP (start), NULL);
			if (tmp != NULL)
			{
				liwdg_manager_set_focus_keyboard (self, tmp);
				return 1;
			}
		}
	}
}

static liwdgWidget*
private_get_next_dialog (liwdgManager* self,
                         liwdgWidget*  curr)
{
	liwdgWidget* widget;

	/* Find old position. */
	if (curr == self->widgets.root)
		widget = self->widgets.dialogs;
	else if (curr != NULL)
		widget = curr->next;
	else
		widget = NULL;

	/* Find next visible dialog. */
	for ( ; widget != NULL ; widget = widget->next)
	{
		if (liwdg_widget_get_visible (widget))
			return widget;
	}

	/* Return root if no more visiable dialogs. */
	if (self->widgets.root != NULL)
	{
		if (liwdg_widget_get_visible (self->widgets.root))
			return self->widgets.root;
	}

	return NULL;
}

static liwdgWidget*
private_get_prev_dialog (liwdgManager* self,
                         liwdgWidget*  curr)
{
	liwdgWidget* widget;

	/* Find old position. */
	if (curr == self->widgets.root)
		widget = self->widgets.active;
	else if (curr != NULL)
		widget = curr->prev;
	else
		widget = NULL;

	/* Find previous visible dialog. */
	for ( ; widget != NULL ; widget = widget->prev)
	{
		if (liwdg_widget_get_visible (widget))
			return widget;
	}

	/* Return root if no more visiable dialogs. */
	if (self->widgets.root != NULL)
	{
		if (liwdg_widget_get_visible (self->widgets.root))
			return self->widgets.root;
	}

	return NULL;
}

/**
 * \brief Gets the next focusable widget under the group.
 *
 * Searches nested groups recursively.
 *
 * \param self A widget manager.
 * \param group A widget group.
 * \param curr The current focused widget in the group or NULL.
 * \return A widget or NULL.
 */
static liwdgWidget*
private_get_next_focusable (liwdgManager* self,
                            liwdgGroup*   group,
                            liwdgWidget*  curr)
{
	int x = 0;
	int y = group->height - 1;
	liwdgWidget* tmp;
	liwdgWidget* child;

	/* Find old position. */
	if (curr != NULL)
	{
		for (y = group->height - 1 ; y >= 0 ; y--)
		{
			for (x = group->width - 1 ; x >= 0 ; x--)
			{
				child = group->cells[x + y * group->width].child;
				if (child == curr)
					goto out;
			}
		}
	out:
		if (x < group->width)
			x++;
		else
			y--;
	}

	/* Iterate forwards. */
	for ( ; y >= 0 ; y--, x = 0)
	for ( ; x < group->width ; x++)
	{
		child = group->cells[x + y * group->width].child;
		if (child == NULL)
			continue;
		if (liwdg_widget_typeis (child, &liwdgGroupType))
		{
			/* Enter a child group. */
			tmp = private_get_next_focusable (self, LIWDG_GROUP (child), NULL);
			if (tmp != NULL)
				return tmp;
		}
		else
		{
			/* Try a child widget. */
			if (liwdg_widget_get_focusable (child))
				return child;
		}
	}

	return NULL;
}

/**
 * \brief Gets the previous focusable widget under the group.
 *
 * Searches nested groups recursively.
 *
 * \param self A widget manager.
 * \param group A widget group.
 * \param curr The current focused widget in the group or NULL.
 * \return A widget or NULL.
 */
static liwdgWidget*
private_get_prev_focusable (liwdgManager* self,
                            liwdgGroup*   group,
                            liwdgWidget*  curr)
{
	int x = group->width - 1;
	int y = 0;
	liwdgWidget* tmp;
	liwdgWidget* child;

	/* Find old position. */
	if (curr != NULL)
	{
		for (y = group->height - 1 ; y >= 0 ; y--)
		for (x = group->width - 1 ; x >= 0 ; x--)
		{
			child = group->cells[x + y * group->width].child;
			if (child == curr)
				goto out;
		}
out:
		if (x)
			x--;
		else
			y++;
	}

	/* Iterate backwards. */
	for ( ; y < group->height ; y++, x = group->width - 1)
	for ( ; x >= 0 ; x--)
	{
		child = group->cells[x + y * group->width].child;
		if (child == NULL)
			continue;
		if (liwdg_widget_typeis (child, &liwdgGroupType))
		{
			tmp = private_get_prev_focusable (self, LIWDG_GROUP (child), NULL);
			if (tmp != NULL)
				return tmp;
		}
		else
		{
			if (liwdg_widget_get_focusable (child))
				return child;
		}
	}

	return NULL;
}

/** @} */
/** @} */
