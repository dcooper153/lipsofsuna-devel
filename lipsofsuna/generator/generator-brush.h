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
 * \addtogroup ligenBrush Brush
 * @{
 */

#ifndef __GENERATOR_BRUSH_H__
#define __GENERATOR_BRUSH_H__

#include <math/lips-math.h>
#include <voxel/lips-voxel.h>
#include "generator-rule.h"
#include "generator-types.h"

typedef struct _ligenBrushobject ligenBrushobject;
struct _ligenBrushobject
{
	int id;
	int flags;
	float probability;
	char* type;
	char* model;
	char* extra;
	limatTransform transform;
};

struct _ligenBrush
{
	int id;
	int size[3];
	char* name;
	ligenGenerator* generator;
	struct
	{
		int count;
		livoxVoxel* array;
	} voxels;
	struct
	{
		int count;
		ligenRule** array;
	} rules;
	struct
	{
		int count;
		ligenBrushobject** array;
	} objects;
};

ligenBrush*
ligen_brush_new (ligenGenerator* generator,
                 int             width,
                 int             height,
                 int             depth);

void
ligen_brush_free (ligenBrush* self);

int
ligen_brush_insert_object (ligenBrush*           self,
                           int                   flags,
                           float                 prob,
                           const char*           type,
                           const char*           model,
                           const char*           extra,
                           const limatTransform* transform);

int
ligen_brush_insert_rule (ligenBrush* self,
                         ligenRule*  rule);

int
ligen_brush_read_rules (ligenBrush* self,
                        liarcSql*   sql);

void
ligen_brush_remove_object (ligenBrush* self,
                           int         index);

void
ligen_brush_remove_rule (ligenBrush* self,
                         int         index);

void
ligen_brush_remove_strokes (ligenBrush* self,
                            int         brush);

int
ligen_brush_write (ligenBrush* self,
                   liarcSql*   sql);

int
ligen_brush_set_name (ligenBrush* self,
                      const char* value);

int
ligen_brush_set_size (ligenBrush* self,
                      int         x,
                      int         y,
                      int         z);

void
ligen_brush_set_voxel (ligenBrush* self,
                       int         x,
                       int         y,
                       int         z,
                       livoxVoxel  voxel);

#endif

/** @} */
/** @} */
