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
 * \addtogroup LIWdgTree Tree
 * @{
 */

#ifndef __WIDGET_TREE_H__
#define __WIDGET_TREE_H__

#include <lipsofsuna/system.h>
#include "widget-manager.h"

#define LIWDG_TREE(o) ((LIWdgTree*)(o))

typedef struct _LIWdgTree LIWdgTree;
typedef struct _LIWdgTreerow LIWdgTreerow;

LIAPICALL (const LIWdgClass*, liwdg_widget_tree, ());

LIAPICALL (LIWdgWidget*, liwdg_tree_new, (
	LIWdgManager* manager));

LIAPICALL (void, liwdg_tree_clear, (
	LIWdgTree* self));

LIAPICALL (void, liwdg_tree_foreach, (
	LIWdgTree* self,
	void     (*call)()));

LIAPICALL (LIWdgTreerow*, liwdg_tree_get_active, (
	LIWdgTree* self));

LIAPICALL (LIWdgTreerow*, liwdg_tree_get_root, (
	LIWdgTree* self));

LIAPICALL (LIWdgTreerow*, liwdg_treerow_append_row, (
	LIWdgTreerow* self,
	const char*   text,
	void*         data));

LIAPICALL (void, liwdg_treerow_remove_row, (
	LIWdgTreerow* self,
	int           index));

LIAPICALL (void*, liwdg_treerow_get_data, (
	LIWdgTreerow* self));

LIAPICALL (void, liwdg_treerow_set_data, (
	LIWdgTreerow* self,
	void*         value));

LIAPICALL (int, liwdg_treerow_get_expanded, (
	LIWdgTreerow* self));

LIAPICALL (void, liwdg_treerow_set_expanded, (
	LIWdgTreerow* self,
	int           value));

LIAPICALL (int, liwdg_treerow_get_highlighted, (
	LIWdgTreerow* self));

LIAPICALL (void, liwdg_treerow_set_highlighted, (
	LIWdgTreerow* self,
	int           value));

LIAPICALL (int, liwdg_treerow_get_index, (
	LIWdgTreerow* self));

LIAPICALL (LIWdgTreerow*, liwdg_treerow_get_parent, (
	LIWdgTreerow* self));

LIAPICALL (LIWdgTreerow*, liwdg_treerow_get_row, (
	LIWdgTreerow* self,
	int           index));

LIAPICALL (int, liwdg_treerow_get_row_count, (
	LIWdgTreerow* self));

LIAPICALL (const char*, liwdg_treerow_get_text, (
	LIWdgTreerow* self));

LIAPICALL (int, liwdg_treerow_set_text, (
	LIWdgTreerow* self,
	const char*   value));

#endif

/** @} */
/** @} */
