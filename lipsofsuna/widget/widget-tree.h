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
 * \addtogroup liwdgTree Tree
 * @{
 */

#ifndef __WIDGET_TREE_H__
#define __WIDGET_TREE_H__

#include "widget-manager.h"

#define LIWDG_TREE(o) ((liwdgTree*)(o))

typedef struct _liwdgTree liwdgTree;
typedef struct _liwdgTreerow liwdgTreerow;

extern const liwdgClass liwdgTreeType;

liwdgWidget*
liwdg_tree_new (liwdgManager* manager);

void
liwdg_tree_clear (liwdgTree* self);

void
liwdg_tree_foreach (liwdgTree* self,
                    void     (*call)());

liwdgTreerow*
liwdg_tree_get_active (liwdgTree* self);

liwdgTreerow*
liwdg_tree_get_root (liwdgTree* self);

liwdgTreerow*
liwdg_treerow_append_row (liwdgTreerow* self,
                          const char*   text,
                          void*         data);

void
liwdg_treerow_remove_row (liwdgTreerow* self,
                          int           index);

void*
liwdg_treerow_get_data (liwdgTreerow* self);

void
liwdg_treerow_set_data (liwdgTreerow* self,
                        void*         value);

int
liwdg_treerow_get_expanded (liwdgTreerow* self);

void
liwdg_treerow_set_expanded (liwdgTreerow* self,
                            int           value);

int
liwdg_treerow_get_highlighted (liwdgTreerow* self);

void
liwdg_treerow_set_highlighted (liwdgTreerow* self,
                               int           value);

int
liwdg_treerow_get_index (liwdgTreerow* self);

liwdgTreerow*
liwdg_treerow_get_parent (liwdgTreerow* self);

liwdgTreerow*
liwdg_treerow_get_row (liwdgTreerow* self,
                       int           index);

int
liwdg_treerow_get_row_count (liwdgTreerow* self);

const char*
liwdg_treerow_get_text (liwdgTreerow* self);

int
liwdg_treerow_set_text (liwdgTreerow* self,
                        const char*   value);

#endif

/** @} */
/** @} */
