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
 * \addtogroup lialgStrdic Strdic
 * @{
 */

#ifndef __ALGORITHM_STRDIC_H__
#define __ALGORITHM_STRDIC_H__

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "algorithm-bst.h"

typedef struct _lialgStrdic lialgStrdic;
typedef struct _lialgStrdicNode lialgStrdicNode;
typedef struct _lialgStrdicIter lialgStrdicIter;

struct _lialgStrdic
{
	int size;
	lialgBst* tree;
	lialgStrdicNode* list;
};

struct _lialgStrdicNode
{
	char* key;
	void* value;
	lialgBstNode node;
	lialgStrdicNode* prev;
	lialgStrdicNode* next;
};

struct _lialgStrdicIter
{
	lialgStrdic* assoc;
	char* key;
	void* value;
	lialgStrdicNode* node;
	lialgStrdicNode* next;
};

/*****************************************************************************/

static inline void
lialg_strdic_node_free (lialgStrdicNode* self)
{
	lisys_free (self->key);
	lisys_free (self);
}

static inline int
lialg_strdic_node_compare (const lialgStrdicNode* self,
                           const lialgStrdicNode* node)
{
	return strcmp (self->key, node->key);
}

/*****************************************************************************/

/**
 * \brief Creates a new associative array.
 *
 * \return New associative array or NULL.
 */
static inline lialgStrdic*
lialg_strdic_new ()
{
	lialgStrdic* self;

	self = (lialgStrdic*) lisys_malloc (sizeof (lialgStrdic));
	if (self == NULL)
		return NULL;
	self->size = 0;
	self->list = NULL;
	self->tree = lialg_bst_new ((lialgBstCompare) lialg_strdic_node_compare, lisys_malloc_func, lisys_free_func);
	if (self->tree == NULL)
	{
		lisys_free (self);
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
lialg_strdic_free (lialgStrdic* self)
{
	lialg_bst_foreach (self->tree, (lialgBstForeach) lialg_strdic_node_free);
	self->tree->root = NULL;
	lialg_bst_free (self->tree);
	lisys_free (self);
}

/**
 * \brief Clears the associative array.
 *
 * \param self Associative array.
 */
static inline void
lialg_strdic_clear (lialgStrdic* self)
{
	lialg_bst_foreach (self->tree, (lialgBstForeach) lisys_free_func);
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
lialg_strdic_find (lialgStrdic* self,
                   const char*  key)
{
	lialgStrdicNode tmp;
	lialgStrdicNode* anode;
	lialgBstNode* tnode;

	tmp.key = (char*) key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	anode = (lialgStrdicNode*)tnode->data;
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
static inline lialgStrdicNode*
lialg_strdic_find_node (lialgStrdic* self,
                        const char*  key)
{
	lialgStrdicNode tmp;
	lialgBstNode* tnode;

	tmp.key = (char*) key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	assert (&((lialgStrdicNode*) tnode->data)->node == tnode);
	return (lialgStrdicNode*)tnode->data;
}

/**
 * \brief Inserts data to the associative array.
 *
 * \param self An associative array.
 * \param key Key of the inserted node.
 * \param value Value of the inserted node.
 * \return Associative array node or NULL.
 */
static inline lialgStrdicNode*
lialg_strdic_insert (lialgStrdic* self,
                     const char*  key,
                     void*        value)
{
	lialgStrdicNode* node;

	/* Create node. */
	node = (lialgStrdicNode*) lisys_malloc (sizeof (lialgStrdicNode));
	if (node == NULL)
		return NULL;
	node->key = listr_dup (key);
	if (node->key == NULL)
	{
		lisys_free (node);
		return NULL;
	}
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
lialg_strdic_remove (lialgStrdic* self,
                     const char*    key)
{
	lialgBstNode* tnode;
	lialgStrdicNode* anode;
	lialgStrdicNode tmp;
	
	/* Find node. */
	tmp.key = (char*) key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return 0;
	anode = (lialgStrdicNode*) tnode->data;
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
	lialg_strdic_node_free (anode);
	self->size--;
	return 1;
}

/**
 * \brief Removes a node from the associative array.
 *
 * \param self Associative array.
 * \param node Node to remove.
 */
static inline void
lialg_strdic_remove_node (lialgStrdic*     self,
                          lialgStrdicNode* node)
{
	if (node->prev != NULL)
		node->prev->next = node->next;
	else
		self->list = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	lialg_bst_unlink (self->tree, &node->node);
	lialg_strdic_node_free (node);
	self->size--;
}

/*****************************************************************************/

#define LI_FOREACH_STRDIC(iter, assoc) \
	for (lialg_strdic_iter_start (&iter, assoc) ; \
	     iter.node != NULL ; \
	     lialg_strdic_iter_next (&iter))

static inline void
lialg_strdic_iter_start (lialgStrdicIter* self,
                         lialgStrdic*     assoc)
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
lialg_strdic_iter_next (lialgStrdicIter* self)
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
