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
 * \addtogroup LIAlgOctree Octree
 * @{
 */

#ifndef __ALGORITHM_OCTREE_H__
#define __ALGORITHM_OCTREE_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/math.h>

typedef struct _LIAlgOcnode LIAlgOcnode;
typedef struct _LIAlgOctree LIAlgOctree;
typedef int (*LIAlgOctreeRead) (void*        data,
                                void**       value,
                                LIMatVector* offset,
                                LIArcReader* reader);
typedef int (*LIAlgOctreeWrite)(void*        data,
                                void*        value,
                                LIMatVector* offset,
                                LIArcWriter* writer);

struct _LIAlgOcnode
{
	LIAlgOcnode* children[2][2][2];
};

struct _LIAlgOctree
{
	int depth;
	LIAlgOcnode* root;
};

LIAlgOctree*
lialg_octree_new (int depth);

LIAlgOctree*
lialg_octree_new_from_file (const char*     path,
                            LIAlgOctreeRead callback,
                            void*           data);
LIAlgOctree*
lialg_octree_new_from_data (LIArcReader*    reader,
                            LIAlgOctreeRead callback,
                            void*           data);
void
lialg_octree_free (LIAlgOctree* self);

int
lialg_octree_write (LIAlgOctree*     self,
                    LIArcWriter*     writer,
                    LIAlgOctreeWrite callback,
                    void*            data);

void*
lialg_octree_get_data (LIAlgOctree*    self,
                       const LIMatVector* point);

int
lialg_octree_set_data (LIAlgOctree*    self,
                       const LIMatVector* point,
                       void*           data);

int
lialg_octree_get_size (const LIAlgOctree* self);

#endif

/** @} */
/** @} */
