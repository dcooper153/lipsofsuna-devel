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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtVoxel Voxel
 * @{
 */

#include <lipsofsuna/main.h>
#include <lipsofsuna/network.h>
#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Voxel"
 * ---
 * -- Use dynamic voxel terrain.
 * -- @name Voxel
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Material friction.
 * --
 * -- @name Material.friction
 * -- @class table
 */
static void Material_getter_friction (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIVoxMaterial*) args->self)->friction);
}

/* @luadoc
 * ---
 * -- Material model.
 * --
 * -- @name Material.model
 * -- @class table
 */
static void Material_getter_model (LIScrArgs* args)
{
	liscr_args_seti_string (args, ((LIVoxMaterial*) args->self)->model);
}

/* @luadoc
 * ---
 * -- Material name.
 * --
 * -- @name Material.name
 * -- @class table
 */
static void Material_getter_name (LIScrArgs* args)
{
	liscr_args_seti_string (args, ((LIVoxMaterial*) args->self)->name);
}

/*****************************************************************************/

/* @luadoc
 * ---
 * -- Creates a new tile.
 * --
 * -- @param self Tile class.
 * -- @param args Arguments.
 * -- @return New tile.
 * function Tile.new(self, args)
 */
static void Tile_new (LIScrArgs* args)
{
	LIScrData* data;

	/* Allocate userdata. */
	data = liscr_data_new_alloc (args->script, sizeof (LIVoxVoxel), LIEXT_SCRIPT_TILE);
	if (data == NULL)
		return;
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Damage counter of the tile.
 * --
 * -- @name Tile.damage
 * -- @class table
 */
static void Tile_getter_damage (LIScrArgs* args)
{
	liscr_args_seti_int (args, ((LIVoxVoxel*) args->self)->damage);
}
static void Tile_setter_damage (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
	{
		if (value < 0x00) value = 0;
		if (value > 0xFF) value = 0xFF;
		((LIVoxVoxel*) args->self)->damage = value;
	}
}

/* @luadoc
 * ---
 * -- Rotation of the tile.
 * --
 * -- @name Tile.rotation
 * -- @class table
 */
static void Tile_getter_rotation (LIScrArgs* args)
{
	liscr_args_seti_int (args, ((LIVoxVoxel*) args->self)->rotation);
}
static void Tile_setter_rotation (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
	{
		if (value <  0) value = 0;
		if (value > 24) value = 24;
		((LIVoxVoxel*) args->self)->rotation = value;
	}
}

/* @luadoc
 * ---
 * -- Terrain type of the tile.
 * --
 * -- @name Tile.terrain
 * -- @class table
 */
static void Tile_getter_terrain (LIScrArgs* args)
{
	liscr_args_seti_int (args, ((LIVoxVoxel*) args->self)->type);
}
static void Tile_setter_terrain (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
	{
		if (value < 0x0000) value = 0;
		if (value > 0xFFFF) value = 0xFFFF;
		((LIVoxVoxel*) args->self)->type = value;
	}
}

/*****************************************************************************/

/* @luadoc
 * ---
 * -- Erases a voxel near the given point.
 * --
 * -- Arguments:
 * -- point: Position vector. (required)
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return True if terrain was erased.
 * function Voxel.erase(self, args)
 */
static void Voxel_erase (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector point;

	if (liscr_args_gets_vector (args, "point", &point))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		liscr_args_seti_bool (args, livox_manager_erase_voxel (module->voxels, &point));
	}
}

/* @luadoc
 * ---
 * -- Finds all blocks near the given point.
 * --
 * -- Arguments:
 * -- point: Position vector. (required)
 * -- radius: Radius.
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return Table of block indices and modification stamps.
 * function Voxel.find_blocks(self, args)
 */
static void Voxel_find_blocks (LIScrArgs* args)
{
	int sx;
	int sy;
	int sz;
	int index;
	int stamp;
	float radius;
	LIAlgRange sectors;
	LIAlgRange blocks;
	LIAlgRange range;
	LIAlgRangeIter iter0;
	LIAlgRangeIter iter1;
	LIExtModule* module;
	LIMatVector min;
	LIMatVector max;
	LIMatVector point;
	LIMatVector size;
	LIVoxBlock* block;
	LIVoxSector* sector;
	const int line = LIVOX_BLOCKS_PER_LINE * LIVOX_SECTORS_PER_LINE;

	/* Initialize arguments. */
	if (!liscr_args_gets_vector (args, "point", &point))
		return;
	liscr_args_gets_float (args, "radius", &radius);
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);

	/* Calculate sight volume. */
	size = limat_vector_init (radius, radius, radius);
	min = limat_vector_subtract (point, size);
	max = limat_vector_add (point, size);
	sectors = lialg_range_new_from_aabb (&min, &max, LIVOX_SECTOR_WIDTH);
	sectors = lialg_range_clamp (sectors, 0, LIVOX_SECTORS_PER_LINE - 1);
	blocks = lialg_range_new_from_aabb (&min, &max, LIVOX_BLOCK_WIDTH);
	blocks = lialg_range_clamp (blocks, 0, LIVOX_SECTORS_PER_LINE * LIVOX_BLOCKS_PER_LINE - 1);

	/* Loop through visible sectors. */
	LIALG_RANGE_FOREACH (iter0, sectors)
	{
		/* Get voxel sector. */
		sector = lialg_sectors_data_index (module->voxels->sectors, "voxel", iter0.index, 0);
		if (sector == NULL)
			continue;

		/* Calculate visible block range. */
		livox_sector_get_offset (sector, &sx, &sy, &sz);
		sx *= LIVOX_BLOCKS_PER_LINE;
		sy *= LIVOX_BLOCKS_PER_LINE;
		sz *= LIVOX_BLOCKS_PER_LINE;
		range.min = 0;
		range.max = LIVOX_BLOCKS_PER_LINE;
		range.minx = LIMAT_MAX (blocks.minx - sx, 0);
		range.miny = LIMAT_MAX (blocks.miny - sy, 0);
		range.minz = LIMAT_MAX (blocks.minz - sz, 0);
		range.maxx = LIMAT_MIN (blocks.maxx - sx, LIVOX_BLOCKS_PER_LINE - 1);
		range.maxy = LIMAT_MIN (blocks.maxy - sy, LIVOX_BLOCKS_PER_LINE - 1);
		range.maxz = LIMAT_MIN (blocks.maxz - sz, LIVOX_BLOCKS_PER_LINE - 1);

		/* Loop through visible blocks. */
		LIALG_RANGE_FOREACH (iter1, range)
		{
			block = livox_sector_get_block (sector, iter1.index);
			stamp = livox_block_get_stamp (block);
			index = (sx + iter1.x) + (sy + iter1.y) * line + (sz + iter1.z) * line * line;
			liscr_args_setf_float (args, index, stamp);
		}
	}
}

/* @luadoc
 * ---
 * -- Finds information on a material.
 * --
 * -- Arguments:
 * -- id: Material ID.
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return Material or nil.
 */
static void Voxel_find_material (LIScrArgs* args)
{
	int id;
	LIExtModule* module;
	LIScrData* data;
	LIVoxMaterial* material;

	if (liscr_args_gets_int (args, "id", &id))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		material = livox_manager_find_material (module->voxels, id);
		if (material == NULL)
			return;
		material = livox_material_new_copy (material);
		if (material == NULL)
			return;
		data = liscr_data_new (args->script, material, LIEXT_SCRIPT_MATERIAL, livox_material_free);
		if (data == NULL)
		{
			livox_material_free (material);
			return;
		}
		liscr_args_seti_data (args, data);
		liscr_data_unref (data, NULL);
	}
}

/* @luadoc
 * ---
 * -- Finds the voxel nearest to the given point.
 * --
 * -- Arguments:
 * -- match: Tiles to search for. ("all"/"empty"/"full")
 * -- point: Position vector. (required)
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return Tile and vector, or nil.
 * function Voxel.find_voxel(self, args)
 */
static void Voxel_find_voxel (LIScrArgs* args)
{
	int flags = LIVOX_FIND_ALL;
	const char* tmp;
	LIExtModule* module;
	LIMatVector point;
	LIMatVector result;
	LIScrData* data;
	LIVoxVoxel* voxel;

	if (liscr_args_gets_vector (args, "point", &point))
	{
		/* Search mode. */
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		if (liscr_args_gets_string (args, "match", &tmp))
		{
			if (!strcmp (tmp, "all")) flags = LIVOX_FIND_ALL;
			else if (!strcmp (tmp, "empty")) flags = LIVOX_FIND_EMPTY;
			else if (!strcmp (tmp, "full")) flags = LIVOX_FIND_FULL;
		}

		/* Find voxel. */
		voxel = livox_manager_find_voxel (module->voxels, flags, &point, &result);
		if (voxel == NULL)
			return;

		/* Return values. */
		data = liscr_data_new_alloc (args->script, sizeof (LIVoxVoxel), LIEXT_SCRIPT_TILE);
		if (data == NULL)
			return;
		*((LIVoxVoxel*) data->data) = *voxel;
		liscr_args_seti_data (args, data);
		liscr_args_seti_vector (args, &result);
		liscr_data_unref (data, NULL);
	}
}

/* @luadoc
 * ---
 * -- Gets the data of a voxel block.
 * --
 * -- Arguments:
 * -- index: Block index. (required)
 * -- packet: Packet writer.
 * -- type: Packet type.
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return Packet writer or nil.
 * function Voxel.get_block(self, args)
 */
static void Voxel_get_block (LIScrArgs* args)
{
	int index;
	int tmp;
	int type = 1;
	LIExtModule* module;
	LIScrData* data = NULL;
	LIScrPacket* packet;
	LIVoxBlock* block;
	LIVoxSector* sector;
	LIVoxBlockAddr addr;

	/* Get block index. */
	if (!liscr_args_gets_int (args, "index", &index))
		return;
	tmp = index;
	addr.block[0] = tmp % LIVOX_BLOCKS_PER_LINE;
	addr.sector[0] = tmp / LIVOX_BLOCKS_PER_LINE % LIVOX_SECTORS_PER_LINE;
	tmp /= LIVOX_BLOCKS_PER_LINE * LIVOX_SECTORS_PER_LINE;
	addr.block[1] = tmp % LIVOX_BLOCKS_PER_LINE;
	addr.sector[1] = tmp / LIVOX_BLOCKS_PER_LINE % LIVOX_SECTORS_PER_LINE;
	tmp /= LIVOX_BLOCKS_PER_LINE * LIVOX_SECTORS_PER_LINE;
	addr.block[2] = tmp % LIVOX_BLOCKS_PER_LINE;
	addr.sector[2] = tmp / LIVOX_BLOCKS_PER_LINE % LIVOX_SECTORS_PER_LINE;

	/* Get block. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	sector = lialg_sectors_data_offset (module->program->sectors, "voxel",
		addr.sector[0], addr.sector[1], addr.sector[2], 0);
	if (sector == NULL)
		return;
	tmp = LIVOX_BLOCK_INDEX (addr.block[0], addr.block[1], addr.block[2]);
	block = livox_sector_get_block (sector, tmp);

	/* Get or create packet. */
	if (!liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
	{
		liscr_args_gets_int (args, "type", &type);
		data = liscr_packet_new_writable (args->script, type);
		if (data == NULL)
			return;
		packet = data->data;
	}
	else
	{
		packet = data->data;
		if (packet->writer == NULL)
			return;
		liscr_data_ref (data, NULL);
	}

	/* Build packet. */
	if (!liarc_writer_append_uint32 (packet->writer, index) ||
		!livox_block_write (block, packet->writer))
	{
		liscr_data_unref (data, NULL);
		return;
	}
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Gets the material configuration of the voxel system.
 * --
 * -- Arguments:
 * -- type: Packet type.
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return Packet writer.
 * function Voxel.get_materials(self, args)
 */
static void Voxel_get_materials (LIScrArgs* args)
{
	int type = 1;
	LIExtModule* module;
	LIScrData* data = NULL;
	LIScrPacket* packet;

	/* Get or create packet. */
	if (!liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
	{
		liscr_args_gets_int (args, "type", &type);
		data = liscr_packet_new_writable (args->script, type);
		if (data == NULL)
			return;
		packet = data->data;
	}
	else
	{
		packet = data->data;
		if (packet->writer == NULL)
			return;
		liscr_data_ref (data, NULL);
	}

	/* Build packet. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (!liarc_writer_append_raw (packet->writer,
	     liarc_writer_get_buffer (module->assign_packet) + 1,
	     liarc_writer_get_length (module->assign_packet) - 1))
	{
		liscr_data_unref (data, NULL);
		return;
	}
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Inserts a voxel near the given point.
 * --
 * -- Arguments:
 * -- point: Position vector. (required)
 * -- tile: Tile. (required)
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return True if terrain was filled.
 * function Voxel.insert(self, args)
 */
static void Voxel_insert (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector point;
	LIScrData* voxel;

	if (liscr_args_gets_vector (args, "point", &point) &&
	    liscr_args_gets_data (args, "tile", LIEXT_SCRIPT_TILE, &voxel))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		liscr_args_seti_bool (args, livox_manager_insert_voxel (module->voxels, &point, voxel->data));
	}
}

/* @luadoc
 * ---
 * -- Replaces the voxel near the given point.
 * --
 * -- Arguments:
 * -- point: Position vector. (required)
 * -- tile: Tile. (required)
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return True if terrain was replaced.
 * function Voxel.replace(self, args)
 */
static void Voxel_replace (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector point;
	LIScrData* voxel;

	if (liscr_args_gets_vector (args, "point", &point) &&
	    liscr_args_gets_data (args, "tile", LIEXT_SCRIPT_TILE, &voxel))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		liscr_args_seti_bool (args, livox_manager_replace_voxel (module->voxels, &point, voxel->data));
	}
}


/* @luadoc
 * ---
 * -- Rotates a voxel near the given point.
 * --
 * -- Arguments:
 * -- axis: Axis of rotation ("x"/"y"/"z" or 1/2/3)
 * -- point: Position vector. (required)
 * -- step: Number of steps to rotate.
 * --
 * -- @param self Voxel class.
 * -- @param args Argumennts.
 * -- @return True if a voxel was rotated.
 * function Voxel.rotate(self, args)
 */
static void Voxel_rotate (LIScrArgs* args)
{
	int step = 1;
	int axis = 0;
	const char* tmp;
	LIExtModule* module;
	LIMatVector point;

	if (liscr_args_gets_vector (args, "point", &point))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		if (liscr_args_gets_int (args, "step", &step))
			step %= 4;
		if (liscr_args_gets_string (args, "axis", &tmp))
		{
			if (!strcmp (tmp, "x")) axis = 0;
			else if (!strcmp (tmp, "y")) axis = 1;
			else if (!strcmp (tmp, "z")) axis = 2;
		}
		else if (liscr_args_gets_int (args, "axis", &axis))
		{
			axis = LIMAT_CLAMP (axis - 1, 0, 2);
		}
		liscr_args_seti_bool (args, livox_manager_rotate_voxel (module->voxels, &point, axis, step));
	}
}

/* @luadoc
 * ---
 * -- Saves the terrain of the currently loaded sectors.
 * --
 * -- @param self Voxel class.
 * function Voxel.save(self)
 */
static void Voxel_save (LIScrArgs* args)
{
	LIExtModule* module;
	LISerServer* server;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	server = limai_program_find_component (module->program, "server");
	if (server != NULL)
	{
		if (!liext_module_write (module, server->sql))
			lisys_error_report ();
	}
}

/* @luadoc
 * ---
 * -- Sets the contents of a voxel block.
 * --
 * -- Arguments:
 * -- packet: Packet reader.
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return True on success.
 * Voxel.set_block(self, args)
 */
static void Voxel_set_block (LIScrArgs* args)
{
	uint8_t skip;
	uint32_t index;
	uint32_t tmp;
	LIExtModule* module;
	LIVoxBlock* block;
	LIVoxBlockAddr addr;
	LIVoxSector* sector;
	LIScrData* data;
	LIScrPacket* packet;

	/* Get packet. */
	if (!liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
		return;
	packet = data->data;
	if (!packet->reader)
		return;

	/* Skip type. */
	if (!packet->reader->pos && !liarc_reader_get_uint8 (packet->reader, &skip))
		return;

	/* Read block offset. */
	if (!liarc_reader_get_uint32 (packet->reader, &index))
		return;
	tmp = index;
	addr.block[0] = tmp % LIVOX_BLOCKS_PER_LINE;
	addr.sector[0] = tmp / LIVOX_BLOCKS_PER_LINE % LIVOX_SECTORS_PER_LINE;
	tmp /= LIVOX_BLOCKS_PER_LINE * LIVOX_SECTORS_PER_LINE;
	addr.block[1] = tmp % LIVOX_BLOCKS_PER_LINE;
	addr.sector[1] = tmp / LIVOX_BLOCKS_PER_LINE % LIVOX_SECTORS_PER_LINE;
	tmp /= LIVOX_BLOCKS_PER_LINE * LIVOX_SECTORS_PER_LINE;
	addr.block[2] = tmp % LIVOX_BLOCKS_PER_LINE;
	addr.sector[2] = tmp / LIVOX_BLOCKS_PER_LINE % LIVOX_SECTORS_PER_LINE;

	/* Find or create sector. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	sector = lialg_sectors_data_offset (module->voxels->sectors, "voxel",
		addr.sector[0], addr.sector[1], addr.sector[2], 1);
	if (sector == NULL)
		return;

	/* Read block data. */
	index = LIVOX_BLOCK_INDEX (addr.block[0], addr.block[1], addr.block[2]);
	block = livox_sector_get_block (sector, index);
	if (!livox_block_read (block, module->voxels, packet->reader))
		return;
	if (livox_block_get_dirty (block))
		livox_sector_set_dirty (sector, 1);

	/* Indicate success. */
	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * ---
 * -- Sets the materials used by the voxel system.
 * --
 * -- Arguments:
 * -- packet: Packet reader.
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.
 * -- @return True on success.
 * Voxel.set_materials(self, args)
 */
static void Voxel_set_materials (LIScrArgs* args)
{
	uint8_t skip;
	LIExtModule* module;
	LIVoxMaterial* material;
	LIScrData* data;
	LIScrPacket* packet;

	/* Get packet. */
	if (!liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
		return;
	packet = data->data;
	if (!packet->reader)
		return;

	/* Skip type. */
	if (!packet->reader->pos && !liarc_reader_get_uint8 (packet->reader, &skip))
		return;

	/* Clear old materials. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	livox_manager_clear_materials (module->voxels);

	/* Read materials. */
	while (!liarc_reader_check_end (packet->reader))
	{
		material = livox_material_new_from_stream (packet->reader);
		if (material == NULL)
			return;
		if (!livox_manager_insert_material (module->voxels, material))
			livox_material_free (material);
	}

	/* Indicate success. */
	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * ---
 * -- Fill type for empty sectors.
 * -- @name Server.time
 * -- @class table
 */
static void Voxel_getter_fill (LIScrArgs* args)
{
	LIExtModule* self;
	LIScrData* voxel;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (self->voxels->fill)
	{
		voxel = liscr_data_new_alloc (args->script, sizeof (LIVoxVoxel), LIEXT_SCRIPT_TILE);
		if (voxel != NULL)
		{
			livox_voxel_init (voxel->data, self->voxels->fill);
			liscr_args_seti_data (args, voxel);
			liscr_data_unref (voxel, NULL);
		}
	}
}
static void Voxel_setter_fill (LIScrArgs* args)
{
	LIExtModule* self;
	LIScrData* voxel;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (liscr_args_geti_data (args, 0, LIEXT_SCRIPT_TILE, &voxel))
		livox_manager_set_fill (self->voxels, ((LIVoxVoxel*) voxel->data)->type);
	else
		livox_manager_set_fill (self->voxels, 0);
}

/*****************************************************************************/

void
liext_script_material (LIScrClass* self,
                       void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_MATERIAL, data);
	liscr_class_insert_mvar (self, "friction", Material_getter_friction, NULL);
	liscr_class_insert_mvar (self, "name", Material_getter_name, NULL);
	liscr_class_insert_mvar (self, "model", Material_getter_model, NULL);
}

void
liext_script_tile (LIScrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_TILE, data);
	liscr_class_insert_cfunc (self, "new", Tile_new);
	liscr_class_insert_mvar (self, "damage", Tile_getter_damage, Tile_setter_damage);
	liscr_class_insert_mvar (self, "rotation", Tile_getter_rotation, Tile_setter_rotation);
	liscr_class_insert_mvar (self, "terrain", Tile_getter_terrain, Tile_setter_terrain);
}

void
liext_script_voxel (LIScrClass* self,
                    void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_VOXEL, data);
	liscr_class_insert_cfunc (self, "erase", Voxel_erase);
	liscr_class_insert_cfunc (self, "find_blocks", Voxel_find_blocks);
	liscr_class_insert_cfunc (self, "find_material", Voxel_find_material);
	liscr_class_insert_cfunc (self, "find_voxel", Voxel_find_voxel);
	liscr_class_insert_cfunc (self, "get_block", Voxel_get_block);
	liscr_class_insert_cfunc (self, "get_materials", Voxel_get_materials);
	liscr_class_insert_cfunc (self, "insert", Voxel_insert);
	liscr_class_insert_cfunc (self, "replace", Voxel_replace);
	liscr_class_insert_cfunc (self, "rotate", Voxel_rotate);
	liscr_class_insert_cfunc (self, "save", Voxel_save);
	liscr_class_insert_cfunc (self, "set_block", Voxel_set_block);
	liscr_class_insert_cfunc (self, "set_materials", Voxel_set_materials);
	liscr_class_insert_cvar (self, "fill", Voxel_getter_fill, Voxel_setter_fill);
}

/** @} */
/** @} */
