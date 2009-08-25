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
 * \addtogroup lialgU32dic U32dic
 * @{
 */

#ifndef __ALGORITHM_U32dic_H__
#define __ALGORITHM_U32dic_H__

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "algorithm-bst.h"

typedef struct _lialgU32dic lialgU32dic;
typedef struct _lialgU32dicNode lialgU32dicNode;
typedef struct _lialgU32dicIter lialgU32dicIter;

struct _lialgU32dic
{
	int size;
	lialgBst* tree;
	lialgU32dicNode* list;
};

struct _lialgU32dicNode
{
	uint32_t key;
	void* value;
	lialgBstNode node;
	lialgU32dicNode* prev;
	lialgU32dicNode* next;
};

struct _lialgU32dicIter
{
	lialgU32dic* assoc;
	uint32_t key;
	void* value;
	lialgU32dicNode* node;
	lialgU32dicNode* next;
};

/*****************************************************************************/

static inline int
lialg_u32dic_node_compare (const lialgU32dicNode* self,
                             const lialgU32dicNode* node)
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
static inline lialgU32dic*
lialg_u32dic_new ()
{
	lialgU32dic* self;

	self = (lialgU32dic*) lisys_malloc (sizeof (lialgU32dic));
	if (self == NULL)
		return NULL;
	self->size = 0;
	self->list = NULL;
	self->tree = lialg_bst_new ((lialgBstCompare) lialg_u32dic_node_compare, lisys_malloc_func, lisys_free_func);
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
lialg_u32dic_free (lialgU32dic* self)
{
	lialg_bst_foreach (self->tree, (lialgBstForeach) lisys_free_func);
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
lialg_u32dic_clear (lialgU32dic* self)
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
lialg_u32dic_find (lialgU32dic* self,
                   uint32_t     key)
{
	lialgU32dicNode tmp;
	lialgU32dicNode* anode;
	lialgBstNode* tnode;

	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	anode = (lialgU32dicNode*) tnode->data;
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
static inline lialgU32dicNode*
lialg_u32dic_find_node (lialgU32dic* self,
                        uint32_t     key)
{
	lialgU32dicNode tmp;
	lialgBstNode* tnode;

	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	assert (&((lialgU32dicNode*) tnode->data)->node == tnode);
	return (lialgU32dicNode*) tnode->data;
}

/**
 * \brief Inserts data to the associative array.
 *
 * \param self Associative array.
 * \param key Key of the inserted node.
 * \param value Value of the inserted node.
 * \return Associative array node or NULL.
 */
static inline lialgU32dicNode*
lialg_u32dic_insert (lialgU32dic* self,
                     uint32_t     key,
                     void*        value)
{
	lialgU32dicNode* node;

	/* Create node. */
	node = (lialgU32dicNode*) lisys_malloc (sizeof (lialgU32dicNode));
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
lialg_u32dic_remove (lialgU32dic* self,
                     uint32_t     key)
{
	lialgBstNode* tnode;
	lialgU32dicNode* anode;
	lialgU32dicNode tmp;
	
	/* Find node. */
	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return 1;
	anode = (lialgU32dicNode*) tnode->data;
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
	lisys_free (anode);
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
lialg_u32dic_remove_node (lialgU32dic*     self,
                          lialgU32dicNode* node)
{
	if (node->prev != NULL)
		node->prev->next = node->next;
	else
		self->list = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	lialg_bst_unlink (self->tree, &node->node);
	lisys_free (node);
	self->size--;
}

/**
 * \brief Returns a random unused key.
 *
 * \param self Associative array.
 * \return Unique key.
 */
static inline uint32_t
lialg_u32dic_unique_key (const lialgU32dic* self)
{
	uint32_t key;

	for (key = 0 ; !key ; )
	{
		key = lisys_randi (0x7FFFFFFF);
		if (lialg_u32dic_find ((lialgU32dic*) self, key) != NULL)
			key = 0;
	}

	return key;
}

/*****************************************************************************/

#define LI_FOREACH_U32DIC(iter, assoc) \
	for (lialg_u32dic_iter_start (&iter, assoc) ; \
	     iter.node != NULL ; \
	     lialg_u32dic_iter_next (&iter))

static inline void
lialg_u32dic_iter_start (lialgU32dicIter* self,
                         lialgU32dic*     assoc)
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
lialg_u32dic_iter_next (lialgU32dicIter* self)
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
