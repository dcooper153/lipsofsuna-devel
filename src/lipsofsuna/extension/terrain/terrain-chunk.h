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

#ifndef __EXT_TERRAIN_CHUNK_H__
#define __EXT_TERRAIN_CHUNK_H__

#include "lipsofsuna/model.h"
#include "terrain-column.h"

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
	LIExtTerrainChunk* chunk_back,
	LIExtTerrainChunk* chunk_front,
	LIExtTerrainChunk* chunk_left,
	LIExtTerrainChunk* chunk_right,
	float              grid_size,
	const LIMatVector* offset));

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

#endif
