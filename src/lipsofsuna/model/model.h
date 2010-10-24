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

#ifndef __MODEL_H__
#define __MODEL_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "model-animation.h"
#include "model-bone.h"
#include "model-faces.h"
#include "model-hair.h"
#include "model-light.h"
#include "model-material.h"
#include "model-particle.h"
#include "model-node.h"
#include "model-shape.h"
#include "model-types.h"
#include "model-vertex.h"

#define LIMDL_FORMAT_VERSION 0xFFFFFFF3

typedef int LIMdlModelFlags;

enum
{
	LIMDL_MATERIAL_FLAG_BILLBOARD    = 0x01,
	LIMDL_MATERIAL_FLAG_COLLISION    = 0x02,
	LIMDL_MATERIAL_FLAG_CULLFACE     = 0x04,
	LIMDL_MATERIAL_FLAG_TRANSPARENCY = 0x08,
};

typedef struct _LIMdlWeight LIMdlWeight;
struct _LIMdlWeight
{
	int group;
	float weight;
};

typedef struct _LIMdlWeights LIMdlWeights;
struct _LIMdlWeights
{
	int count;
	LIMdlWeight* weights;
};

struct _LIMdlWeightGroup
{
	char* name;
	char* bone;
	LIMdlNode* node;
};

/*****************************************************************************/

struct _LIMdlModel
{
	int flags;
	LIMatAabb bounds;
	struct { int count; LIMdlAnimation* array; } animations;
	struct { int count; LIMdlHairs* array; } hairs;
	struct { int count; LIMdlFaces* array; } facegroups;
	struct { int count; LIMdlMaterial* array; } materials;
	struct { int count; LIMdlNode** array; } nodes;
	struct { int count; LIMdlParticleSystem* array; } particlesystems;
	struct { int count; LIMdlShape* array; } shapes;
	struct { int count; LIMdlVertex* array; int capacity; } vertices;
	struct { int count; LIMdlWeightGroup* array; } weightgroups;
	struct { int count; LIMdlWeights* array; int capacity; } weights;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (LIMdlModel*, limdl_model_new, ());

LIAPICALL (LIMdlModel*, limdl_model_new_copy, (
	LIMdlModel* model));

LIAPICALL (LIMdlModel*, limdl_model_new_from_data, (
	LIArcReader* reader));

LIAPICALL (LIMdlModel*, limdl_model_new_from_file, (
	const char* path));

LIAPICALL (void, limdl_model_calculate_bounds, (
	LIMdlModel* self));

LIAPICALL (void, limdl_model_free, (
	LIMdlModel* self));

LIAPICALL (LIMdlAnimation*, limdl_model_find_animation, (
	LIMdlModel* self,
	const char* name));

LIAPICALL (int, limdl_model_find_facegroup, (
	LIMdlModel* self,
	int         material));

LIAPICALL (int, limdl_model_find_material, (
	const LIMdlModel*    self,
	const LIMdlMaterial* material));

LIAPICALL (LIMdlNode*, limdl_model_find_node, (
	const LIMdlModel* self,
	const char*       name));

LIAPICALL (int, limdl_model_find_vertex, (
	LIMdlModel*        self,
	const LIMdlVertex* vertex));

LIAPICALL (int, limdl_model_find_vertex_weighted, (
	LIMdlModel*         self,
	const LIMdlVertex*  vertex,
	const LIMdlWeights* weights,
	const int*          mapping));

LIAPICALL (int, limdl_model_find_weightgroup, (
	LIMdlModel* self,
	const char* name,
	const char* bone));

LIAPICALL (int, limdl_model_insert_face, (
	LIMdlModel*         self,
	int                 group,
	const LIMdlVertex*  vertices,
	const LIMdlWeights* weights));

LIAPICALL (int, limdl_model_insert_facegroup, (
	LIMdlModel* self,
	int         material));

LIAPICALL (int, limdl_model_insert_indices, (
	LIMdlModel* self,
	int         group,
	uint32_t*   indices,
	int         count));

LIAPICALL (int, limdl_model_insert_material, (
	LIMdlModel*          self,
	const LIMdlMaterial* material));

LIAPICALL (int, limdl_model_insert_node, (
	LIMdlModel*      self,
	const LIMdlNode* node));

LIAPICALL (int, limdl_model_insert_vertex, (
	LIMdlModel*         self,
	const LIMdlVertex*  vertex));

LIAPICALL (int, limdl_model_insert_vertex_weighted, (
	LIMdlModel*         self,
	const LIMdlVertex*  vertex,
	const LIMdlWeights* weights,
	const int*          mapping));

LIAPICALL (int, limdl_model_insert_vertex_weights, (
	LIMdlModel*         self,
	const LIMdlWeights* weights,
	const int*          mapping));

LIAPICALL (int, limdl_model_insert_weightgroup, (
	LIMdlModel* self,
	const char* name,
	const char* bone));

LIAPICALL (int, limdl_model_merge, (
	LIMdlModel* self,
	LIMdlModel* model));

LIAPICALL (int, limdl_model_write, (
	const LIMdlModel* self,
	LIArcWriter*      writer));

LIAPICALL (int, limdl_model_write_file, (
	const LIMdlModel* self,
	const char*       path));

LIAPICALL (int, limdl_model_get_index_count, (
	const LIMdlModel* self));

#ifdef __cplusplus
}
#endif

#endif
