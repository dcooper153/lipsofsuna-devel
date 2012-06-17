/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#include "lipsofsuna/system.h"
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

static void private_attach_window (
	LIWdgManager* self,
	LIWdgWidget*  widget);

static void private_detach_window (
	LIWdgManager* self,
	LIWdgWidget*  widget);

static LIWdgWidget* private_find_window (
	LIWdgManager* self,
	int           x,
	int           y,
	int*          match);

static void private_resize_window (
	LIWdgManager* self,
	LIWdgWidget*  window);

/*****************************************************************************/

/**
 * \brief Creates a new widget manager.
 * \param render Renderer.
 * \param callbacks Callback manager.
 * \param paths Paths used for loading data files.
 * \return New widget manager or NULL.
 */
LIWdgManager* liwdg_manager_new (
	LIRenRender*    render,
	LICalCallbacks* callbacks,
	LIPthPaths*     paths)
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
	self->projection = limat_matrix_identity ();

	/* Initialize widget dictionary. */
	self->widgets.all = lialg_ptrdic_new ();
	if (self->widgets.all == NULL)
	{
		liwdg_manager_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Freed the widget manager.
 * \param self Widget manager.
 */
void liwdg_manager_free (
	LIWdgManager* self)
{
	lisys_assert (self->dialogs.bottom == NULL);
	lisys_assert (self->dialogs.top == NULL);

	if (self->widgets.all != NULL)
		lialg_ptrdic_free (self->widgets.all);
	lisys_free (self);
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
LIWdgWidget* liwdg_manager_find_window_by_point (
	LIWdgManager* self,
	int           x,
	int           y)
{
	int match;

	return private_find_window (self, x, y, &match);
}

int liwdg_manager_insert_window (
	LIWdgManager* self,
	LIWdgWidget*  widget)
{
	LIWdgSize size;

	/* Detach and attach. */
	liwdg_widget_detach (widget);
	widget->floating = 1;
	private_attach_window (self, widget);

	/* Set the geometry of the window. */
	liwdg_widget_get_request (widget, &size);
	size.width = (self->width - size.width) / 2;
	size.height = (self->height - size.height) / 2;
	liwdg_widget_move (widget, size.width, size.height);
	private_resize_window (self, widget);

	return 1;
}

int liwdg_manager_remove_window (
	LIWdgManager* self,
	LIWdgWidget*  widget)
{
	lisys_assert (widget->floating);
	lisys_assert (widget->above != NULL || widget == self->dialogs.top);
	lisys_assert (widget->below != NULL || widget == self->dialogs.bottom);

	/* Make sure that the update loop doesn't break. */
	if (self->widgets.iter == widget)
		self->widgets.iter = widget->below;

	/* Remove from stack. */
	private_detach_window (self, widget);
	widget->floating = 0;

	return 1;
}

void liwdg_manager_sort_windows (
	LIWdgManager* self)
{
	int depth;
	LIWdgWidget* widget;

	/* Update render overlays. */
	for (depth = 0, widget = self->dialogs.bottom ; widget != NULL ; widget = widget->above)
	{
		if (liwdg_widget_get_visible (widget))
		{
			liren_render_overlay_set_depth (self->render, widget->overlay, 100*depth);
			depth++;
		}
	}
}

void liwdg_manager_update (
	LIWdgManager* self,
	float         secs)
{
	LIWdgWidget* widget;

	for (widget = self->dialogs.top ; widget != NULL ; widget = self->widgets.iter)
	{
		self->widgets.iter = widget->below;
		private_resize_window (self, widget);
	}
}

/**
 * \brief Gets the projection matrix used for rendering widgets.
 * \param self Widget manager.
 * \param matrix Return location for the projection matrix.
 */
void liwdg_manager_get_projection (
	LIWdgManager* self,
	LIMatMatrix*  matrix)
{
	*matrix = self->projection;
}

/**
 * \brief Gets the screen size.
 * \param self Widget manager.
 * \param width Return location for the width or NULL.
 * \param height Return location for the height or NULL.
 */
void liwdg_manager_get_size (
	LIWdgManager* self,
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
 * \param self Widget manager.
 * \param width Width in pixels.
 * \param height Height in pixels.
 */
void liwdg_manager_set_size (
	LIWdgManager* self,
	int           width,
	int           height)
{
	/* Update projection matrix. */
	self->width = width;
	self->height = height;
	self->projection = limat_matrix_ortho (0.0f, width, height, 0.0f, -100.0f, 100.0f);
}

/*****************************************************************************/

static void private_attach_window (
	LIWdgManager* self,
	LIWdgWidget*  widget)
{
	LIWdgWidget* below;

	/* Find the insertion position based on the depth. */
	/* Simply find the first widget from the top that has a depth less
	   than ours. We'll link ourselves above that widget. */
	for (below = self->dialogs.top ; below != NULL ; below = below->below)
	{
		if (below->depth < widget->depth)
			break;
	}

	/* Add the widget to the dialog list. */
	/* If our depth was the lowest, make us the new bottom widget.
	   Otherwise, link ourselves above the found widget. */
	if (below == NULL)
	{
		widget->above = self->dialogs.bottom;
		widget->below = NULL;
		if (widget->above != NULL)
			widget->above->below = widget;
		else
			self->dialogs.top = widget;
		self->dialogs.bottom = widget;
	}
	else
	{
		widget->above = below->above;
		widget->below = below;
		below->above = widget;
		if (widget->above != NULL)
			widget->above->below = widget;
		else
			self->dialogs.top = widget;
	}

	/* Add the overlay to the root. */
	liren_render_overlay_set_floating (self->render, widget->overlay, 1);
}

static void private_detach_window (
	LIWdgManager* self,
	LIWdgWidget*  widget)
{
	/* Remove the widget from the dialog list. */
	if (widget->below != NULL)
		widget->below->above = widget->above;
	else
		self->dialogs.bottom = widget->above;
	if (widget->above != NULL)
		widget->above->below = widget->below;
	else
		self->dialogs.top = widget->below;

	/* Remove the overlay from the root. */
	liren_render_overlay_set_floating (self->render, widget->overlay, 0);
}

static LIWdgWidget* private_find_window (
	LIWdgManager* self,
	int           x,
	int           y,
	int*          match)
{
	LIWdgRect rect;
	LIWdgWidget* widget;

	for (widget = self->dialogs.top ; widget != NULL ; widget = widget->below)
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

static void private_resize_window (
	LIWdgManager* self,
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

/** @} */
/** @} */
