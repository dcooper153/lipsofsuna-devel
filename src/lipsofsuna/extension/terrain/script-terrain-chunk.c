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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtTerrain Terrain
 * @{
 */

#include "module.h"
#include "terrain-chunk.h"

static void TerrainChunk_new (LIScrArgs* args)
{
	int chunk_size;
	LIExtTerrainChunk* self;
	LIScrData* data;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &chunk_size) || chunk_size < 1)
		return;

	/* Allocate the object. */
	self = liext_terrain_chunk_new (chunk_size);
	if (self == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_TERRAIN_CHUNK, liext_terrain_chunk_free);
	if (data == NULL)
	{
		liext_terrain_chunk_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void TerrainChunk_add_box (LIScrArgs* args)
{
	int grid_x1;
	int grid_z1;
	int grid_x2;
	int grid_z2;
	float world_y;
	float world_h;
	int material;
	LIExtTerrainChunk* self;

	/* Get the arguments. */
	self = args->self;
	if (!liscr_args_geti_int (args, 0, &grid_x1) || grid_x1 < 0 || grid_x1 >= self->size)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z1) || grid_z1 < 0 || grid_z1 >= self->size)
		return;
	if (!liscr_args_geti_int (args, 2, &grid_x2) || grid_x2 < grid_x1 || grid_x2 >= self->size)
		return;
	if (!liscr_args_geti_int (args, 3, &grid_z2) || grid_z2 < grid_z1 || grid_z2 >= self->size)
		return;
	if (!liscr_args_geti_float (args, 4, &world_y) || world_y < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 5, &world_h))
		return;
	world_h = LIMAT_MAX (world_h, LIEXT_STICK_EPSILON);
	if (!liscr_args_geti_int (args, 6, &material) || material < 0)
		return;

	liscr_args_seti_int (args, liext_terrain_chunk_add_box (self,
		grid_x1, grid_z1, grid_x2, grid_z2, world_y, world_h,
		material, NULL, NULL));
}

static void TerrainChunk_add_box_corners (LIScrArgs* args)
{
	int grid_x1;
	int grid_z1;
	int grid_x2;
	int grid_z2;
	float bot[4];
	float top[4];
	int material;
	LIExtTerrainChunk* self;

	/* Get the arguments. */
	self = args->self;
	if (!liscr_args_geti_int (args, 0, &grid_x1) || grid_x1 < 0 || grid_x1 >= self->size)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z1) || grid_z1 < 0 || grid_z1 >= self->size)
		return;
	if (!liscr_args_geti_int (args, 2, &grid_x2) || grid_x2 < grid_x1 || grid_x2 >= self->size)
		return;
	if (!liscr_args_geti_int (args, 3, &grid_z2) || grid_z2 < grid_z1 || grid_z2 >= self->size)
		return;
	if (!liscr_args_geti_float (args, 4, bot + 0) || bot[0] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 5, bot + 1) || bot[1] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 6, bot + 2) || bot[2] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 7, bot + 3) || bot[3] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 8, top + 0) || top[0] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 9, top + 1) || top[1] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 10, top + 2) || top[2] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 11, top + 3) || top[3] < 0.0f)
		return;
	if (!liscr_args_geti_int (args, 12, &material) || material < 0)
		return;

	liscr_args_seti_bool (args, liext_terrain_chunk_add_box_corners (self, grid_x1, grid_z1, grid_x2, grid_z2,
		bot[0], bot[1], bot[2], bot[3], top[0], top[1], top[2], top[3], material, NULL, NULL));
}

static void TerrainChunk_add_stick (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	float world_y;
	float world_h;
	int material;
	LIExtTerrainChunk* self;

	/* Get the arguments. */
	self = args->self;
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0 || grid_x >= self->size)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0 || grid_z >= self->size)
		return;
	if (!liscr_args_geti_float (args, 2, &world_y) || world_y < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 3, &world_h))
		return;
	world_h = LIMAT_MAX (world_h, LIEXT_STICK_EPSILON);
	if (!liscr_args_geti_int (args, 4, &material) || material < 0)
		return;

	liscr_args_seti_bool (args, liext_terrain_chunk_add_stick (args->self, grid_x, grid_z, world_y, world_h,
		material, NULL, NULL));
}

static void TerrainChunk_add_stick_corners (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	float bot[4];
	float top[4];
	int material;
	LIExtTerrainChunk* self;

	/* Get the arguments. */
	self = args->self;
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0 || grid_x >= self->size)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0 || grid_z >= self->size)
		return;
	if (!liscr_args_geti_float (args, 2, bot + 0) || bot[0] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 3, bot + 1) || bot[1] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 4, bot + 2) || bot[2] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 5, bot + 3) || bot[3] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 6, top + 0) || top[0] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 7, top + 1) || top[1] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 8, top + 2) || top[2] < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 9, top + 3) || top[3] < 0.0f)
		return;
	if (!liscr_args_geti_int (args, 10, &material) || material < 0)
		return;

	liscr_args_seti_bool (args, liext_terrain_chunk_add_stick_corners (args->self, grid_x, grid_z,
		bot[0], bot[1], bot[2], bot[3], top[0], top[1], top[2], top[3], material, NULL, NULL));
}

static void TerrainChunk_clear_column (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	LIExtTerrainChunk* self;

	/* Get the arguments. */
	self = args->self;
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0 || grid_x >= self->size)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0 || grid_z >= self->size)
		return;

	liext_terrain_chunk_clear_column (args->self, grid_x, grid_z);
}

static void TerrainChunk_get_chunk_data (LIScrArgs* args)
{
	LIArcPacket* packet;
	LIScrData* data;

	/* Get the packet writer. */
	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);
	if (packet->writer == NULL)
		return;

	/* Read the data. */
	liscr_args_seti_bool (args, liext_terrain_chunk_get_data (args->self, packet->writer));
}

static void TerrainChunk_set_chunk_data (LIScrArgs* args)
{
	LIArcPacket* packet;
	LIScrData* data;

	/* Get the packet reader. */
	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);

	/* Read the data. */
	if (packet->reader != NULL)
		liscr_args_seti_bool (args, liext_terrain_chunk_set_data (args->self, packet->reader));
}

static void TerrainChunk_get_column (LIScrArgs* args)
{
	int i;
	int vtx_x;
	int vtx_z;
	int grid_x;
	int grid_z;
	LIExtTerrainChunk* self;
	LIExtTerrainColumn* column;
	LIExtTerrainStick* stick;

	/* Get the arguments. */
	self = args->self;
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0 || grid_x >= self->size)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0 || grid_z >= self->size)
		return;

	/* Get the column. */
	column = liext_terrain_chunk_get_column (self, grid_x, grid_z);
	if (column == NULL)
		return;

	/* Pack the sticks into a table. */
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	for (stick = column->sticks ; stick != NULL ; stick = stick->next)
	{
		i = 0;
		lua_newtable (args->lua);
#define PUSHNUM(v) lua_pushnumber (args->lua, ++i);\
                   lua_pushnumber (args->lua, v);\
                   lua_settable (args->lua, -3);
		PUSHNUM (stick->material);
		PUSHNUM (stick->height);
		for (vtx_z = 0 ; vtx_z < 2 ; vtx_z++)
		{
			for (vtx_x = 0 ; vtx_x < 2 ; vtx_x++)
			{
				PUSHNUM (stick->vertices[vtx_x][vtx_z].offset);
				PUSHNUM (stick->vertices[vtx_x][vtx_z].splatting);
				PUSHNUM (stick->vertices[vtx_x][vtx_z].normal.x);
				PUSHNUM (stick->vertices[vtx_x][vtx_z].normal.y);
				PUSHNUM (stick->vertices[vtx_x][vtx_z].normal.z);
			}
		}
#undef PUSHNUM
		liscr_args_seti_stack (args);
	}
}

static void TerrainChunk_set_column (LIScrArgs* args)
{
	float y;
	float height;
	float slope[4];
	float slope_prev[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	int i;
	int j;
	int grid_x;
	int grid_z;
	int material;
	LIExtTerrainChunk* self;
	LIExtTerrainColumn* column;

	/* Get the arguments. */
	self = args->self;
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0 || grid_x >= self->size)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0 || grid_z >= self->size)
		return;
	if (!liscr_args_geti_table (args, 2))
		return;

	/* Get the column. */
	column = liext_terrain_chunk_get_column (self, grid_x, grid_z);
	if (column == NULL)
		return;

	/* Unpack the sticks from the table. */
	for (y = 0.0f, i = 0 ; 1 ; i++)
	{
		/* Get the next stick table. */
		lua_pushnumber (args->lua, i + 1);
		lua_gettable (args->lua, -2);
		if (lua_type (args->lua, -1) != LUA_TTABLE)
			break;

		/* Get the height. */
		lua_pushnumber (args->lua, 1);
		lua_gettable (args->lua, -2);
		if (lua_type (args->lua, -1) != LUA_TNUMBER)
		{
			lua_pop (args->lua, 2);
			continue;
		}
		height = lua_tonumber (args->lua, -1);
		if (height <= LIEXT_STICK_EPSILON)
		{
			lua_pop (args->lua, 2);
			continue;
		}
		lua_pop (args->lua, 1);

		/* Get the material. */
		lua_pushnumber (args->lua, 2);
		lua_gettable (args->lua, -2);
		if (lua_type (args->lua, -1) != LUA_TNUMBER)
		{
			lua_pop (args->lua, 2);
			continue;
		}
		material = lua_tointeger (args->lua, -1);
		if (material < 0)
		{
			lua_pop (args->lua, 2);
			continue;
		}
		lua_pop (args->lua, 1);

		/* Get the slope. */
		for (j = 0 ; j < 4 ; j++)
		{
			slope[j] = 0.0f;
			lua_pushnumber (args->lua, 2 + j);
			lua_gettable (args->lua, -2);
			if (lua_type (args->lua, -1) != LUA_TNUMBER)
			{
				lua_pop (args->lua, 1);
				continue;
			}
			slope[j] = lua_tonumber (args->lua, -1);
			lua_pop (args->lua, 1);
		}
		lua_pop (args->lua, 1);

		/* Add the stick. */
		liext_terrain_column_add_stick (column, y, height, slope_prev, slope, material, NULL, NULL);
		y += height;
		memcpy (slope_prev, slope, 4 * sizeof (float));
	}

	liscr_args_seti_bool (args, 1);
}

static void TerrainChunk_get_column_data (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	LIArcPacket* packet;
	LIExtTerrainChunk* self;
	LIScrData* data;

	/* Get the arguments. */
	self = args->self;
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0 || grid_x >= self->size)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0 || grid_z >= self->size)
		return;

	/* Get the packet writer. */
	if (!liscr_args_geti_data (args, 2, LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);
	if (packet->writer == NULL)
		return;

	/* Read the data. */
	liscr_args_seti_bool (args, liext_terrain_chunk_get_column_data (self, grid_x, grid_z, packet->writer));
}

static void TerrainChunk_set_column_data (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	LIArcPacket* packet;
	LIExtTerrainChunk* self;
	LIScrData* data;

	/* Get the arguments. */
	self = args->self;
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0 || grid_x >= self->size)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0 || grid_z >= self->size)
		return;

	/* Get the packet reader. */
	if (!liscr_args_geti_data (args, 2, LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);
	if (packet->reader == NULL)
		return;

	/* Read the data. */
	liscr_args_seti_bool (args, liext_terrain_chunk_set_column_data (args->self, grid_x, grid_z, packet->reader));
}

static void TerrainChunk_get_memory_used (LIScrArgs* args)
{
	liscr_args_seti_int (args, liext_terrain_chunk_get_memory_used (args->self));
}

static void TerrainChunk_get_stick (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	float y;
	float ref_y;
	LIExtTerrainChunk* self;
	LIExtTerrainColumn* column;
	LIExtTerrainStick* stick;

	/* Get the arguments. */
	self = args->self;
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0 || grid_x >= self->size)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0 || grid_z >= self->size)
		return;
	if (!liscr_args_geti_float (args, 2, &ref_y))
		return;

	/* Get the column. */
	column = liext_terrain_chunk_get_column (args->self, grid_x, grid_z);
	if (column == NULL)
		return;

	/* Find the stick. */
	y = 0.0f;
	for (stick = column->sticks ; stick != NULL ; stick = stick->next)
	{
		if (ref_y < y + stick->height)
		{
			liscr_args_seti_float (args, y);
			liscr_args_seti_float (args, stick->height);
			liscr_args_seti_int (args, stick->material);
			return;
		}
		y += stick->height;
	}

	/* Return the infinite end stick. */
	liscr_args_seti_float (args, y);
	liscr_args_seti_float (args, 1000000.0f);
	liscr_args_seti_int (args, 0);
}

/*****************************************************************************/

void liext_script_terrain_chunk (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_new", TerrainChunk_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_add_box", TerrainChunk_add_box);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_add_box_corners", TerrainChunk_add_box_corners);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_add_stick", TerrainChunk_add_stick);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_add_stick_corners", TerrainChunk_add_stick_corners);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_clear_column", TerrainChunk_clear_column);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_get_chunk_data", TerrainChunk_get_chunk_data);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_set_chunk_data", TerrainChunk_set_chunk_data);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_get_column", TerrainChunk_get_column);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_set_column", TerrainChunk_set_column);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_get_column_data", TerrainChunk_get_column_data);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_set_column_data", TerrainChunk_set_column_data);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_get_memory_used", TerrainChunk_get_memory_used);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN_CHUNK, "terrain_chunk_get_stick", TerrainChunk_get_stick);
}

/** @} */
/** @} */
