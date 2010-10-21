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
 * \addtogroup LIGenBrush Brush
 * @{
 */

#ifndef __GENERATOR_BRUSH_H__
#define __GENERATOR_BRUSH_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/voxel.h>
#include "generator-rule.h"
#include "generator-types.h"

typedef struct _LIGenBrushobject LIGenBrushobject;
struct _LIGenBrushobject
{
	int id;
	int flags;
	float probability;
	char* type;
	char* model;
	char* extra;
	LIMatTransform transform;
};

struct _LIGenBrush
{
	int id;
	int size[3];
	int disabled;
	char* name;
	LIGenGenerator* generator;
	struct
	{
		int count;
		LIGenRule** array;
	} rules;
};

LIAPICALL (LIGenBrush*, ligen_brush_new, (
	LIGenGenerator* generator,
	int             width,
	int             height,
	int             depth));

LIAPICALL (void, ligen_brush_free, (
	LIGenBrush* self));

LIAPICALL (int, ligen_brush_insert_rule, (
	LIGenBrush* self,
	LIGenRule*  rule));

LIAPICALL (void, ligen_brush_remove_rule, (
	LIGenBrush* self,
	int         index));

LIAPICALL (int, ligen_brush_set_name, (
	LIGenBrush* self,
	const char* value));

LIAPICALL (int, ligen_brush_set_size, (
	LIGenBrush* self,
	int         x,
	int         y,
	int         z));

#endif

/** @} */
/** @} */
