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
 * \addtogroup lialgOctree Octree
 * @{
 */

#ifndef __ALGORITHM_OCTREE_H__
#define __ALGORITHM_OCTREE_H__

#include <archive/lips-archive.h>
#include <math/lips-math.h>
#include <string/lips-string.h>

typedef struct _lialgOcnode lialgOcnode;
typedef struct _lialgOctree lialgOctree;
typedef int (*lialgOctreeRead) (void*     data,
                                void**    value,
                                limatVector* offset,
                                liReader* reader);
typedef int (*lialgOctreeWrite)(void*     data,
                                void*     value,
                                limatVector* offset,
                                liarcWriter* writer);

struct _lialgOcnode
{
	lialgOcnode* children[2][2][2];
};

struct _lialgOctree
{
	int depth;
	lialgOcnode* root;
};

lialgOctree*
lialg_octree_new (int depth);

lialgOctree*
lialg_octree_new_from_file (const char*     path,
                            lialgOctreeRead callback,
                            void*           data);
lialgOctree*
lialg_octree_new_from_data (liReader*       reader,
                            lialgOctreeRead callback,
                            void*           data);
void
lialg_octree_free (lialgOctree* self);

int
lialg_octree_write (lialgOctree*     self,
                    liarcWriter*     writer,
                    lialgOctreeWrite callback,
                    void*            data);

void*
lialg_octree_get_data (lialgOctree*    self,
                       const limatVector* point);

int
lialg_octree_set_data (lialgOctree*    self,
                       const limatVector* point,
                       void*           data);

int
lialg_octree_get_size (const lialgOctree* self);

#endif

/** @} */
/** @} */
