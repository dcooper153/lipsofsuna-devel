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

#ifndef __MODEL_BUILDER_H__
#define __MODEL_BUILDER_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "model.h"

typedef struct _LIMdlBuilder LIMdlBuilder;
struct _LIMdlBuilder
{
	int facegroup_capacity;
	int material_capacity;
	int weightgroup_capacity;
	int vertex_capacity;
	LIMdlModel* model;
};

LIAPICALL (LIMdlBuilder*, limdl_builder_new, (
	LIMdlModel* model));

LIAPICALL (void, limdl_builder_free, (
	LIMdlBuilder* self));

LIAPICALL (void, limdl_builder_finish, (
	LIMdlBuilder* self));

LIAPICALL (int, limdl_builder_insert_face, (
	LIMdlBuilder*      self,
	int                groupidx,
	const LIMdlVertex* vertices,
	const int*         bone_mapping));

LIAPICALL (int, limdl_builder_insert_facegroup, (
	LIMdlBuilder* self,
	int           material));

LIAPICALL (int, limdl_builder_insert_indices, (
	LIMdlBuilder*   self,
	int             groupidx,
	const uint32_t* indices,
	int             count));

LIAPICALL (int, limdl_builder_insert_material, (
	LIMdlBuilder*        self,
	const LIMdlMaterial* material));

LIAPICALL (int, limdl_builder_insert_node, (
	LIMdlBuilder*    self,
	const LIMdlNode* node));

LIAPICALL (int, limdl_builder_insert_vertices, (
	LIMdlBuilder*      self,
	const LIMdlVertex* vertices,
	int                count,
	const int*         bone_mapping));

LIAPICALL (int, limdl_builder_insert_weightgroup, (
	LIMdlBuilder* self,
	const char*   name,
	const char*   bone));

#endif
