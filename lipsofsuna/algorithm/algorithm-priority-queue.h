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
 * \addtogroup lialgPriorityQueue Priority Queue
 * @{
 */

#ifndef __ALGORITHM_PRIORITY_QUEUE_H__
#define __ALGORITHM_PRIORITY_QUEUE_H__

#include <system/lips-system.h>
#include "algorithm-bst.h"

typedef struct _lialgPriorityQueue lialgPriorityQueue;
struct _lialgPriorityQueue
{
	lialgBst* tree;
};

typedef struct _lialgPriorityQueueNode lialgPriorityQueueNode;
struct _lialgPriorityQueueNode
{
	float priority;
	void* value;
	lialgBstNode node;
};

/*****************************************************************************/

static inline int
lialg_priority_queue_node_compare (const lialgPriorityQueueNode* self,
                                   const lialgPriorityQueueNode* node)
{
	if (self->priority < node->priority) return -1;
	if (self->priority > node->priority) return 1;
	return 0;
}

/*****************************************************************************/

/**
 * \brief Creates a new priority queue.
 *
 * \return New priority queue or NULL.
 */
static inline lialgPriorityQueue*
lialg_priority_queue_new ()
{
	lialgPriorityQueue* self;

	self = (lialgPriorityQueue*) lisys_malloc (sizeof (lialgPriorityQueue));
	if (self == NULL)
		return NULL;
	self->tree = lialg_bst_new ((lialgBstCompare) lialg_priority_queue_node_compare, lisys_malloc_func, lisys_free_func);
	if (self->tree == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	return self;
}

/**
 * \brief Frees the priority queue.
 *
 * \param self Priority queue.
 */
static inline void
lialg_priority_queue_free (lialgPriorityQueue* self)
{
	lialg_bst_foreach (self->tree, (lialgBstForeach) lisys_free_func);
	self->tree->root = NULL;
	lialg_bst_free (self->tree);
	lisys_free (self);
}

/**
 * \brief Clears the priority queue.
 *
 * \param self Priority queue.
 */
static inline void
lialg_priority_queue_clear (lialgPriorityQueue* self)
{
	lialg_bst_foreach (self->tree, (lialgBstForeach) lisys_free_func);
	self->tree->root = NULL;
	lialg_bst_clear (self->tree);
}

/**
 * \brief Finds the value with the highest priority from the queue.
 *
 * \param self Priority queue.
 * \return Value or NULL.
 */
static inline void*
lialg_priority_queue_find_highest (lialgPriorityQueue* self)
{
	lialgBstNode* tnode;
	lialgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->right != NULL)
		tnode = tnode->right;
	pnode = (lialgPriorityQueueNode*) tnode->data;
	assert (&pnode->node == tnode);
	return pnode->value;
}

/**
 * \brief Finds the node with the lowest priority from the queue.
 *
 * \param self Priority queue.
 * \return Priority queue node or NULL.
 */
static inline lialgPriorityQueueNode*
lialg_priority_queue_find_highest_node (lialgPriorityQueue* self)
{
	lialgBstNode* tnode;
	lialgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->left != NULL)
		tnode = tnode->left;
	pnode = (lialgPriorityQueueNode*) tnode->data;
	assert (&pnode->node == tnode);
	return pnode;
}

/**
 * \brief Finds the value with the lowest priority from the queue.
 *
 * \param self Priority queue.
 * \return Value or NULL.
 */
static inline void*
lialg_priority_queue_find_lowest (lialgPriorityQueue* self)
{
	lialgBstNode* tnode;
	lialgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->left != NULL)
		tnode = tnode->left;
	pnode = (lialgPriorityQueueNode*) tnode->data;
	assert (&pnode->node == tnode);
	return pnode->value;
}

/**
 * \brief Finds the node with the lowest priority from the queue.
 *
 * \param self Priority queue.
 * \return Priority queue node or NULL.
 */
static inline lialgPriorityQueueNode*
lialg_priority_queue_find_lowest_node (lialgPriorityQueue* self)
{
	lialgBstNode* tnode;
	lialgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->left != NULL)
		tnode = tnode->left;
	pnode = (lialgPriorityQueueNode*) tnode->data;
	assert (&pnode->node == tnode);
	return pnode;
}

/**
 * \brief Inserts data to the priority queue.
 *
 * \param self Priority queue.
 * \param priority Priority of the inserted node.
 * \param value Value of the inserted node.
 * \return Priority queue node or NULL.
 */
static inline lialgPriorityQueueNode*
lialg_priority_queue_insert (lialgPriorityQueue* self,
                             float               priority,
                             void*               value)
{
	lialgPriorityQueueNode* node;

	/* Create node. */
	node = (lialgPriorityQueueNode*) lisys_malloc (sizeof (lialgPriorityQueueNode));
	if (node == NULL)
		return NULL;
	node->priority = priority;
	node->value = value;

	/* Link to tree. */
	node->node.data = node;
	lialg_bst_link (self->tree, &node->node);
	return node;
}

/**
 * \brief Pops the value with the highest priority from the queue.
 *
 * \param self Priority queue.
 * \return Value or NULL.
 */
static inline void*
lialg_priority_queue_pop_highest (lialgPriorityQueue* self)
{
	void* value;
	lialgBstNode* tnode;
	lialgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->right != NULL)
		tnode = tnode->right;
	pnode = (lialgPriorityQueueNode*) tnode->data;
	value = pnode->value;
	assert (&pnode->node == tnode);
	lialg_bst_unlink (self->tree, tnode);
	lisys_free (pnode);
	return value;
}

/**
 * \brief Pops the value with the lowest priority from the queue.
 *
 * \param self Priority queue.
 * \return Value or NULL.
 */
static inline void*
lialg_priority_queue_pop_lowest (lialgPriorityQueue* self)
{
	void* value;
	lialgBstNode* tnode;
	lialgPriorityQueueNode* pnode;

	tnode = self->tree->root;
	if (tnode == NULL)
		return NULL;
	while (tnode->left != NULL)
		tnode = tnode->left;
	pnode = (lialgPriorityQueueNode*) tnode->data;
	value = pnode->value;
	assert (&pnode->node == tnode);
	lialg_bst_unlink (self->tree, tnode);
	lisys_free (pnode);
	return value;
}

/**
 * \brief Removes a node from the priority queue.
 *
 * \param self Priority queue.
 * \param node Node to remove.
 */
static inline void
lialg_priority_queue_remove_node (lialgPriorityQueue*     self,
                                  lialgPriorityQueueNode* node)
{
	lialg_bst_unlink (self->tree, &node->node);
	lisys_free (node);
}

#endif

/** @} */
/** @} */
