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
 * \addtogroup lialgMemdic Memdic
 * @{
 */

#ifndef __ALGORITHM_MEMDIC_H__
#define __ALGORITHM_MEMDIC_H__

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "algorithm-bst.h"

typedef struct _lialgMemdic lialgMemdic;
typedef struct _lialgMemdicNode lialgMemdicNode;
typedef struct _lialgMemdicIter lialgMemdicIter;

struct _lialgMemdic
{
	int size;
	lialgBst* tree;
	lialgMemdicNode* list;
};

struct _lialgMemdicNode
{
	int keysize;
	void* key;
	void* value;
	lialgBstNode node;
	lialgMemdicNode* prev;
	lialgMemdicNode* next;
};

struct _lialgMemdicIter
{
	lialgMemdic* assoc;
	int keysize;
	void* key;
	void* value;
	lialgMemdicNode* node;
	lialgMemdicNode* next;
};

/*****************************************************************************/

static inline void
lialg_memdic_node_free (lialgMemdicNode* self)
{
	lisys_free (self->key);
	lisys_free (self);
}

static inline int
lialg_memdic_node_compare (const lialgMemdicNode* self,
                           const lialgMemdicNode* node)
{
	int ret;
	int min;

	if (self->keysize < node->keysize)
		min = self->keysize;
	else
		min = node->keysize;
	ret = memcmp (self->key, node->key, min);
	if (ret) return ret;
	if (self->keysize < node->keysize) return -1;
	if (self->keysize > node->keysize) return 1;
	return 0;
}

/*****************************************************************************/

/**
 * \brief Creates a new associative array.
 *
 * \return New associative array or NULL.
 */
static inline lialgMemdic*
lialg_memdic_new ()
{
	lialgMemdic* self;

	self = (lialgMemdic*) lisys_malloc (sizeof (lialgMemdic));
	if (self == NULL)
		return NULL;
	self->size = 0;
	self->list = NULL;
	self->tree = lialg_bst_new ((lialgBstCompare) lialg_memdic_node_compare, lisys_malloc_func, lisys_free_func);
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
lialg_memdic_free (lialgMemdic* self)
{
	lialg_bst_foreach (self->tree, (lialgBstForeach) lialg_memdic_node_free);
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
lialg_memdic_clear (lialgMemdic* self)
{
	lialg_bst_foreach (self->tree, (lialgBstForeach) lialg_memdic_node_free);
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
 * \param keysize Size of the key.
 * \return Value or NULL.
 */
static inline void*
lialg_memdic_find (lialgMemdic* self,
                   const void*  key,
                   int          keysize)
{
	lialgMemdicNode tmp;
	lialgMemdicNode* anode;
	lialgBstNode* tnode;

	tmp.key = (void*) key;
	tmp.keysize = keysize;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	anode = (lialgMemdicNode*) tnode->data;
	assert (&anode->node == tnode);
	return anode->value;
}

/**
 * \brief Finds a node from the associative array.
 *
 * \param self Associative array.
 * \param key Key of the node.
 * \param keysize Size of the key.
 * \return Associative array node or NULL.
 */
static inline lialgMemdicNode*
lialg_memdic_find_node (lialgMemdic* self,
                        const void*  key,
                        int          keysize)
{
	lialgMemdicNode tmp;
	lialgBstNode* tnode;

	tmp.key = (void*) key;
	tmp.keysize = keysize;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return NULL;
	assert (&((lialgMemdicNode*) tnode->data)->node == tnode);
	return (lialgMemdicNode*) tnode->data;
}

/**
 * \brief Inserts data to the associative array.
 *
 * \param self Associative array.
 * \param key Key of the inserted node.
 * \param keysize Size of the key.
 * \param value Value of the inserted node.
 * \return Associative array node or NULL.
 */
static inline lialgMemdicNode*
lialg_memdic_insert (lialgMemdic* self,
                     const void*  key,
                     int          keysize,
                     void*        value)
{
	lialgMemdicNode* node;

	/* Create node. */
	node = (lialgMemdicNode*) lisys_malloc (sizeof (lialgMemdicNode));
	if (node == NULL)
		return NULL;
	node->key = lisys_malloc (keysize? keysize : 1);
	node->keysize = keysize;
	if (node->key == NULL)
	{
		lisys_free (node);
		return NULL;
	}
	memcpy (node->key, key, keysize);
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
 * \param keysize Size of the key.
 * \return Nonzero if a node was removed.
 */
static inline int
lialg_memdic_remove (lialgMemdic* self,
                     const void*  key,
                     int          keysize)
{
	lialgBstNode* tnode;
	lialgMemdicNode* anode;
	lialgMemdicNode tmp;
	
	/* Find node. */
	tmp.key = (void*) key;
	tmp.keysize = keysize;
	tnode = lialg_bst_find (self->tree, &tmp);
	if (tnode == NULL)
		return 1;
	anode = (lialgMemdicNode*) tnode->data;
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
	lialg_memdic_node_free (anode);
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
lialg_memdic_remove_node (lialgMemdic*     self,
                          lialgMemdicNode* node)
{
	if (node->prev != NULL)
		node->prev->next = node->next;
	else
		self->list = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	lialg_bst_unlink (self->tree, &node->node);
	lialg_memdic_node_free (node);
	self->size--;
}

/*****************************************************************************/

#define LI_FOREACH_MEMDIC(iter, assoc) \
	for (lialg_memdic_iter_start (&iter, assoc) ; \
	     iter.node != NULL ; \
	     lialg_memdic_iter_next (&iter))

static inline void
lialg_memdic_iter_start (lialgMemdicIter* self,
                         lialgMemdic*     assoc)
{
	self->assoc = assoc;
	if (assoc->list == NULL)
	{
		self->node = NULL;
		self->next = NULL;
		self->key = NULL;
		self->keysize = 0;
		self->value = NULL;
	}
	else
	{
		self->node = assoc->list;
		self->next = self->node->next;
		self->key = self->node->key;
		self->keysize = self->node->keysize;
		self->value = self->node->value;
	}
}

static inline int
lialg_memdic_iter_next (lialgMemdicIter* self)
{
	if (self->next == NULL)
	{
		self->node = NULL;
		self->next = NULL;
		self->key = NULL;
		self->keysize = 0;
		self->value = NULL;
		return 0;
	}
	else
	{
		self->node = self->next;
		self->next = self->node->next;
		self->key = self->node->key;
		self->keysize = self->node->keysize;
		self->value = self->node->value;
		return 1;
	}
}

#endif

/** @} */
/** @} */
