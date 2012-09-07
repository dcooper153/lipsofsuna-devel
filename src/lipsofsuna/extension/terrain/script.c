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
	LIExtTerrainModule* module;
	LIScrData* data;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &chunk_size) || chunk_size < 1)
		return;
	if (!liscr_args_geti_float (args, 1, &grid_size) || grid_size <= 0.0f)
		return;
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_TERRAIN);

	/* Allocate the object. */
	self = liext_terrain_new (module, chunk_size, grid_size);
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

static void Terrain_build_chunk_model (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	LIExtTerrain* self;
	LIExtTerrainChunk* chunk;
	LIExtTerrainModule* module;
	LIMdlModel* model;
	LIScrData* data;

	/* Get the arguments. */
	self = args->self;
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_TERRAIN);
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;

	/* Build the model. */
	chunk = liext_terrain_get_chunk (self, grid_x, grid_z);
	if (chunk == NULL)
		return;
	if (!liext_terrain_chunk_build_model (chunk, self->grid_size))
		return;

	/* Copy the model. */
	if (chunk->model == NULL)
		return;
	model = limdl_model_new_copy (chunk->model, 0);
	if (model == NULL)
		return;

	/* Allocate the unique ID. */
	if (!limdl_manager_add_model (module->program->models, model))
	{
		limdl_model_free (model);
		return;
	}

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, model, LISCR_SCRIPT_MODEL, limdl_manager_free_model);
	if (data == NULL)
	{
		limdl_model_free (model);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Terrain_cast_ray (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	float fraction;
	LIMatVector src;
	LIMatVector dst;
	LIMatVector point;
	LIMatVector normal;

	/* Get the arguments. */
	if (!liscr_args_geti_vector (args, 0, &src))
		return;
	if (!liscr_args_geti_vector (args, 1, &dst))
		return;

	if (!liext_terrain_intersect_ray (args->self, &src, &dst, &grid_x, &grid_z, &point, &normal, &fraction))
		return;
	liscr_args_seti_int (args, grid_x);
	liscr_args_seti_int (args, grid_z);
	liscr_args_seti_float (args, point.x);
	liscr_args_seti_float (args, point.y);
	liscr_args_seti_float (args, point.z);
	liscr_args_seti_float (args, normal.x);
	liscr_args_seti_float (args, normal.y);
	liscr_args_seti_float (args, normal.z);
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

static void Terrain_smoothen_column (LIScrArgs* args)
{
	int grid_x;
	int grid_z;
	float y;
	float h;

	/* Get the arguments. */
	if (!liscr_args_geti_int (args, 0, &grid_x) || grid_x < 0)
		return;
	if (!liscr_args_geti_int (args, 1, &grid_z) || grid_z < 0)
		return;
	if (!liscr_args_geti_float (args, 2, &y))
		return;
	if (!liscr_args_geti_float (args, 3, &h) || h <= 0.0f)
		return;

	liscr_args_seti_bool (args, liext_terrain_smoothen_column (args->self, grid_x, grid_z, y, h));
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
	int i;
	int vtx_x;
	int vtx_z;
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
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_build_chunk_model", Terrain_build_chunk_model);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_cast_ray", Terrain_cast_ray);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_clear_column", Terrain_clear_column);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_load_chunk", Terrain_load_chunk);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_TERRAIN, "terrain_smoothen_column", Terrain_smoothen_column);
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
