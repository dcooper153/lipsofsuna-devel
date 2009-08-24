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
 * \addtogroup lialg Algorithm
 * @{
 * \addtogroup lialgPtrdic Ptrdic
 * @{
 */

#ifndef __ALGORITHM_PTRDIC_H__
#define __ALGORITHM_PTRDIC_H__

#include <assert.h>
#include <stdlib.h>
#include "algorithm-bst.h"

typedef struct _lialgPtrdic lialgPtrdic;
typedef struct _lialgPtrdicNode lialgPtrdicNode;
typedef struct _lialgPtrdicIter lialgPtrdicIter;

struct _lialgPtrdic
{
	int size;
	lialgBst* tree;
	lialgPtrdicNode* list;
};

struct _lialgPtrdicNode
{
	void* key;
	void* value;
	lialgBstNode node;
	lialgPtrdicNode* prev;
	lialgPtrdicNode* next;
};

struct _lialgPtrdicIter
{
	lialgPtrdic* assoc;
	void* key;
	void* value;
	lialgPtrdicNode* node;
	lialgPtrdicNode* next;
};

/*****************************************************************************/

static inline int
lialg_ptrdic_node_compare (const lialgPtrdicNode* self,
                              const lialgPtrdicNode* node)
{
	if (self->key < node->key) return -1;
	if (self->key > node->key) return 1;
	return 0;
}

/*****************************************************************************/

/**
 * \brief Creates a new associative array.
 *
 * \return New associative array or NULL.
 */
static inline lialgPtrdic*
lialg_ptrdic_new ()
{
	lialgPtrdic* self;

	self = (lialgPtrdic*) malloc (sizeof (lialgPtrdic));
	if (self == NULL)
		return NULL;
	self->size = 0;
	self->list = NULL;
	self->tree = lialg_bst_new ((lialgBstCompare) lialg_ptrdic_node_compare, malloc, free);
	if (self->tree == NULL)
	{
		free (self);
		return NULL;
	}
	return self;
}

/**
 * \brief Frees the associative array.
 *
 * \param self Associative array.
 */
static inline void
lialg_ptrdic_free (lialgPtrdic* self)
{
	lialg_bst_foreach (self->tree, (lialgBstForeach) free);
	self->tree->root = NULL;
	lialg_bst_free (self->tree);
	free (self);
}

/**
 * \brief Clears the associative array.
 *
 * \param self Associative array.
 */
static inline void
lialg_ptrdic_clear (lialgPtrdic* self)
{
	lialg_bst_foreach (self->tree, (lialgBstForeach) free);
	self->size = 0;
	self->list = NULL;
	self->tree->root = NULL;
	lialg_bst_clear (self->tree);
}

/**
 * \brief Finds a value from the associative array.
 *
 * \param self Associative array.
 * \param key Key of the node.
 * \return Value or NULL.
 */
static inline void*
lialg_ptrdic_find (lialgPtrdic* self,
                      void*          key)
{
	lialgPtrdicNode tmp;
	lialgPtrdicNode* anode;
	lialgBstNode* tnode;

	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	anode = (lialgPtrdicNode*) tnode->data;
	assert (&anode->node == tnode);
	return anode->value;
}

/**
 * \brief Finds a node from the associative array.
 *
 * \param self Associative array.
 * \param key Key of the node.
 * \return Associative array node or NULL.
 */
static inline lialgPtrdicNode*
lialg_ptrdic_find_node (lialgPtrdic* self,
                           void*          key)
{
	lialgPtrdicNode tmp;
	lialgBstNode* tnode;

	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	assert (&((lialgPtrdicNode*) tnode->data)->node == tnode);
	return (lialgPtrdicNode*) tnode->data;
}

/**
 * \brief Inserts data to the associative array.
 *
 * \param self Associative array.
 * \param key Key of the inserted node.
 * \param value Value of the inserted node.
 * \return Associative array node or NULL.
 */
static inline lialgPtrdicNode*
lialg_ptrdic_insert (lialgPtrdic* self,
                        void*          key,
                        void*          value)
{
	lialgPtrdicNode* node;

	/* Create node. */
	node = (lialgPtrdicNode*) malloc (sizeof (lialgPtrdicNode));
	if (node == NULL)
		return NULL;
	node->key = key;
	node->value = value;
	node->prev = NULL;
	node->next = self->list;

	/* Link to list. */
	node->node.data = node;
	if (self->list != NULL)
		self->list->prev = node;
	self->list = node;

	/* Link to tree. */
	lialg_bst_link (self->tree, &node->node);
	self->size++;
	return node;
}

/**
 * \brief Removes data from the associative array.
 *
 * \param self Associative array.
 * \param key Key of the removed node.
 * \return Nonzero if a node was removed.
 */
static inline int
lialg_ptrdic_remove (lialgPtrdic* self,
                        void*          key)
{
	lialgBstNode* tnode;
	lialgPtrdicNode* anode;
	lialgPtrdicNode tmp;
	
	/* Find node. */
	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return 1;
	anode = (lialgPtrdicNode*) tnode->data;
	assert (&anode->node == tnode);

	/* Unlink from tree. */
	lialg_bst_unlink (self->tree, tnode);

	/* Remove from list. */
	if (anode->prev != NULL)
		anode->prev->next = anode->next;
	else
		self->list = anode->next;
	if (anode->next != NULL)
		anode->next->prev = anode->prev;
	free (anode);
	self->size--;
	return 0;
}

/**
 * \brief Removes a node from the associative array.
 *
 * \param self Associative array.
 * \param node Node to remove.
 */
static inline void
lialg_ptrdic_remove_node (lialgPtrdic*     self,
                             lialgPtrdicNode* node)
{
	if (node->prev != NULL)
		node->prev->next = node->next;
	else
		self->list = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	lialg_bst_unlink (self->tree, &node->node);
	free (node);
	self->size--;
}

/*****************************************************************************/

#define LI_FOREACH_PTRDIC(iter, assoc) \
	for (lialg_ptrdic_iter_start (&iter, assoc) ; \
	     iter.node != NULL ; \
	     lialg_ptrdic_iter_next (&iter))

static inline void
lialg_ptrdic_iter_start (lialgPtrdicIter* self,
                            lialgPtrdic*     assoc)
{
	self->assoc = assoc;
	if (assoc->list == NULL)
	{
		self->node = NULL;
		self->next = NULL;
		self->key = 0;
		self->value = NULL;
	}
	else
	{
		self->node = assoc->list;
		self->next = self->node->next;
		self->key = self->node->key;
		self->value = self->node->value;
	}
}

static inline int
lialg_ptrdic_iter_next (lialgPtrdicIter* self)
{
	if (self->next == NULL)
	{
		self->node = NULL;
		self->next = NULL;
		self->key = 0;
		self->value = NULL;
		return 0;
	}
	else
	{
		self->node = self->next;
		self->next = self->node->next;
		self->key = self->node->key;
		self->value = self->node->value;
		return 1;
	}
}

#endif

/** @} */
/** @} */
