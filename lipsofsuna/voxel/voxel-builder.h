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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup livoxBuilder Builder
 * @{
 */

#ifndef __VOXEL_BUILDER_H__
#define __VOXEL_BUILDER_H__

#include <algorithm/lips-algorithm.h>
#include <math/lips-math.h>
#include "voxel-sector.h"

typedef struct _livoxBuilderNormal livoxBuilderNormal;
struct _livoxBuilderNormal
{
	limatVector normal;
	livoxBuilderNormal* next;
};

typedef struct _livoxBuilderVertex livoxBuilderVertex;
struct _livoxBuilderVertex
{
	float texcoord[2];
	float normal[3];
	float coord[3];
};

typedef struct _livoxFaces livoxFaces;
struct _livoxFaces
{
	int material;
	struct
	{
		int count;
		livoxBuilderVertex* array;
	} vertices;
};

typedef struct _livoxBuilder livoxBuilder;
struct _livoxBuilder
{
	limatAabb aabb;
	livoxSector* sector;
	struct
	{
		lialgU32dic* materials;
		lialgMemdic* normals;
		liphyShape* shape;
	} helpers;
	struct
	{
		int count;
		limatVector* array;
	} vertices;
};

livoxBuilder*
livox_builder_new (livoxSector* sector);

void
livox_builder_free (livoxBuilder* self);

int
livox_builder_build (livoxBuilder* self,
                     int           bx,
                     int           by,
                     int           bz);

lirndObject*
livox_builder_get_render (livoxBuilder* self);

liphyShape*
livox_builder_get_shape (livoxBuilder* self);

#endif

/** @} */
/** @} */
