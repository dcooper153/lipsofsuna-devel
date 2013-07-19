/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __EXT_TERRAIN_TERRAIN_H__
#define __EXT_TERRAIN_TERRAIN_H__

#include "terrain-types.h"
#include "terrain-chunk.h"
#include "terrain-column.h"
#include "terrain-material.h"
#include "terrain-stick.h"
#include "terrain-stick-filter.h"

struct _LIExtTerrain
{
	int chunk_size;
	float grid_size;
	LIAlgU32dic* chunks;
	LIExtTerrainModule* module;
	LIExtTerrainMaterial materials[LIEXT_TERRAIN_MATERIAL_MAX];
};

LIAPICALL (LIExtTerrain*, liext_terrain_new, (
	LIExtTerrainModule* module,
	int                 chunk_size,
	float               grid_size));

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

LIAPICALL (LIMdlModel*, liext_terrain_build_chunk_model, (
	LIExtTerrain*      self,
	int                grid_x,
	int                grid_z,
	const LIMatVector* offset));

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

#endif
