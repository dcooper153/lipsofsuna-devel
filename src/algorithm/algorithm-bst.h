/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup lialg Algorithm
 * @{
 * \addtogroup lialgBst Bst
 * @{
 */

#ifndef __ALGORITHM_BST_H__
#define __ALGORITHM_BST_H__

#include <stdlib.h>

typedef int (*lialgBstCompare)(const void*, const void*);
typedef void (*lialgBstForeach)(void* a);
typedef void (*lialgBstFree)(void*);
typedef void* (*lialgBstMalloc)(size_t);
typedef int (*lialgBstMatch)(const void*, const void*);

/**
 * \brief A binary search tree node.
 */
typedef struct _lialgBstNode lialgBstNode;
struct _lialgBstNode
{
	void* data;
	lialgBstNode* left;
	lialgBstNode* right;
	lialgBstNode* parent;
};

/**
 * \brief A self-balancing binary search tree.
 */
typedef struct _lialgBst lialgBst;
struct _lialgBst
{
	int size;
	int max_size;
	lialgBstNode* root;
	lialgBstCompare cmp;
	lialgBstFree free;
	lialgBstMalloc malloc;
};

lialgBst*
lialg_bst_new (lialgBstCompare cmp,
               lialgBstMalloc  malloc,
               lialgBstFree    free);

void
lialg_bst_free (lialgBst* self);

void
lialg_bst_clear (lialgBst* self);

lialgBstNode*
lialg_bst_find (lialgBst*   self,
                const void* data);

lialgBstNode*
lialg_bst_find_by_index (lialgBst* self,
                         int       index);

void
lialg_bst_foreach (lialgBst*       self,
                   lialgBstForeach func);

lialgBstNode*
lialg_bst_insert (lialgBst* self,
                  void*     data);

void
lialg_bst_link (lialgBst*     self,
                lialgBstNode* node);

lialgBstNode*
lialg_bst_match (lialgBst*     self,
                 lialgBstMatch func,
                 const void*   data);

void
lialg_bst_remove (lialgBst*     self,
                  lialgBstNode* node);

void
lialg_bst_unlink (lialgBst*     self,
                  lialgBstNode* node);

#endif

/** @} */
/** @} */
