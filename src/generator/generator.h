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
 * \addtogroup ligen Generator
 * @{
 * \addtogroup ligenGenerator Generator
 * @{
 */

#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <archive/lips-archive.h>
#include <paths/lips-paths.h>
#include <system/lips-system.h>
#include <voxel/lips-voxel.h>
#include "generator-brush.h"

typedef struct _ligenStroke ligenStroke;
struct _ligenStroke
{
	int pos[3];
	int size[3];
	int brush;
};

typedef struct _ligenGenerator ligenGenerator;
struct _ligenGenerator
{
	liarcSql* gensql;
	liarcSql* srvsql;
	lipthPaths* paths;
	liphyPhysics* physics;
	livoxManager* voxels;
	struct
	{
		int count;
		ligenBrush** array;
	} brushes;
	struct
	{
		int count;
		ligenStroke* array;
	} world;
	/* TODO */
};

ligenGenerator*
ligen_generator_new (const char* name);

void
ligen_generator_free (ligenGenerator* self);

int
ligen_generator_main (ligenGenerator* self);

int
ligen_generator_step (ligenGenerator* self);

#endif

/** @} */
/** @} */
