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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengBlockBuilder BlockBuilder
 * @{
 */

#ifndef __ENGINE_BLOCK_BUILDER_H__
#define __ENGINE_BLOCK_BUILDER_H__

#include <algorithm/lips-algorithm.h>
#include <math/lips-math.h>
#include "engine-sector.h"

typedef struct _liengBlockBuilderNormal liengBlockBuilderNormal;
struct _liengBlockBuilderNormal
{
	int count;
	limatVector vertex;
	limatVector normals;
};

typedef struct _liengBlockBuilder liengBlockBuilder;
struct _liengBlockBuilder
{
	liengSector* sector;
	struct
	{
		lialgMemdic* normals;
		limdlModel* model;
		liphyShape* shape;
	} helpers;
	struct
	{
		int count;
		limatVector* array;
	} vertices;
};

liengBlockBuilder*
lieng_block_builder_new (liengSector* sector);

void
lieng_block_builder_free (liengBlockBuilder* self);

int
lieng_block_builder_build (liengBlockBuilder* self,
                           int                bx,
                           int                by,
                           int                bz);

limdlModel*
lieng_block_builder_get_model (liengBlockBuilder* self);

liphyShape*
lieng_block_builder_get_shape (liengBlockBuilder* self);

#endif

/** @} */
/** @} */
