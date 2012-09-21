/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#ifndef __EXT_TERRAIN_MODULE_H__
#define __EXT_TERRAIN_MODULE_H__

#include "lipsofsuna/extension.h"
#include "lipsofsuna/model.h"

#define LIEXT_SCRIPT_TERRAIN "Terrain"

#define LIEXT_STICK_EPSILON 0.01f
#define LIEXT_TERRAIN_SMOOTHING_LIMIT 0.1f

typedef struct _LIExtTerrainModule LIExtTerrainModule;

/*****************************************************************************/

typedef struct _LIExtTerrainVertex LIExtTerrainVertex;
struct _LIExtTerrainVertex
{
	float offset;
	float splatting;
	LIMatVector normal;
};

/*****************************************************************************/

typedef struct _LIExtTerrainStick LIExtTerrainStick;
struct _LIExtTerrainStick
{
	LIExtTerrainStick* next;
	int material;
	float height;
	LIExtTerrainVertex vertices[2][2];
};

LIAPICALL (LIExtTerrainStick*, liext_terrain_stick_new, (
	int   material,
	float height));

LIAPICALL (void, liext_terrain_stick_free, (
	LIExtTerrainStick* self));

LIAPICALL (void, liext_terrain_stick_clamp_vertices, (
	LIExtTerrainStick* self,
	LIExtTerrainStick* next));

LIAPICALL (void, liext_terrain_stick_clamp_vertices_bottom, (
	LIExtTerrainStick* self,
	LIExtTerrainStick* prev));

LIAPICALL (void, liext_terrain_stick_copy_vertices, (
	LIExtTerrainStick* self,
	LIExtTerrainStick* src));

LIAPICALL (void, liext_terrain_stick_fix_vertices_downwards, (
	LIExtTerrainStick* self,
	LIExtTerrainStick* stick_first));

LIAPICALL (void, liext_terrain_stick_fix_vertices_upwards, (
	LIExtTerrainStick* self));

LIAPICALL (void, liext_terrain_stick_move_vertices, (
	LIExtTerrainStick* self,
	float              diff));

LIAPICALL (void, liext_terrain_stick_reset_vertices, (
	LIExtTerrainStick* self));

LIAPICALL (int, liext_terrain_stick_subtract, (
	LIExtTerrainStick* self,
	float              y,
	float              h,
	float              bot00,
	float              bot10,
	float              bot01,
	float              bot11,
	float              top00,
	float              top10,
	float              top01,
	float              top11));

LIAPICALL (int, liext_terrain_stick_get_data, (
	LIExtTerrainStick* self,
	LIArcWriter*       writer));

LIAPICALL (int, liext_terrain_stick_set_data, (
	LIExtTerrainStick* self,
	LIArcReader*       reader));

LIAPICALL (int, liext_terrain_stick_get_intersection_type, (
	const LIExtTerrainStick* self,
	float                    y,
	float                    h));

LIAPICALL (void, liext_terrain_stick_get_normal, (
	const LIExtTerrainStick* self,
	LIMatVector*             result));

LIAPICALL (void, liext_terrain_stick_get_normal_override, (
	const LIExtTerrainStick* self,
	int                      vertex_x,
	int                      vertex_y,
	float                    vertex_offset,
	LIMatVector*             result));

LIAPICALL (void, liext_terrain_stick_set_vertices, (
	LIExtTerrainStick* self,
	const float*       slope,
	float              offset));

LIAPICALL (void, liext_terrain_stick_set_vertices_max, (
	LIExtTerrainStick* self,
	const float*       slope,
	float              offset));

LIAPICALL (void, liext_terrain_stick_set_vertices_min, (
	LIExtTerrainStick* self,
	const float*       slope,
	float              offset));

/*****************************************************************************/

typedef int (*LIExtTerrainStickFilter)(LIExtTerrainStick* stick, void* data);

LIAPICALL (int, liext_terrain_stick_filter_id, (
	LIExtTerrainStick* stick,
	void*              data));

LIAPICALL (int, liext_terrain_stick_filter_mask, (
	LIExtTerrainStick* stick,
	void*              data));

/*****************************************************************************/

typedef struct _LIExtTerrainColumn LIExtTerrainColumn;
struct _LIExtTerrainColumn
{
	int stamp;
	int stamp_model;
	LIExtTerrainStick* sticks;
	LIMdlModel* model;
};

LIAPICALL (int, liext_terrain_column_add_stick, (
	LIExtTerrainColumn*     self,
	float                   world_y,
	float                   world_h,
	const float*            slope_bot,
	const float*            slope_top,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data));

LIAPICALL (int, liext_terrain_column_add_stick_corners, (
	LIExtTerrainColumn*     self,
	float                   bot00,
	float                   bot10,
	float                   bot01,
	float                   bot11,
	float                   top00,
	float                   top10,
	float                   top01,
	float                   top11,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data));

LIAPICALL (int, liext_terrain_column_build_model, (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  sticks_back,
	LIExtTerrainStick*  sticks_front,
	LIExtTerrainStick*  sticks_left,
	LIExtTerrainStick*  sticks_right,
	float               grid_size));

LIAPICALL (void, liext_terrain_column_calculate_smooth_normals, (
	LIExtTerrainColumn* self,
	LIExtTerrainColumn* c10,
	LIExtTerrainColumn* c01,
	LIExtTerrainColumn* c11));

LIAPICALL (void, liext_terrain_column_clear, (
	LIExtTerrainColumn* self));

LIAPICALL (void, liext_terrain_column_clear_model, (
	LIExtTerrainColumn* self));

LIAPICALL (int, liext_terrain_column_intersect_ray, (
	LIExtTerrainColumn* self,
	const LIMatVector*  src,
	const LIMatVector*  dst,
	LIMatVector*        result_point,
	LIMatVector*        result_normal,
	float*              result_fraction));

LIAPICALL (void, liext_terrain_column_smoothen, (
	LIExtTerrainColumn* self,
	LIExtTerrainColumn* c10,
	LIExtTerrainColumn* c01,
	LIExtTerrainColumn* c11,
	float               y,
	float               h));

LIAPICALL (int, liext_terrain_column_get_data, (
	LIExtTerrainColumn* self,
	LIArcWriter*        writer));

LIAPICALL (int, liext_terrain_column_set_data, (
	LIExtTerrainColumn* self,
	LIArcReader*        reader));

/*****************************************************************************/

typedef int LIExtTerrainChunkID;

typedef struct _LIExtTerrainChunk LIExtTerrainChunk;
struct _LIExtTerrainChunk
{
	int size;
	int stamp;
	int stamp_model;
	LIExtTerrainColumn* columns;
	LIMdlModel* model;
};

LIAPICALL (LIExtTerrainChunk*, liext_terrain_chunk_new, (
	int size));

LIAPICALL (void, liext_terrain_chunk_free, (
	LIExtTerrainChunk* self));

LIAPICALL (int, liext_terrain_chunk_add_stick, (
	LIExtTerrainChunk*      self,
	int                     column_x,
	int                     column_z,
	float                   world_y,
	float                   world_h,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data));

LIAPICALL (int, liext_terrain_chunk_add_stick_corners, (
	LIExtTerrainChunk*      self,
	int                     column_x,
	int                     column_z,
	float                   bot00,
	float                   bot10,
	float                   bot01,
	float                   bot11,
	float                   top00,
	float                   top10,
	float                   top01,
	float                   top11,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data));

LIAPICALL (int, liext_terrain_chunk_build_model, (
	LIExtTerrainChunk* self,
	float              grid_size));

LIAPICALL (void, liext_terrain_chunk_clear_column, (
	LIExtTerrainChunk* self,
	int                column_x,
	int                column_z));

LIAPICALL (void, liext_terrain_chunk_clear_model, (
	LIExtTerrainChunk* self));

LIAPICALL (LIExtTerrainColumn*, liext_terrain_chunk_get_column, (
	LIExtTerrainChunk* self,
	int                x,
	int                y));

LIAPICALL (int, liext_terrain_chunk_get_data, (
	LIExtTerrainChunk* self,
	LIArcWriter*       writer));

LIAPICALL (int, liext_terrain_chunk_set_data, (
	LIExtTerrainChunk* self,
	LIArcReader*       reader));

/*****************************************************************************/

typedef struct _LIExtTerrain LIExtTerrain;
struct _LIExtTerrain
{
	int chunk_size;
	float grid_size;
	LIAlgU32dic* chunks;
	LIExtTerrainModule* module;
};

LIAPICALL (LIExtTerrain*, liext_terrain_new, (
	LIExtTerrainModule* module,
	int                 chunk_size,
	int                 grid_size));

LIAPICALL (void, liext_terrain_free, (
	LIExtTerrain* self));

LIAPICALL (int, liext_terrain_add_stick, (
	LIExtTerrain*           self,
	int                     grid_x,
	int                     grid_z,
	float                   world_y,
	float                   world_h,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data));

LIAPICALL (int, liext_terrain_add_stick_corners, (
	LIExtTerrain*           self,
	int                     grid_x,
	int                     grid_z,
	float                   bot00,
	float                   bot10,
	float                   bot01,
	float                   bot11,
	float                   top00,
	float                   top10,
	float                   top01,
	float                   top11,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data));

LIAPICALL (int, liext_terrain_calculate_smooth_normals, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z));

LIAPICALL (int, liext_terrain_clear_column, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z));

LIAPICALL (int, liext_terrain_intersect_ray, (
	LIExtTerrain*      self,
	const LIMatVector* src,
	const LIMatVector* dst,
	int*               result_x,
	int*               result_z,
	LIMatVector*       result_point,
	LIMatVector*       result_normal,
	float*             result_fraction));

LIAPICALL (int, liext_terrain_load_chunk, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z));

LIAPICALL (int, liext_terrain_smoothen_column, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	float         world_y,
	float         world_h));

LIAPICALL (int, liext_terrain_unload_chunk, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z));

LIAPICALL (LIExtTerrainChunk*, liext_terrain_get_chunk, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z));

LIAPICALL (int, liext_terrain_get_chunk_data, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	LIArcWriter*  writer));

LIAPICALL (int, liext_terrain_set_chunk_data, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	LIArcReader*  reader));

LIAPICALL (LIExtTerrainColumn*, liext_terrain_get_column, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z));

LIAPICALL (int, liext_terrain_get_column_data, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	LIArcWriter*  writer));

LIAPICALL (int, liext_terrain_set_column_data, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	LIArcReader*  reader));

LIAPICALL (int, liext_terrain_get_nearest_chunk_with_outdated_model, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	int*          result_x,
	int*          result_z));

/*****************************************************************************/

struct _LIExtTerrainModule
{
	LIMaiProgram* program;
};

LIAPICALL (LIExtTerrainModule*, liext_terrain_module_new, (
	LIMaiProgram* program));

LIAPICALL (void, liext_terrain_module_free, (
	LIExtTerrainModule* self));

/*****************************************************************************/

LIAPICALL (void, liext_script_terrain, (
	LIScrScript* self));

/*****************************************************************************/

LIAPICALL (void, liext_terrain_unittest, ());

#endif
