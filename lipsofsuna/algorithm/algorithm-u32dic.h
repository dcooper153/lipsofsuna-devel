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
 * \addtogroup lialg Algorithm
 * @{
 * \addtogroup LIAlgU32dic U32dic
 * @{
 */

#ifndef __ALGORITHM_U32DIC_H__
#define __ALGORITHM_U32DIC_H__

#include <lipsofsuna/system.h>
#include "algorithm-bst.h"

typedef struct _LIAlgU32dic LIAlgU32dic;
typedef struct _LIAlgU32dicNode LIAlgU32dicNode;
typedef struct _LIAlgU32dicIter LIAlgU32dicIter;

struct _LIAlgU32dic
{
	int size;
	LIAlgBst* tree;
	LIAlgU32dicNode* list;
};

struct _LIAlgU32dicNode
{
	uint32_t key;
	void* value;
	LIAlgBstNode node;
	LIAlgU32dicNode* prev;
	LIAlgU32dicNode* next;
};

struct _LIAlgU32dicIter
{
	LIAlgU32dic* assoc;
	uint32_t key;
	void* value;
	LIAlgU32dicNode* node;
	LIAlgU32dicNode* next;
};

/*****************************************************************************/

static inline int
lialg_u32dic_node_compare (const LIAlgU32dicNode* self,
                           const LIAlgU32dicNode* node)
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
static inline LIAlgU32dic*
lialg_u32dic_new ()
{
	LIAlgU32dic* self;

	self = (LIAlgU32dic*) lisys_malloc (sizeof (LIAlgU32dic));
	if (self == NULL)
		return NULL;
	self->size = 0;
	self->list = NULL;
	self->tree = lialg_bst_new ((LIAlgBstCompare) lialg_u32dic_node_compare, lisys_malloc_func, lisys_free_func);
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
lialg_u32dic_free (LIAlgU32dic* self)
{
	lialg_bst_foreach (self->tree, (LIAlgBstForeach) lisys_free_func);
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
lialg_u32dic_clear (LIAlgU32dic* self)
{
	lialg_bst_foreach (self->tree, (LIAlgBstForeach) lisys_free_func);
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
lialg_u32dic_find (LIAlgU32dic* self,
                   uint32_t     key)
{
	LIAlgU32dicNode tmp;
	LIAlgU32dicNode* anode;
	LIAlgBstNode* tnode;

	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	anode = (LIAlgU32dicNode*) tnode->data;
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
static inline LIAlgU32dicNode*
lialg_u32dic_find_node (LIAlgU32dic* self,
                        uint32_t     key)
{
	LIAlgU32dicNode tmp;
	LIAlgBstNode* tnode;

	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	assert (&((LIAlgU32dicNode*) tnode->data)->node == tnode);
	return (LIAlgU32dicNode*) tnode->data;
}

/**
 * \brief Inserts data to the associative array.
 *
 * \param self Associative array.
 * \param key Key of the inserted node.
 * \param value Value of the inserted node.
 * \return Associative array node or NULL.
 */
static inline LIAlgU32dicNode*
lialg_u32dic_insert (LIAlgU32dic* self,
                     uint32_t     key,
                     void*        value)
{
	LIAlgU32dicNode* node;

	/* Create node. */
	node = (LIAlgU32dicNode*) lisys_malloc (sizeof (LIAlgU32dicNode));
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
lialg_u32dic_remove (LIAlgU32dic* self,
                     uint32_t     key)
{
	LIAlgBstNode* tnode;
	LIAlgU32dicNode* anode;
	LIAlgU32dicNode tmp;
	
	/* Find node. */
	tmp.key = key;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return 1;
	anode = (LIAlgU32dicNode*) tnode->data;
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
lialg_u32dic_remove_node (LIAlgU32dic*     self,
                          LIAlgU32dicNode* node)
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
lialg_u32dic_unique_key (const LIAlgU32dic* self)
{
	uint32_t key;

	for (key = 0 ; !key ; )
	{
		key = lisys_randi (0x7FFFFFFF);
		if (lialg_u32dic_find ((LIAlgU32dic*) self, key) != NULL)
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
lialg_u32dic_iter_start (LIAlgU32dicIter* self,
                         LIAlgU32dic*     assoc)
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
lialg_u32dic_iter_next (LIAlgU32dicIter* self)
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
