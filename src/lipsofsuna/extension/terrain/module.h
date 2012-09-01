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

#define LIEXT_STICK_EPSILON 0.01

typedef struct _LIExtTerrainStick LIExtTerrainStick;
struct _LIExtTerrainStick
{
	LIExtTerrainStick* next;
	int material;
	float height;
	float corners[2][2];
};

LIAPICALL (LIExtTerrainStick*, liext_terrain_stick_new, (
	int   material,
	float height));

LIAPICALL (void, liext_terrain_stick_free, (
	LIExtTerrainStick* self));

LIAPICALL (int, liext_terrain_stick_get_data, (
	LIExtTerrainStick* self,
	LIArcWriter*       writer));

LIAPICALL (int, liext_terrain_stick_set_data, (
	LIExtTerrainStick* self,
	LIArcReader*       reader));

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
	LIExtTerrainColumn* self,
	float         world_y,
	float         world_h,
	int           material));

LIAPICALL (void, liext_terrain_column_clear, (
	LIExtTerrainColumn* self));

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
};

LIAPICALL (LIExtTerrain*, liext_terrain_new, (
	int chunk_size,
	int grid_size));

LIAPICALL (void, liext_terrain_free, (
	LIExtTerrain* self));

LIAPICALL (int, liext_terrain_add_stick, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	float         world_y,
	float         world_h,
	int           material));

LIAPICALL (int, liext_terrain_clear_column, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z));

LIAPICALL (int, liext_terrain_load_chunk, (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z));

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

/*****************************************************************************/

typedef struct _LIExtTerrainModule LIExtTerrainModule;
struct _LIExtTerrainModule
{
	LIMaiProgram* program;
};

LIExtTerrainModule* liext_terrain_module_new (
	LIMaiProgram* program);

void liext_terrain_module_free (
	LIExtTerrainModule* self);

/*****************************************************************************/

void liext_script_terrain (
	LIScrScript* self);

#endif
