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
 * \addtogroup LIWdgGroup Group
 * @{
 */

#include <assert.h>
#include <lipsofsuna/math.h>
#include "widget-group.h"
#include "widget-window.h"

#define LIWDG_GROUP_DEFAULT_SPACING 5

enum
{
	PRIVATE_REBUILD_CHILDREN = 0x01,
	PRIVATE_REBUILD_HORZ = 0x02,
	PRIVATE_REBUILD_REQUEST = 0x04,
	PRIVATE_REBUILD_VERT = 0x08
};

static int
private_init (LIWdgGroup*   self,
              LIWdgManager* manager);

static void
private_free (LIWdgGroup* self);

static int
private_event (LIWdgGroup* self,
               LIWdgEvent* event);

static LIWdgWidget*
private_child_at (LIWdgGroup* self,
                  int         pixx,
                  int         pixy);

static void
private_child_request (LIWdgGroup*  self,
                       LIWdgWidget* child);

static LIWdgWidget*
private_cycle_focus (LIWdgGroup*  self,
                     LIWdgWidget* curr,
                     int          next);

static void
private_detach_child (LIWdgGroup*  self,
                      LIWdgWidget* child);

static void
private_foreach_child (LIWdgGroup* self,
                       void      (*call)(),
                       void*       data);

static void
private_call_detach (LIWdgGroup* self,
                     int         x,
                     int         y);

static void
private_cell_changed (LIWdgGroup* self,
                      int         x,
                      int         y);

static int
private_get_col_size (LIWdgGroup* self,
                      int         x);

static int
private_get_row_size (LIWdgGroup* self,
                      int         y);

static void
private_rebuild (LIWdgGroup* self,
                 int         flags);

/****************************************************************************/

const LIWdgClass*
liwdg_widget_group ()
{
	static const LIWdgClass clss =
	{
		liwdg_widget_container, "Group", sizeof (LIWdgGroup),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	return &clss;
}

/**
 * \brief Creates a new group widget.
 *
 * \param manager Widget manager.
 * \return New group widget or NULL.
 */
LIWdgWidget*
liwdg_group_new (LIWdgManager* manager)
{
	LIWdgWidget* self;

	self = liwdg_widget_new (manager, liwdg_widget_group ());
	if (self == NULL)
		return NULL;
	private_rebuild (LIWDG_GROUP (self), PRIVATE_REBUILD_REQUEST);

	return self;
}

/**
 * \brief Creates a new group widget and sets its size.
 *
 * \param manager Widget manager.
 * \param cols Number of columns.
 * \param rows Number of rows.
 * \return New group widget or NULL.
 */
LIWdgWidget*
liwdg_group_new_with_size (LIWdgManager* manager,
                           int           cols,
                           int           rows)
{
	LIWdgWidget* self;

	self = liwdg_widget_new (manager, liwdg_widget_group ());
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
 * \brief Appends an empty column to the group.
 *
 * \param self Group.
 * \return Nonzero on success.
 */
int
liwdg_group_append_col (LIWdgGroup* self)
{
	return liwdg_group_set_size (self, self->width + 1, self->height);
}

/**
 * \brief Appends an empty row to the group.
 *
 * \param self Group.
 * \return Nonzero on success.
 */
int
liwdg_group_append_row (LIWdgGroup* self)
{
	return liwdg_group_set_size (self, self->width, self->height + 1);
}

/**
 * \brief Inserts an empty column to the group.
 *
 * \param self Group.
 * \param index Column index.
 * \return Nonzero on success.
 */
int
liwdg_group_insert_col (LIWdgGroup* self,
                        int         index)
{
	int x;
	int y;

	assert (index >= 0);
	assert (index <= self->width);

	/* Resize. */
	if (!liwdg_group_set_size (self, self->width + 1, self->height))
		return 0;

	/* Shift columns. */
	for (x = self->width - 1 ; x > index ; x--)
	{
		self->cols[x] = self->cols[x - 1];
		for (y = 0 ; y < self->height ; y++)
			self->cells[x + y * self->width] = self->cells[(x - 1) + y * self->width];
	}

	/* Clear new column. */
	memset (self->cols + index, 0, sizeof (LIWdgGroupCol));
	for (y = 0 ; y < self->height ; y++)
		self->cells[index + x * self->width].child = NULL;

	/* Rebuild columns. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_CHILDREN);

	return 1;
}

/**
 * \brief Inserts an empty row to the group.
 *
 * \param self Group.
 * \param index Row index.
 * \return Nonzero on success.
 */
int
liwdg_group_insert_row (LIWdgGroup* self,
                        int         index)
{
	int x;
	int y;

	assert (index >= 0);
	assert (index <= self->height);

	/* Resize. */
	if (!liwdg_group_set_size (self, self->width, self->height + 1))
		return 0;

	/* Shift rows. */
	for (y = self->height - 1 ; y > index ; y--)
	{
		self->rows[y] = self->rows[y - 1];
		for (x = 0 ; x < self->width ; x++)
			self->cells[x + y * self->width] = self->cells[x + (y - 1) * self->width];
	}

	/* Clear new row. */
	memset (self->rows + index, 0, sizeof (LIWdgGroupRow));
	for (x = 0 ; x < self->width ; x++)
		self->cells[x + index * self->width].child = NULL;

	/* Rebuild rows. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);

	return 1;
}

/**
 * \brief Removes a column from the group.
 *
 * \param self Group.
 * \param index Column index.
 */
void
liwdg_group_remove_col (LIWdgGroup* self,
                        int         index)
{
	int x;
	int y;

	assert (index >= 0);
	assert (index < self->width);

	/* Delete widgets. */
	for (y = 0 ; y < self->height ; y++)
		private_call_detach (self, index, y);

	/* Shift columns. */
	for (x = index ; x < self->width - 1 ; x++)
	{
		self->cols[x] = self->cols[x + 1];
		for (y = 0 ; y < self->height ; y++)
			self->cells[x + y * self->width] = self->cells[(x + 1) + y * self->width];
	}

	/* Clear last column. */
	for (y = 0 ; y < self->height ; y++)
		self->cells[(self->width - 1) + y * self->width].child = NULL;

	/* Resize. */
	liwdg_group_set_size (self, self->width - 1, self->height);

	/* Rebuild all. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
}

/**
 * \brief Removes a row from the group.
 *
 * \param self Group.
 * \param index Row index.
 */
void
liwdg_group_remove_row (LIWdgGroup* self,
                        int         index)
{
	int x;
	int y;

	assert (index >= 0);
	assert (index < self->height);

	/* Delete widgets. */
	for (x = 0 ; x < self->width ; x++)
		private_call_detach (self, x, index);

	/* Shift rows. */
	for (y = index ; y < self->height - 1 ; y++)
	{
		self->rows[y] = self->rows[y + 1];
		for (x = 0 ; x < self->width ; x++)
			self->cells[x + y * self->width] = self->cells[x + (y + 1) * self->width];
	}

	/* Clear last row. */
	for (x = 0 ; x < self->width ; x++)
		self->cells[x + (self->height - 1) * self->width].child = NULL;

	/* Resize. */
	liwdg_group_set_size (self, self->width, self->height - 1);

	/* Rebuild all. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
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
liwdg_group_get_cell_rect (LIWdgGroup* self,
                           int         x,
                           int         y,
                           LIWdgRect*  rect)
{
	liwdg_widget_get_content (LIWDG_WIDGET (self), rect);
	rect->x += self->cols[x].start;
	rect->y += self->rows[y].start;
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
LIWdgWidget*
liwdg_group_get_child (LIWdgGroup* self,
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
liwdg_group_set_child (LIWdgGroup*  self,
                       int          x,
                       int          y,
                       LIWdgWidget* child)
{
	LIWdgGroupCell* cell;
	LIWdgManager* manager;

	assert (x < self->width);
	assert (y < self->height);

	/* Check for same widget. */
	cell = self->cells + x + y * self->width;
	if (cell->child == child)
		return;

	/* Detach the old child. */
	if (cell->child != NULL)
	{
		assert (cell->child->state == LIWDG_WIDGET_STATE_DETACHED);
		assert (cell->child->parent == LIWDG_WIDGET (self));
		private_call_detach (self, x, y);
	}

	/* Attach the new child. */
	cell->child = child;
	if (child != NULL)
	{
		assert (child->parent == NULL);
		assert (child->state == LIWDG_WIDGET_STATE_DETACHED);
		manager = LIWDG_WIDGET (self)->manager;
		child->parent = LIWDG_WIDGET (self);
		lical_callbacks_call (manager->callbacks, manager, "widget-attach", lical_marshal_DATA_PTR_PTR, child, self);
	}

	/* Update the size of the cell. */
	private_cell_changed (self, x, y);
}

/**
 * \brief Gets the column expand status of a column.
 *
 * \param self Group.
 * \param x Column number.
 * \return Nonzero if the column is set to expand.
 */
int
liwdg_group_get_col_expand (LIWdgGroup* self,
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
liwdg_group_set_col_expand (LIWdgGroup* self,
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
		private_rebuild (self, PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_CHILDREN);
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
liwdg_group_get_col_size (LIWdgGroup* self,
                          int         x)
{
	return self->cols[x].allocation;
}

/**
 * \brief Get the homogeneousness flag.
 *
 * \param self Group.
 * \return value Nonzero if homogeneous.
 */
int
liwdg_group_get_homogeneous (const LIWdgGroup* self)
{
	return self->homogeneous;
}

/**
 * \brief Set the homogeneousness flag.
 *
 * \param self Group.
 * \param value Nonzero if homogeneous.
 */
void
liwdg_group_set_homogeneous (LIWdgGroup* self,
                             int         value)
{
	self->homogeneous = value;
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
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
liwdg_group_get_margins (LIWdgGroup* self,
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
liwdg_group_set_margins (LIWdgGroup* self,
                         int         left,
                         int         right,
                         int         top,
                         int         bottom)
{
	/* Set margins. */
	self->margin_left = left;
	self->margin_right = right;
	self->margin_top = top;
	self->margin_bottom = bottom;

	/* Rebuild the layout. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
}

/**
 * \brief Gets the row expand status of a row.
 *
 * \param self Group.
 * \param y Row number.
 * \return Nonzero if the row is set to expand.
 */
int
liwdg_group_get_row_expand (LIWdgGroup* self,
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
liwdg_group_set_row_expand (LIWdgGroup* self,
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
		private_rebuild (self, PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
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
liwdg_group_get_row_size (LIWdgGroup* self,
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
liwdg_group_get_size (LIWdgGroup* self,
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
liwdg_group_set_size (LIWdgGroup* self,
                      int         width,
                      int         height)
{
	int x;
	int y;
	LIWdgGroupCol* mem0 = NULL;
	LIWdgGroupRow* mem1 = NULL;
	LIWdgGroupCell* mem2 = NULL;

	/* Allocate memory. */
	if (width > 0)
	{
		mem0 = (LIWdgGroupCol*) lisys_calloc (width, sizeof (LIWdgGroupCol));
		if (mem0 == NULL)
			return 0;
	}
	if (height > 0)
	{
		mem1 = (LIWdgGroupRow*) lisys_calloc (height, sizeof (LIWdgGroupRow));
		if (mem1 == NULL)
		{
			lisys_free (mem0);
			return 0;
		}
	}
	if (width > 0 && height > 0)
	{
		mem2 = (LIWdgGroupCell*) lisys_calloc (width * height, sizeof (LIWdgGroupCell));
		if (mem2 == NULL)
		{
			lisys_free (mem0);
			lisys_free (mem1);
			return 0;
		}
	}

	/* Free widgets that don't fit. */
	for (y = height ; y < self->height ; y++)
	{
		for (x = width ; x < self->width ; x++)
			private_call_detach (self, x, y);
	}

	/* Copy over the column data. */
	if (self->width < width)
		memcpy (mem0, self->cols, self->width * sizeof (LIWdgGroupCol));
	else
		memcpy (mem0, self->cols, width * sizeof (LIWdgGroupCol));
	lisys_free (self->cols);
	self->cols = mem0;

	/* Copy over the row data. */
	if (self->height < height)
		memcpy (mem1, self->rows, self->height * sizeof (LIWdgGroupRow));
	else
		memcpy (mem1, self->rows, height * sizeof (LIWdgGroupRow));
	lisys_free (self->rows);
	self->rows = mem1;

	/* Copy over the cell data. */
	for (y = 0 ; y < height && y < self->height ; y++)
	{
		for (x = 0 ; x < width && x < self->width ; x++)
		{
			mem2[x + y * width].child = self->cells[x + y * self->width].child;
		}
	}
	lisys_free (self->cells);
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

	/* Update the size request. */
	private_rebuild (self, PRIVATE_REBUILD_REQUEST);
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
liwdg_group_get_spacings (LIWdgGroup* self,
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
liwdg_group_set_spacings (LIWdgGroup* self,
                          int         column,
                          int         row)
{
	if (self->col_spacing != column &&
	    self->row_spacing != row)
	{
		self->col_spacing = column;
		self->row_spacing = row;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
	}
	else if (self->col_spacing != column)
	{
		self->col_spacing = column;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_CHILDREN);
	}
	else if (self->row_spacing != row)
	{
		self->row_spacing = row;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
	}
}

/****************************************************************************/

static int
private_init (LIWdgGroup*   self,
              LIWdgManager* manager)
{
	self->cols = NULL;
	self->rows = NULL;
	self->cells = NULL;
	self->width = 0;
	self->height = 0;
	self->col_expand = 0;
	self->row_expand = 0;
	self->col_spacing = LIWDG_GROUP_DEFAULT_SPACING;
	self->row_spacing = LIWDG_GROUP_DEFAULT_SPACING;
	self->margin_left = 0;
	self->margin_right = 0;
	self->margin_top = 0;
	self->margin_bottom = 0;
	liwdg_widget_set_style (LIWDG_WIDGET (self), "group");

	return 1;
}

static void
private_free (LIWdgGroup* self)
{
	int x;
	int y;

	/* Free children. */
	for (y = 0 ; y < self->height ; y++)
	for (x = 0 ; x < self->width ; x++)
		private_call_detach (self, x, y);

	lisys_free (self->cols);
	lisys_free (self->rows);
	lisys_free (self->cells);
}

static int
private_event (LIWdgGroup* self,
               LIWdgEvent* event)
{
	int i;
	LIWdgWidget* child;

	/* Container interface. */
	if (event->type == LIWDG_EVENT_TYPE_PROBE &&
	    event->probe.clss == liwdg_widget_container ())
	{
		static LIWdgContainerIface iface =
		{
			(LIWdgContainerChildAtFunc) private_child_at,
			(LIWdgContainerChildRequestFunc) private_child_request,
			(LIWdgContainerCycleFocusFunc) private_cycle_focus,
			(LIWdgContainerDetachChildFunc) private_detach_child,
			(LIWdgContainerForeachChildFunc) private_foreach_child,
			(LIWdgContainerTranslateCoordsFunc) NULL
		};
		event->probe.result = &iface;
		return 0;
	}

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_ALLOCATION:
			private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			for (i = 0 ; i < self->width * self->height ; i++)
			{
				child = self->cells[i].child;
				if (child != NULL)
					liwdg_widget_draw (child);
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

	return liwdg_widget_container ()->event (LIWDG_WIDGET (self), event);
}

static LIWdgWidget*
private_child_at (LIWdgGroup* self,
                  int         pixx,
                  int         pixy)
{
	int x;
	int y;
	LIWdgRect rect;

	liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
	pixx -= rect.x;
	pixy -= rect.y;

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

static void
private_child_request (LIWdgGroup*  self,
                       LIWdgWidget* child)
{
	int x;
	int y;
	LIWdgSize size;
	LIWdgGroupCell* cell;

	for (y = 0 ; y < self->height ; y++)
	{
		for (x = 0 ; x < self->width ; x++)
		{
			cell = self->cells + x + y * self->width;
			if (cell->child == child)
			{
				size.width = private_get_col_size (self, x);
				size.height = private_get_row_size (self, y);
				if (self->cols[x].request != size.width ||
				    self->rows[y].request != size.height)
					private_cell_changed (self, x, y);
				return;
			}
		}
	}
	assert (0 && "Invalid child request");
}

/**
 * \brief Finds the next or previous focusable widget under the group.
 *
 * Searches nested containers recursively.
 *
 * \param group Group.
 * \param next Nonzero for next, zero for previous.
 * \return Widget or NULL.
 */
static LIWdgWidget*
private_cycle_focus (LIWdgGroup*  self,
                     LIWdgWidget* curr,
                     int          next)
{
	int x = 0;
	int y = 0;
	int found;
	LIWdgWidget* tmp;
	LIWdgWidget* child;

	/* Find old focused widget. */
	found = 0;
	if (curr != NULL)
	{
		for (y = self->height - 1 ; y >= 0 ; y--)
		{
			for (x = self->width - 1 ; x >= 0 ; x--)
			{
				child = self->cells[x + y * self->width].child;
				if (child == curr)
				{
					found = 1;
					break;
				}
			}
			if (found)
				break;
		}
		assert (found);
	}

	/* Find new focused widget. */
	if (next)
	{
		/* Set start position. */
		if (!found)
		{
			x = 0;
			y = 0;
		}

		/* Iterate forward. */
		for ( ; y < self->height ; y++, x = 0)
		for ( ; x < self->width ; x++)
		{
			if (!found)
			{
				child = self->cells[x + y * self->width].child;
				if (child == NULL)
					continue;
				if (liwdg_widget_typeis (child, liwdg_widget_container ()))
				{
					tmp = liwdg_container_cycle_focus (LIWDG_CONTAINER (child), NULL, 1);
					if (tmp != NULL)
						return tmp;
				}
				else
				{
					if (liwdg_widget_get_focusable (child))
						return child;
				}
			}
			else
				found = 0;
		}
	}
	else
	{
		/* Set start position. */
		if (!found)
		{
			x = self->width - 1;
			y = self->height - 1;
		}

		/* Iterate backward. */
		for ( ; y >= 0 ; y--, x = self->width - 1)
		for ( ; x >= 0 ; x--)
		{
			if (!found)
			{
				child = self->cells[x + y * self->width].child;
				if (child == NULL)
					continue;
				if (liwdg_widget_typeis (child, liwdg_widget_container ()))
				{
					tmp = liwdg_container_cycle_focus (LIWDG_CONTAINER (child), NULL, 0);
					if (tmp != NULL)
						return tmp;
				}
				else
				{
					if (liwdg_widget_get_focusable (child))
						return child;
				}
			}
			else
				found = 0;
		}
	}

	return NULL;
}

static void
private_detach_child (LIWdgGroup*  self,
                      LIWdgWidget* child)
{
	int x;
	int y;
	LIWdgGroupCell* cell;

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

static void
private_foreach_child (LIWdgGroup* self,
                       void      (*call)(),
                       void*       data)
{
	int x;
	int y;
	LIWdgGroupCell* cell;

	for (y = 0 ; y < self->height ; y++)
	{
		for (x = 0 ; x < self->width ; x++)
		{
			cell = self->cells + x + y * self->width;
			if (cell->child != NULL)
				call (data, cell->child);
		}
	}
}

static void
private_call_detach (LIWdgGroup* self,
                     int         x,
                     int         y)
{
	int free = 1;
	LIWdgManager* manager;
	LIWdgWidget* child;

	manager = LIWDG_WIDGET (self)->manager;
	child = self->cells[x + y * self->width].child;
	if (child != NULL)
	{
		lical_callbacks_call (manager->callbacks, manager, "widget-detach", lical_marshal_DATA_PTR_PTR, child, &free);
		if (free)
			liwdg_widget_free (child);
		else
			child->parent = NULL;
		self->cells[x + y * self->width].child = NULL;
	}
}

static void
private_cell_changed (LIWdgGroup* self,
                      int         x,
                      int         y)
{
	LIWdgSize size;
	LIWdgWidget* child;

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
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
	}
	else if (size.width != self->cols[x].request)
	{
		/* Only vertical layout changed. */
		self->cols[x].request = size.width;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_CHILDREN);
	}
	else if (size.height != self->rows[y].request)
	{
		/* Only horizontal layout changed. */
		self->rows[y].request = size.height;
		private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
	}
	else if (child != NULL && child->visible)
	{
		/* Only set allocation of the widget. */
		assert (self->cols[x].allocation >= size.width);
		assert (self->rows[y].allocation >= size.height);
		liwdg_widget_set_allocation (child,
			LIWDG_WIDGET (self)->allocation.x + self->cols[x].start,
			LIWDG_WIDGET (self)->allocation.y + self->rows[y].start,
			self->cols[x].allocation,
			self->rows[y].allocation);
	}

	private_rebuild (self, PRIVATE_REBUILD_REQUEST | PRIVATE_REBUILD_HORZ | PRIVATE_REBUILD_VERT | PRIVATE_REBUILD_CHILDREN);
}

static int
private_get_col_size (LIWdgGroup* self,
                      int         x)
{
	int y;
	int width;
	LIWdgSize size;
	LIWdgWidget* child;

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
private_get_row_size (LIWdgGroup* self,
                      int         y)
{
	int x;
	int height;
	LIWdgSize size;
	LIWdgWidget* child;

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
private_rebuild (LIWdgGroup* self,
                 int         flags)
{
	int x;
	int y;
	int wmax;
	int hmax;
	int wpad;
	int hpad;
	int wreq;
	int hreq;
	int start;
	int expand;
	LIWdgWidget* child;
	LIWdgRect rect;
	LIWdgSize size;

	if (self->rebuilding)
		return;
	self->rebuilding = 1;

	if (self->homogeneous)
	{
		if (1)
		{
			/* Calculate the width request. */
			wmax = 0;
			wpad = self->margin_right + self->margin_left;
			for (x = 0 ; x < self->width ; x++)
			{
				if (self->cols[x].request > 0)
				{
					if (x != self->width - 1)
						wpad += self->col_spacing;
					if (wmax < self->cols[x].request)
						wmax = self->cols[x].request;
				}
			}
			wreq = wpad + wmax * self->width;

			/* Calculate the height request. */
			hmax = 0;
			hpad = self->margin_top + self->margin_bottom;
			for (y = 0 ; y < self->height ; y++)
			{
				if (self->rows[y].request > 0)
				{
					if (y != self->height - 1)
						hpad += self->row_spacing;
					if (hmax < self->rows[y].request)
						hmax = self->rows[y].request;
				}
			}
			hreq = hpad + hmax * self->height;

			/* Set the size request. */
			liwdg_widget_set_request_internal (LIWDG_WIDGET (self), wreq, hreq);
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
			liwdg_widget_get_request (LIWDG_WIDGET (self), &size);
			rect.width = LIMAT_MAX (size.width, rect.width);
			rect.height = LIMAT_MAX (size.height, rect.height);
			liwdg_widget_set_allocation (LIWDG_WIDGET (self), rect.x, rect.y, rect.width, rect.height);
		}

		liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);

		if (flags & PRIVATE_REBUILD_HORZ)
		{
			/* Get horizontal expansion. */
			if (self->col_expand > 0)
			{
				expand = rect.width - wreq;
				assert (expand >= 0);
				expand /= self->width;
			}
			else
				expand = 0;

			/* Set horizontal allocations. */
			start = self->margin_left;
			for (x = 0 ; x < self->width ; x++)
			{
				self->cols[x].start = start;
				self->cols[x].allocation = wmax;
				self->cols[x].allocation += expand;
				start += self->cols[x].allocation;
				start += self->col_spacing;
			}
		}

		if (flags & PRIVATE_REBUILD_VERT)
		{
			/* Get vertical expansion. */
			if (self->row_expand > 0)
			{
				expand = rect.height - hreq;
				assert (expand >= 0);
				expand /= self->height;
			}
			else
				expand = 0;

			/* Set vertical allocations. */
			start = self->margin_top;
			for (y = 0 ; y < self->height ; y++)
			{
				self->rows[y].start = start;
				self->rows[y].allocation = hmax;
				self->rows[y].allocation += expand;
				start += self->rows[y].allocation;
				start += self->row_spacing;
			}
		}
	}
	else
	{
		if (flags & PRIVATE_REBUILD_REQUEST)
		{
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
			liwdg_widget_set_request_internal (LIWDG_WIDGET (self), wreq, hreq);
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
			liwdg_widget_get_request (LIWDG_WIDGET (self), &size);
			rect.width = LIMAT_MAX (size.width, rect.width);
			rect.height = LIMAT_MAX (size.height, rect.height);
			liwdg_widget_set_allocation (LIWDG_WIDGET (self), rect.x, rect.y, rect.width, rect.height);
		}

		liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);

		if (flags & PRIVATE_REBUILD_HORZ)
		{
			/* Get horizontal expansion. */
			if (self->col_expand > 0)
			{
				expand = rect.width - self->margin_left - self->margin_right;
				assert (expand >= 0);
				for (x = 0 ; x < self->width ; x++)
				{
					if (self->cols[x].request)
					{
						expand -= self->cols[x].request;
						if (x < self->width - 1)
							expand -= self->col_spacing;
					}
				}
				assert (expand >= 0);
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
				start += self->cols[x].allocation;
				if (self->cols[x].request)
					start += self->col_spacing;
			}
		}

		if (flags & PRIVATE_REBUILD_VERT)
		{
			/* Get vertical expansion. */
			if (self->row_expand > 0)
			{
				expand = rect.height - self->margin_top - self->margin_bottom;
				assert (expand >= 0);
				for (y = 0 ; y < self->height ; y++)
				{
					if (self->rows[y].request)
					{
						expand -= self->rows[y].request;
						if (y < self->height - 1)
							expand -= self->row_spacing;
					}
				}
				assert (expand >= 0);
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
				start += self->rows[y].allocation;
				if (self->rows[y].request)
					start += self->row_spacing;
			}
		}
	}

	if (flags & PRIVATE_REBUILD_CHILDREN)
	{
		liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);

		/* Set positions of widgets. */
		for (x = 0 ; x < self->width ; x++)
		{
			for (y = 0 ; y < self->height ; y++)
			{
				child = self->cells[x + y * self->width].child;
				if (child != NULL)
				{
					liwdg_widget_set_allocation (child,
						rect.x + self->cols[x].start,
						rect.y + self->rows[y].start,
						self->cols[x].allocation,
						self->rows[y].allocation);
				}
			}
		}
	}

	self->rebuilding = 0;
}

/** @} */
/** @} */
