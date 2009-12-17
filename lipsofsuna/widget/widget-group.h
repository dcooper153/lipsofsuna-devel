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

#ifndef __WIDGET_GROUP_H__
#define __WIDGET_GROUP_H__

#include <algorithm/lips-algorithm.h>
#include "widget.h"
#include "widget-container.h"

#define LIWDG_GROUP(o) ((liwdgGroup*)(o))

typedef struct _liwdgGroupRow liwdgGroupRow;
struct _liwdgGroupRow
{
	int start;
	int expand;
	int request;
	int allocation;
};

typedef struct _liwdgGroupCol liwdgGroupCol;
struct _liwdgGroupCol
{
	int start;
	int expand;
	int request;
	int allocation;
};

typedef struct _liwdgGroupCell liwdgGroupCell;
struct _liwdgGroupCell
{
	int width;
	int height;
	liwdgWidget* child;
};

typedef struct _liwdgGroup liwdgGroup;
struct _liwdgGroup
{
	liwdgContainer base;
	int width;
	int height;
	int homogeneous;
	int col_expand;
	int row_expand;
	int col_spacing;
	int row_spacing;
	int margin_left;
	int margin_right;
	int margin_top;
	int margin_bottom;
	int rebuilding;
	liwdgGroupRow* rows;
	liwdgGroupCol* cols;
	liwdgGroupCell* cells;
};

extern const liwdgClass liwdgGroupType;

liwdgWidget*
liwdg_group_new (liwdgManager* manager);

liwdgWidget*
liwdg_group_new_with_size (liwdgManager* manager,
                           int           cols,
                           int           rows);

int
liwdg_group_append_col (liwdgGroup* self);

int
liwdg_group_append_row (liwdgGroup* self);

int
liwdg_group_insert_col (liwdgGroup* self,
                        int         index);

int
liwdg_group_insert_row (liwdgGroup* self,
                        int         index);

void
liwdg_group_remove_col (liwdgGroup* self,
                        int         index);

void
liwdg_group_remove_row (liwdgGroup* self,
                        int         index);

void
liwdg_group_get_cell_rect (liwdgGroup* self,
                           int         x,
                           int         y,
                           liwdgRect*  rect);

liwdgWidget*
liwdg_group_get_child (liwdgGroup* self,
                       int         x,
                       int         y);

void
liwdg_group_set_child (liwdgGroup*  self,
                       int          x,
                       int          y,
                       liwdgWidget* child);

int
liwdg_group_get_col_expand (liwdgGroup* self,
                            int         x);

void
liwdg_group_set_col_expand (liwdgGroup* self,
                            int         x,
                            int         expand); 

int
liwdg_group_get_col_size (liwdgGroup* self,
                          int         x);

int
liwdg_group_get_homogeneous (const liwdgGroup* self);

void
liwdg_group_set_homogeneous (liwdgGroup* self,
                             int         value);

void
liwdg_group_get_margins (liwdgGroup* self,
                         int*        left,
                         int*        right,
                         int*        top,
                         int*        bottom);

void
liwdg_group_set_margins (liwdgGroup* self,
                         int         left,
                         int         right,
                         int         top,
                         int         bottom);

int
liwdg_group_get_row_expand (liwdgGroup* self,
                            int         y);

void
liwdg_group_set_row_expand (liwdgGroup* self,
                            int         y,
                            int         expand);

int
liwdg_group_get_row_size (liwdgGroup* self,
                          int         y);

void
liwdg_group_get_size (liwdgGroup* self,
                      int*        cols,
                      int*        rows);

int
liwdg_group_set_size (liwdgGroup* self,
                      int         cols,
                      int         rows);

void
liwdg_group_get_spacings (liwdgGroup* self,
                          int*        column,
                          int*        row);

void
liwdg_group_set_spacings (liwdgGroup* self,
                          int         column,
                          int         row);

#endif

/** @} */
/** @} */
