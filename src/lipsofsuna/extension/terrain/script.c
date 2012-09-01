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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtTerrain Terrain
 * @{
 */

#include "module.h"

static void Terrain_new (LIScrArgs* args)
{
	int chunk_size;
	float grid_size;
	LIExtTerrain* self;
	LIScrData* data;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &chunk_size) || chunk_size < 1)
		return;
	if (!liscr_args_geti_float (args, 1, &grid_size) || grid_size <= 0.0f)
		return;

	/* Allocate the object. */
	self = liext_terrain_new (chunk_size, grid_size);
	if (self == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_TERRAIN, liext_terrain_free);
	if (data == NULL)
	{
		liext_terrain_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Terrain_add_stick (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	float world_y;
	float world_h;
	int material;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;
	if (!liscr_args_geti_float (args, 2, &world_y) || world_y < 0.0f)
		return;
	if (!liscr_args_geti_float (args, 3, &world_h))
		return;
	world_h = LIMAT_MAX (world_h, LIEXT_STICK_EPSILON);
	if (!liscr_args_geti_int (args, 4, &material) || material < 0)
		return;

	liscr_args_seti_bool (args, liext_terrain_add_stick (args->self, grid_x, grid_z, world_y, world_h, material));
}

static void Terrain_clear_column (LIScrArgs* args)
{
	int grid_x;
	int grid_z;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;

	liscr_args_seti_bool (args, liext_terrain_clear_column (args->self, grid_x, grid_z));
}

static void Terrain_load_chunk (LIScrArgs* args)
{
	int grid_x;
	int grid_z;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;

	liscr_args_seti_bool (args, liext_terrain_load_chunk (args->self, grid_x, grid_z));
}

static void Terrain_unload_chunk (LIScrArgs* args)
{
	int grid_x;
	int grid_z;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;

	liscr_args_seti_bool (args, liext_terrain_unload_chunk (args->self, grid_x, grid_z));
}

static void Terrain_get_chunk_data (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	LIArcPacket* packet;
	LIScrData* data;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;

	/* Get the packet writer. */
	if (!liscr_args_geti_data (args, 2, LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);
	if (packet->writer == NULL)
		return;

	/* Read the data. */
	liscr_args_seti_bool (args, liext_terrain_get_chunk_data (args->self, grid_x, grid_z, packet->writer));
}

static void Terrain_set_chunk_data (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	LIArcPacket* packet;
	LIScrData* data;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;

	/* Get the packet reader. */
	if (!liscr_args_geti_data (args, 2, LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);
	if (packet->reader == NULL)
		return;

	/* Read the data. */
	liscr_args_seti_bool (args, liext_terrain_set_chunk_data (args->self, grid_x, grid_z, packet->reader));
}

static void Terrain_get_column (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	LIExtTerrainColumn* column;
	LIExtTerrainStick* stick;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;

	/* Get the column. */
	column = liext_terrain_get_column (args->self, grid_x, grid_z);
	if (column == NULL)
		return;

	/* Pack the sticks into a table. */
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	for (stick = column->sticks ; stick != NULL ; stick = stick->next)
	{
		lua_newtable (args->lua);
		lua_pushnumber (args->lua, 1);
		lua_pushnumber (args->lua, stick->material);
		lua_settable (args->lua, -3);
		lua_pushnumber (args->lua, 2);
		lua_pushnumber (args->lua, stick->height);
		lua_settable (args->lua, -3);
		lua_pushnumber (args->lua, 3);
		lua_pushnumber (args->lua, stick->corners[0][0]);
		lua_settable (args->lua, -3);
		lua_pushnumber (args->lua, 4);
		lua_pushnumber (args->lua, stick->corners[1][0]);
		lua_settable (args->lua, -3);
		lua_pushnumber (args->lua, 5);
		lua_pushnumber (args->lua, stick->corners[0][1]);
		lua_settable (args->lua, -3);
		lua_pushnumber (args->lua, 6);
		lua_pushnumber (args->lua, stick->corners[1][1]);
		lua_settable (args->lua, -3);
		liscr_args_seti_stack (args);
	}
}

static void Terrain_set_column (LIScrArgs* args)
{
	float y;
	float height;
	int i;
	int grid_x;
	int grid_z;
	int material;
	LIExtTerrainColumn* column;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;
	if (!liscr_args_geti_table (args, 2))
		return;

	/* Get the column. */
	column = liext_terrain_get_column (args->self, grid_x, grid_z);
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
		lua_pop (args->lua, 2);

		/* Get the material. */
		lua_pushnumber (args->lua, i + 1);
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

		/* Add the stick. */
		liext_terrain_column_add_stick (column, y, height, material);
		y += height;
	}

	liscr_args_seti_bool (args, 1);
}

static void Terrain_get_column_data (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	LIArcPacket* packet;
	LIScrData* data;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;

	/* Get the packet writer. */
	if (!liscr_args_geti_data (args, 2, LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);
	if (packet->writer == NULL)
		return;

	/* Read the data. */
	liscr_args_seti_bool (args, liext_terrain_get_column_data (args->self, grid_x, grid_z, packet->writer));
}

static void Terrain_set_column_data (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	LIArcPacket* packet;
	LIScrData* data;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;

	/* Get the packet reader. */
	if (!liscr_args_geti_data (args, 2, LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);
	if (packet->reader == NULL)
		return;

	/* Read the data. */
	liscr_args_seti_bool (args, liext_terrain_set_column_data (args->self, grid_x, grid_z, packet->reader));
}

/*****************************************************************************/

void liext_script_terrain (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_new", Terrain_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_add_stick", Terrain_add_stick);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_clear_column", Terrain_clear_column);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_load_chunk", Terrain_load_chunk);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_unload_chunk", Terrain_unload_chunk);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_get_chunk_data", Terrain_get_chunk_data);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_set_chunk_data", Terrain_set_chunk_data);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_get_column", Terrain_get_column);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_set_column", Terrain_set_column);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_get_column_data", Terrain_get_column_data);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_set_column_data", Terrain_set_column_data);
}

/** @} */
/** @} */
