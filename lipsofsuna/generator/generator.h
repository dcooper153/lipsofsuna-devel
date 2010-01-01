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

#include <algorithm/lips-algorithm.h>
#include <archive/lips-archive.h>
#include <paths/lips-paths.h>
#include <system/lips-system.h>
#include <voxel/lips-voxel.h>
#include "generator-brush.h"
#include "generator-types.h"

typedef struct _ligenStroke ligenStroke;
struct _ligenStroke
{
	int pos[3];
	int size[3];
	int brush;
};

struct _ligenGenerator
{
	int fill;
	lialgSectors* sectors;
	lialgU32dic* brushes;
	liarcSql* gensql;
	liarcSql* srvsql;
	licalCallbacks* callbacks;
	lipthPaths* paths;
	livoxManager* voxels;
	struct
	{
		int count;
		ligenStroke* array;
	} strokes;
};

ligenGenerator*
ligen_generator_new (lipthPaths*     paths,
                     licalCallbacks* callbacks,
                     lialgSectors*   sectors);

void
ligen_generator_free (ligenGenerator* self);

void
ligen_generator_clear_scene (ligenGenerator* self);

ligenBrush*
ligen_generator_find_brush (ligenGenerator* self,
                            int             id);

int
ligen_generator_insert_brush (ligenGenerator* self,
                              ligenBrush*     brush);

int
ligen_generator_insert_stroke (ligenGenerator* self,
                               int             brush,
                               int             x,
                               int             y,
                               int             z);

int
ligen_generator_load_materials (ligenGenerator* self);

int
ligen_generator_main (ligenGenerator* self);

int
ligen_generator_rebuild_scene (ligenGenerator* self);

void
ligen_generator_remove_brush (ligenGenerator* self,
                              int             id);

int
ligen_generator_step (ligenGenerator* self);

int
ligen_generator_write (ligenGenerator* self);

int
ligen_generator_write_brushes (ligenGenerator* self);

void
ligen_generator_set_fill (ligenGenerator* self,
                          int             fill);

#endif

/** @} */
/** @} */
