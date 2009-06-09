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
 * \addtogroup liwdgGroup Group
 * @{
 */

#include <assert.h>
#include <math/lips-math.h>
#include "widget-group.h"
#include "widget-window.h"

#define LIWDG_GROUP_DEFAULT_SPACING 5

static int
private_init (liwdgGroup*   self,
              liwdgManager* manager);

static void
private_free (liwdgGroup* self);

static int
private_event (liwdgGroup* self,
               liwdgEvent* event);

static void
private_cell_changed (liwdgGroup* self,
                      int         x,
                      int         y);

static int
private_get_col_size (liwdgGroup* self,
                      int         x);

static int
private_get_row_size (liwdgGroup* self,
                      int         y);

static void
private_rebuild_horz (liwdgGroup* self);

static void
private_rebuild_vert (liwdgGroup* self);

static void
private_rebuild_children (liwdgGroup* self);

static void
private_rebuild_request (liwdgGroup* self);

const liwdgClass liwdgGroupType =
{
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Group", sizeof (liwdgGroup),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/****************************************************************************/

/**
 * \brief Creates a new group widget.
 *
 * \param manager Widget manager.
 * \return New group widget or NULL.
 */
liwdgWidget*
liwdg_group_new (liwdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdgGroupType);
}

/**
 * \brief Creates a new group widget and sets its size.
 *
 * \param manager Widget manager.
 * \param cols Number of columns.
 * \param rows Number of rows.
 * \return New group widget or NULL.
 */
liwdgWidget*
liwdg_group_new_with_size (liwdgManager* manager,
                           int           cols,
                           int           rows)
{
	liwdgWidget* self;

	self = liwdg_widget_new (manager, &liwdgGroupType);
	if (self == NULL)
		return NULL;
	if (!liwdg_group_set_size (LIWDG_GROUP (self), cols, rows))
	{
		liwdg_widget_free (self);
		return NULL;
	}
	return self;
}

/**
 * \brief Updates the layout after the size of a child changing.
 *
 * \param self A group widget.
 * \param child A widget.
 */
void
liwdg_group_child_request (liwdgGroup*  self,
                           liwdgWidget* child)
{
	int x;
	int y;
	liwdgSize size;
	liwdgGroupCell* cell;

	for (y = 0 ; y < self->height ; y++)
	{
		for (x = 0 ; x < self->width ; x++)
		{
			cell = self->cells + x + y * self->width;
			if (cell->child == child)
			{
				liwdg_widget_get_request (cell->child, &size);
				if (self->cols[x].request < size.width ||
				    self->rows[y].request < size.height)
					private_cell_changed (self, x, y);
				return;
			}
		}
	}
	assert (0 && "Invalid child request");
}

/**
 * \brief Finds and unparents a child widget.
 *
 * \param self Group.
 * \param child Child widget.
 */
void
liwdg_group_detach_child (liwdgGroup*  self,
                          liwdgWidget* child)
{
	int x;
	int y;
	liwdgGroupCell* cell;

	for (y = 0 ; y < self->height ; y++)
	{
		for (x = 0 ; x < self->width ; x++)
		{
			cell = self->cells + x + y * self->width;
			if (cell->child == child)
			{
				liwdg_group_set_child (self, x, y, NULL);
				return;
			}
		}
	}
}

/**
 * \brief Gets the size of a cell.
 *
 * \param self Group.
 * \param x Column number.
 * \param y Row number.
 * \param rect Return location for the size.
 */
void
liwdg_group_get_cell_rect (liwdgGroup* self,
                           int         x,
                           int         y,
                           liwdgRect*  rect)
{
	rect->x = LIWDG_WIDGET (self)->allocation.x + self->cols[x].start;
	rect->y = LIWDG_WIDGET (self)->allocation.y + self->rows[y].start;
	rect->width = self->cols[x].allocation;
	rect->height = self->rows[y].allocation;
}

/**
 * \brief Gets a child widget.
 *
 * \param self Group.
 * \param x Column number.
 * \param y Row number.
 * \return Widget owned by the group or NULL.
 */
liwdgWidget*
liwdg_group_get_child (liwdgGroup* self,
                       int         x,
                       int         y)
{
	assert (x < self->width);
	assert (y < self->height);
	return self->cells[x + y * self->width].child;
}

/**
 * \brief Sets a child widget in the given cell position.
 *
 * \param self Group.
 * \param x Column number.
 * \param y Row number.
 * \param child Widget or NULL.
 */
void
liwdg_group_set_child (liwdgGroup*  self,
                       int          x,
                       int          y,
                       liwdgWidget* child)
{
	liwdgGroupCell* cell;

	assert (x < self->width);
	assert (y < self->height);

	/* Detach the old child. */
	cell = self->cells + x + y * self->width;
	if (cell->child != NULL)
	{
		assert (cell->child->state == LIWDG_WIDGET_STATE_DETACHED);
		assert (cell->child->parent == LIWDG_WIDGET (self));
		cell->child->parent = NULL;
	}

	/* Attach the new child. */
	cell->child = child;
	if (child != NULL)
	{
		assert (child->parent == NULL);
		assert (child->state == LIWDG_WIDGET_STATE_DETACHED);
		child->parent = LIWDG_WIDGET (self);
	}

	/* Ensure valid focus. */
	liwdg_manager_fix_focus (LIWDG_WIDGET (self)->manager);

	/* Update the size of the cell. */
	private_cell_changed (self, x, y);
}

/**
 * \brief Gets a child widget by cursor position.
 *
 * \param self Group.
 * \param pixx Cursor position.
 * \param pixy Cursor position.
 * \return Widget owned by the group or NULL.
 */
liwdgWidget*
liwdg_group_get_child_at (liwdgGroup* self,
                          int         pixx,
                          int         pixy)
{
	int x;
	int y;

	pixx -= LIWDG_WIDGET (self)->allocation.x;
	pixy -= LIWDG_WIDGET (self)->allocation.y;

	/* Get column. */
	for (x = 0 ; x < self->width ; x++)
	{
		if (pixx >= self->cols[x].start + self->cols[x].allocation)
			continue;
		if (pixx >= self->cols[x].start)
			break;
		return NULL;
	}
	if (x == self->width)
		return NULL;

	/* Get row. */
	for (y = 0 ; y < self->height ; y++)
	{
		if (pixy >= self->rows[y].start + self->rows[y].allocation)
			continue;
		if (pixy >= self->rows[y].start)
			break;
		return NULL;
	}
	if (y == self->height)
		return NULL;

	/* Return the child. */
	return self->cells[x + y * self->width].child;
}

/**
 * \brief Gets the column expand status of a column.
 *
 * \param self Group.
 * \param x Column number.
 * \return Nonzero if the column is set to expand.
 */
int
liwdg_group_get_col_expand (liwdgGroup* self,
                            int         x)
{
	return self->cols[x].expand;
}

/**
 * \brief Sets the expand attribute of a column.
 *
 * \param self Group.
 * \param x Column number.
 * \param expand Nonzero if should expand.
 */
void
liwdg_group_set_col_expand (liwdgGroup* self,
                            int         x,
                            int         expand) 
{
	if (self->cols[x].expand != expand)
	{
		if (expand)
			self->col_expand++;
		else
			self->col_expand--;
		self->cols[x].expand = expand;
		private_rebuild_horz (self);
		private_rebuild_children (self);
	}
}

/**
 * \brief Gets the allocation of a column in pixels.
 *
 * \param self Group.
 * \param x Column number.
 * \return Horizontal allocation of the column in pixels.
 */
int
liwdg_group_get_col_size (liwdgGroup* self,
                          int         x)
{
	return self->cols[x].allocation;
}

/**
 * \brief Gets the margins of group in pixels.
 *
 * \param self Group.
 * \param left Return location for the left margin.
 * \param right Return location for the right margin.
 * \param top Return location for the top margin.
 * \param bottom Return location for the bottom margin.
 */
void
liwdg_group_get_margins (liwdgGroup* self,
                         int*        left,
                         int*        right,
                         int*        top,
                         int*        bottom)
{
	if (left != NULL) *left = self->margin_left;
	if (right != NULL) *right = self->margin_right;
	if (top != NULL) *top = self->margin_top;
	if (bottom != NULL) *bottom = self->margin_bottom;
}

/**
 * \brief Sets the margins of the group.
 *
 * \param self Group.
 * \param left Left margin in pixels.
 * \param right Right margin in pixels.
 * \param top Top margin in pixels.
 * \param bottom Bottom margin in pixels.
 */
void
liwdg_group_set_margins (liwdgGroup* self,
                         int         left,
                         int         right,
                         int         top,
                         int         bottom)
{
	/* Set horizontal margins. */
	if (self->margin_left != left ||
	    self->margin_right != right)
	{
		self->margin_left = left;
		self->margin_right = right;
		private_rebuild_horz (self);
	}

	/* Set vertical margins. */
	if (self->margin_top != top ||
	    self->margin_bottom != bottom)
	{
		self->margin_top = top;
		self->margin_bottom = bottom;
		private_rebuild_vert (self);
	}

	/* Rebuild the layout. */
	private_rebuild_children (self);
}

/**
 * \brief Gets the row expand status of a row.
 *
 * \param self Group.
 * \param y Row number.
 * \return Nonzero if the row is set to expand.
 */
int
liwdg_group_get_row_expand (liwdgGroup* self,
                            int         y)
{
	return self->rows[y].expand;
}

/**
 * \brief Sets the expand attribute of a row.
 *
 * \param self Group.
 * \param y Row number.
 * \param expand Nonzero if should expand.
 */
void
liwdg_group_set_row_expand (liwdgGroup* self,
                            int         y,
                            int         expand)
{
	if (self->rows[y].expand != expand)
	{
		if (expand)
			self->row_expand++;
		else
			self->row_expand--;
		self->rows[y].expand = expand;
		private_rebuild_vert (self);
		private_rebuild_children (self);
	}
}

/**
 * \brief Gets the allocation of a row in pixels.
 *
 * \param self Group.
 * \param y Row number.
 * \return Vertical allocation of the row in pixels.
 */
int
liwdg_group_get_row_size (liwdgGroup* self,
                          int         y)
{
	return self->rows[y].allocation;
}

/**
 * \brief Gets the number of columns and rows.
 *
 * \param self Group.
 * \param cols Return location for the number of columns.
 * \param rows Return location for the number of rows.
 */
void
liwdg_group_get_size (liwdgGroup* self,
                      int*        cols,
                      int*        rows)
{
	if (cols != NULL) *cols = self->width;
	if (rows != NULL) *rows = self->height;
}

/**
 * \brief Sets the number of columns and rows.
 *
 * \param self Group.
 * \param width Number of columns.
 * \param height Number of rows.
 * \return Nonzero on success.
 */
int
liwdg_group_set_size (liwdgGroup* self,
                      int         width,
                      int         height)
{
	int x;
	int y;
	liwdgGroupCol* mem0;
	liwdgGroupRow* mem1;
	liwdgGroupCell* mem2;

	/* Allocate memory. */
	mem0 = (liwdgGroupCol*) calloc (width, sizeof (liwdgGroupCol));
	if (mem0 == NULL)
	{
		return 0;
	}
	mem1 = (liwdgGroupRow*) calloc (height, sizeof (liwdgGroupRow));
	if (mem1 == NULL)
	{
		free (mem0);
		return 0;
	}
	mem2 = (liwdgGroupCell*) calloc (width * height, sizeof (liwdgGroupCell));
	if (mem2 == NULL)
	{
		free (mem0);
		free (mem1);
		return 0;
	}

	/* Copy over the column data. */
	if (self->width < width)
		memcpy (mem0, self->cols, self->width * sizeof (liwdgGroupCol));
	else
		memcpy (mem0, self->cols, width * sizeof (liwdgGroupCol));
	free (self->cols);
	self->cols = mem0;

	/* Copy over the row data. */
	if (self->height < height)
		memcpy (mem1, self->rows, self->height * sizeof (liwdgGroupRow));
	else
		memcpy (mem1, self->rows, height * sizeof (liwdgGroupRow));
	free (self->rows);
	self->rows = mem1;

	/* Copy over the cell data. */
	for (y = 0 ; y < height && y < self->height ; y++)
	{
		for (x = 0 ; x < width && x < self->width ; x++)
		{
			mem2[x + y * width].child = self->cells[x + y * self->width].child;
		}
	}
	free (self->cells);
	self->cells = mem2;
	self->width = width;
	self->height = height;

	/* Update the expansion information. */
	self->col_expand = 0;
	self->row_expand = 0;
	for (x = 0 ; x < width ; x++)
	{
		if (self->cols[x].expand)
			self->col_expand++;
	}
	for (y = 0 ; y < height ; y++)
	{
		if (self->rows[y].expand)
			self->row_expand++;
	}

	/* Ensure valid focus. */
	liwdg_manager_fix_focus (LIWDG_WIDGET (self)->manager);

	/* Update the size request. */
	private_rebuild_request (self);
	return 1;
}

/**
 * \brief Gets the row and column spacings in pixels.
 *
 * \param self Group.
 * \param column Return location for the column spacing.
 * \param row Return location for the row spacing.
 */
void
liwdg_group_get_spacings (liwdgGroup* self,
                          int*        column,
                          int*        row)
{
	if (column != NULL) *column = self->col_spacing;
	if (row != NULL) *row = self->row_spacing;
}

/**
 * \brief Sets row and column spacings.
 *
 * \param self Group.
 * \param column Spacing between columns.
 * \param row Spacing between rows.
 */
void
liwdg_group_set_spacings (liwdgGroup* self,
                          int         column,
                          int         row)
{
	if (self->col_spacing != column &&
	    self->row_spacing != row)
	{
		self->col_spacing = column;
		self->row_spacing = row;
		private_rebuild_horz (self);
		private_rebuild_vert (self);
		private_rebuild_children (self);
	}
	else if (self->col_spacing != column)
	{
		self->col_spacing = column;
		private_rebuild_horz (self);
		private_rebuild_children (self);
	}
	else if (self->row_spacing != row)
	{
		self->row_spacing = row;
		private_rebuild_vert (self);
		private_rebuild_children (self);
	}
}

/****************************************************************************/

static int
private_init (liwdgGroup*   self,
              liwdgManager* manager)
{
	/* Allocate memory. */
	self->cols = calloc (1, sizeof (liwdgGroupCol));
	if (self->cols == NULL)
	{
		return 0;
	}
	self->rows = calloc (1, sizeof (liwdgGroupRow));
	if (self->rows == NULL)
	{
		free (self->cols);
		return 0;
	}
	self->cells = calloc (1, sizeof (liwdgGroupCell));
	if (self->cells == NULL)
	{
		free (self->cols);
		free (self->rows);
		return 0;
	}
	self->width = 1;
	self->height = 1;
	self->col_expand = 0;
	self->row_expand = 0;
	self->col_spacing = LIWDG_GROUP_DEFAULT_SPACING;
	self->row_spacing = LIWDG_GROUP_DEFAULT_SPACING;
	self->margin_left = 0;
	self->margin_right = 0;
	self->margin_top = 0;
	self->margin_bottom = 0;
	return 1;
}

static void
private_free (liwdgGroup* self)
{
	int i;

	/* Remove child focus. */
	i = LIWDG_WIDGET (self)->visible;
	LIWDG_WIDGET (self)->visible = 0;
	liwdg_manager_fix_focus (LIWDG_WIDGET (self)->manager);
	LIWDG_WIDGET (self)->visible = i;

	/* Free childrem. */
	for (i = 0 ; i < self->width * self->height ; i++)
	{
		if (self->cells[i].child != NULL)
			liwdg_widget_free (self->cells[i].child);
	}

	free (self->cols);
	free (self->rows);
	free (self->cells);
}

static int
private_event (liwdgGroup* self,
               liwdgEvent* event)
{
	int i;
	int x;
	int y;
	liwdgWidget* child;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_ALLOCATION:
			private_rebuild_horz (self);
			private_rebuild_vert (self);
			private_rebuild_children (self);
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			for (i = 0 ; i < self->width * self->height ; i++)
			{
				child = self->cells[i].child;
				if (child != NULL)
					liwdg_widget_render (child);
			}
			return 1;
		case LIWDG_EVENT_TYPE_UPDATE:
			for (i = 0 ; i < self->width * self->height ; i++)
			{
				child = self->cells[i].child;
				if (child != NULL)
					liwdg_widget_update (child, event->update.secs);
			}
			return 1;
	}

	if (!liwdg_widget_get_visible (LIWDG_WIDGET (self)))
		return 1;

	/* Get cursor position. */
	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			x = event->button.x;
			y = event->button.y;
			break;
		case LIWDG_EVENT_TYPE_MOTION:
			x = event->motion.x;
			y = event->motion.y;
			break;
		default:
			return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
	}

	/* Get the affected widget. */
	child = liwdg_group_get_child_at (self, x, y);
	if (child == NULL)
		return 1;

	/* Propagate event. */
	liwdg_widget_set_focus_mouse (child);
	if (!liwdg_widget_event (child, event))
		return 0;

	return 1;
}

static void
private_cell_changed (liwdgGroup* self,
                      int         x,
                      int         y)
{
	liwdgSize size;
	liwdgWidget* child;

	/* Get the row and column size requests. */
	size.width = private_get_col_size (self, x);
	size.height = private_get_row_size (self, y);
	child = self->cells[x + y * self->width].child;

	/* Rebuild if the requests have changed. */
	if (size.width  != self->cols[x].request &&
	    size.height != self->rows[y].request)
	{
		/* Both horizontal and vertical layout changed. */
		self->cols[x].request = size.width;
		self->rows[y].request = size.height;
		private_rebuild_horz (self);
		private_rebuild_vert (self);
		private_rebuild_children (self);
	}
	else if (size.width != self->cols[x].request)
	{
		/* Only vertical layout changed. */
		self->cols[x].request = size.width;
		private_rebuild_horz (self);
		private_rebuild_children (self);
	}
	else if (size.height != self->rows[y].request)
	{
		/* Only horizontal layout changed. */
		self->rows[y].request = size.height;
		private_rebuild_vert (self);
		private_rebuild_children (self);
	}
	else if (child != NULL/* && LIWDG_WIDGET (self)->allocated && !child->allocated*/)
	{
		/* Only set allocation of the widget. */
		liwdg_widget_set_allocation (child,
			LIWDG_WIDGET (self)->allocation.x + self->cols[x].start,
			LIWDG_WIDGET (self)->allocation.y + self->rows[y].start,
			self->cols[x].allocation,
			self->rows[y].allocation);
	}
}

static int
private_get_col_size (liwdgGroup* self,
                      int         x)
{
	int y;
	int width;
	liwdgSize size;
	liwdgWidget* child;

	width = 0;
	for (y = 0 ; y < self->height ; y++)
	{
		child = self->cells[x + y * self->width].child;
		if (child != NULL && child->visible)
		{
			liwdg_widget_get_request (child, &size);
			if (width < size.width)
				width = size.width;
		}
	}
	return width;
}

static int
private_get_row_size (liwdgGroup* self,
                      int         y)
{
	int x;
	int height;
	liwdgSize size;
	liwdgWidget* child;

	height = 0;
	for (x = 0 ; x < self->width ; x++)
	{
		child = self->cells[x + y * self->width].child;
		if (child != NULL && child->visible)
		{
			liwdg_widget_get_request (child, &size);
			if (height < size.height)
				height = size.height;
		}
	}
	return height;
}

static void
private_rebuild_horz (liwdgGroup* self)
{
	int x;
	int start;
	int expand;

	/* Get horizontal expansion. */
	if (self->col_expand > 0)
	{
		expand = LIWDG_WIDGET (self)->allocation.width - self->margin_left - self->margin_right;
		if (self->width > 1)
			expand -= (self->width - 1) * self->col_spacing;
		for (x = 0 ; x < self->width ; x++)
			expand -= self->cols[x].request;
		expand /= self->col_expand;
	}
	else
		expand = 0;

	/* Set horizontal allocations. */
	start = self->margin_left;
	for (x = 0 ; x < self->width ; x++)
	{
		self->cols[x].start = start;
		self->cols[x].allocation = self->cols[x].request;
		if (self->cols[x].expand)
			self->cols[x].allocation += expand;
		start += self->cols[x].allocation + self->col_spacing;
	}
}

static void
private_rebuild_vert (liwdgGroup* self)
{
	int y;
	int start;
	int expand;

	/* Get vertical expansion. */
	if (self->row_expand > 0)
	{
		expand = LIWDG_WIDGET (self)->allocation.height - self->margin_top - self->margin_bottom;
		if (self->height > 1)
			expand -= (self->height - 1) * self->row_spacing;
		for (y = 0 ; y < self->height ; y++)
			expand -= self->rows[y].request;
		expand /= self->row_expand;
	}
	else
		expand = 0;

	/* Set vertical allocations. */
	start = self->margin_top;
	for (y = 0 ; y < self->height ; y++)
	{
		self->rows[y].start = start;
		self->rows[y].allocation = self->rows[y].request;
		if (self->rows[y].expand)
			self->rows[y].allocation += expand;
		start += self->rows[y].allocation + self->row_spacing;
	}
}

static void
private_rebuild_children (liwdgGroup* self)
{
	int x;
	int y;
	liwdgWidget* child;

	/* Set positions of widgets. */
	for (x = 0 ; x < self->width ; x++)
	{
		for (y = 0 ; y < self->height ; y++)
		{
			child = self->cells[x + y * self->width].child;
			if (child != NULL)
			{
				liwdg_widget_set_allocation (child,
					LIWDG_WIDGET (self)->allocation.x + self->cols[x].start,
					LIWDG_WIDGET (self)->allocation.y + self->rows[y].start,
					self->cols[x].allocation,
					self->rows[y].allocation);
			}
		}
	}

	/* Update the size request. */
	private_rebuild_request (self);
}

static void
private_rebuild_request (liwdgGroup* self)
{
	int x;
	int y;
	int wreq;
	int hreq;

	/* Calculate the width request. */
	wreq = self->margin_right + self->margin_left;
	for (x = 0 ; x < self->width ; x++)
	{
		if (self->cols[x].request > 0)
		{
			if (x != self->width - 1)
				wreq += self->col_spacing;
			wreq += self->cols[x].request;
		}
	}

	/* Calculate the height request. */
	hreq = self->margin_top + self->margin_bottom;
	for (y = 0 ; y < self->height ; y++)
	{
		if (self->rows[y].request > 0)
		{
			if (y != self->height - 1)
				hreq += self->row_spacing;
			hreq += self->rows[y].request;
		}
	}

	/* Set the size request. */
	liwdg_widget_set_request (LIWDG_WIDGET (self), wreq, hreq);
}

/** @} */
/** @} */
