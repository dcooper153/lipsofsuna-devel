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

#ifndef __WIDGET_GROUP_H__
#define __WIDGET_GROUP_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-container.h"

#define LIWDG_GROUP(o) ((LIWdgGroup*)(o))

typedef struct _LIWdgGroupRow LIWdgGroupRow;
struct _LIWdgGroupRow
{
	int start;
	int expand;
	int request;
	int allocation;
};

typedef struct _LIWdgGroupCol LIWdgGroupCol;
struct _LIWdgGroupCol
{
	int start;
	int expand;
	int request;
	int allocation;
};

typedef struct _LIWdgGroupCell LIWdgGroupCell;
struct _LIWdgGroupCell
{
	int width;
	int height;
	LIWdgWidget* child;
};

typedef struct _LIWdgGroup LIWdgGroup;
struct _LIWdgGroup
{
	LIWdgContainer base;
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
	LIWdgGroupRow* rows;
	LIWdgGroupCol* cols;
	LIWdgGroupCell* cells;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_group, ());

LIAPICALL (LIWdgWidget*, liwdg_group_new, (
	LIWdgManager* manager));

LIAPICALL (LIWdgWidget*, liwdg_group_new_with_size, (
	LIWdgManager* manager,
	int           cols,
	int           rows));

LIAPICALL (int, liwdg_group_append_col, (
	LIWdgGroup* self));

LIAPICALL (int, liwdg_group_append_row, (
	LIWdgGroup* self));

LIAPICALL (int, liwdg_group_insert_col, (
	LIWdgGroup* self,
	int         index));

LIAPICALL (int, liwdg_group_insert_row, (
	LIWdgGroup* self,
	int         index));

LIAPICALL (void, liwdg_group_remove_col, (
	LIWdgGroup* self,
	int         index));

LIAPICALL (void, liwdg_group_remove_row, (
	LIWdgGroup* self,
	int         index));

LIAPICALL (void, liwdg_group_get_cell_rect, (
	LIWdgGroup* self,
	int         x,
	int         y,
	LIWdgRect*  rect));

LIAPICALL (LIWdgWidget*, liwdg_group_get_child, (
	LIWdgGroup* self,
	int         x,
	int         y));

LIAPICALL (void, liwdg_group_set_child, (
	LIWdgGroup*  self,
	int          x,
	int          y,
	LIWdgWidget* child));

LIAPICALL (int, liwdg_group_get_col_expand, (
	LIWdgGroup* self,
	int         x));

LIAPICALL (void, liwdg_group_set_col_expand, (
	LIWdgGroup* self,
	int         x,
	int         expand));

LIAPICALL (int, liwdg_group_get_col_size, (
	LIWdgGroup* self,
	int         x));

LIAPICALL (int, liwdg_group_get_homogeneous, (
	const LIWdgGroup* self));

LIAPICALL (void, liwdg_group_set_homogeneous, (
	LIWdgGroup* self,
	int         value));

LIAPICALL (void, liwdg_group_get_margins, (
	LIWdgGroup* self,
	int*        left,
	int*        right,
	int*        top,
	int*        bottom));

LIAPICALL (void, liwdg_group_set_margins, (
	LIWdgGroup* self,
	int         left,
	int         right,
	int         top,
	int         bottom));

LIAPICALL (int, liwdg_group_get_row_expand, (
	LIWdgGroup* self,
	int         y));

LIAPICALL (void, liwdg_group_set_row_expand, (
	LIWdgGroup* self,
	int         y,
	int         expand));

LIAPICALL (int, liwdg_group_get_row_size, (
	LIWdgGroup* self,
	int         y));

LIAPICALL (void, liwdg_group_get_size, (
	LIWdgGroup* self,
	int*        cols,
	int*        rows));

LIAPICALL (int, liwdg_group_set_size, (
	LIWdgGroup* self,
	int         cols,
	int         rows));

LIAPICALL (void, liwdg_group_get_spacings, (
	LIWdgGroup* self,
	int*        column,
	int*        row));

LIAPICALL (void, liwdg_group_set_spacings, (
	LIWdgGroup* self,
	int         column,
	int         row));

#endif

/** @} */
/** @} */
