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
 * \addtogroup ligen Generator
 * @{
 * \addtogroup LIGenGenerator Generator
 * @{
 */

#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/archive.h>
#include <lipsofsuna/paths.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/voxel.h>
#include "generator-brush.h"
#include "generator-types.h"

typedef struct _LIGenStroke LIGenStroke;
struct _LIGenStroke
{
	int pos[3];
	int size[3];
	int brush;
};

struct _LIGenGenerator
{
	int fill;
	LIAlgSectors* sectors;
	LIAlgU32dic* brushes;
	LIArcSql* sql;
	LICalCallbacks* callbacks;
	LIPthPaths* paths;
	LIVoxManager* voxels;
	struct
	{
		int count;
		LIGenStroke* array;
	} strokes;
};

LIGenGenerator*
ligen_generator_new (LIPthPaths*     paths,
                     LICalCallbacks* callbacks,
                     LIAlgSectors*   sectors);

void
ligen_generator_free (LIGenGenerator* self);

void
ligen_generator_clear_scene (LIGenGenerator* self);

LIGenBrush*
ligen_generator_find_brush (LIGenGenerator* self,
                            int             id);

int
ligen_generator_insert_brush (LIGenGenerator* self,
                              LIGenBrush*     brush);

int
ligen_generator_insert_stroke (LIGenGenerator* self,
                               int             brush,
                               int             x,
                               int             y,
                               int             z);

int
ligen_generator_load_materials (LIGenGenerator* self);

int
ligen_generator_main (LIGenGenerator* self);

int
ligen_generator_rebuild_scene (LIGenGenerator* self);

void
ligen_generator_remove_brush (LIGenGenerator* self,
                              int             id);

int
ligen_generator_step (LIGenGenerator* self);

int
ligen_generator_write (LIGenGenerator* self);

int
ligen_generator_write_brushes (LIGenGenerator* self);

int
ligen_generator_write_materials (LIGenGenerator* self);

void
ligen_generator_set_fill (LIGenGenerator* self,
                          int             fill);

#endif

/** @} */
/** @} */
