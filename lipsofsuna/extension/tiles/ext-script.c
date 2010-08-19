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

#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Voxel"
 * --- Use dynamic voxel terrain.
 * -- @name Voxel
 * -- @class table
 */

/* @luadoc
 * --- Material friction.
 * --
 * -- @name Material.friction
 * -- @class table
 */
static void Material_getter_friction (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIVoxMaterial*) args->self)->friction);
}

/* @luadoc
 * --- Material model.
 * --
 * -- @name Material.model
 * -- @class table
 */
static void Material_getter_model (LIScrArgs* args)
{
	liscr_args_seti_string (args, ((LIVoxMaterial*) args->self)->model);
}

/* @luadoc
 * --- Material name.
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
 * --- Creates a new tile.
 * --
 * -- @param clss Tile class.
 * -- @param args Arguments.
 * -- @return New tile.
 * function Tile.new(clss, args)
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
 * --- Damage counter of the tile.
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
 * --- Rotation of the tile.
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
 * --- Terrain type of the tile.
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
 * --- Copies a terrain region into a packet.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>point: Position vector. (required)</li>
 * --   <li>size: Size vector. (required)</li></ul>
 * -- @return Packet writer.
 * function Voxel.copy_region(clss, args)
 */
static void Voxel_copy_region (LIScrArgs* args)
{
	int i;
	int length;
	LIArcWriter* writer;
	LIExtModule* module;
	LIMatVector point;
	LIMatVector size;
	LIScrData* packet;
	LIVoxVoxel* result;

	if (liscr_args_gets_vector (args, "point", &point) &&
	    liscr_args_gets_vector (args, "size", &size))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
		length = ((int) size.x / module->voxels->tile_width) *
		         ((int) size.y / module->voxels->tile_width) *
		         ((int) size.z / module->voxels->tile_width);

		/* Read voxel data. */
		if (length)
		{
			result = lisys_calloc (length, sizeof (LIVoxVoxel));
			if (result == NULL)
				return;
			livox_manager_copy_voxels (module->voxels,
				(int) point.x / module->voxels->tile_width,
				(int) point.y / module->voxels->tile_width,
				(int) point.z / module->voxels->tile_width,
				(int) size.x / module->voxels->tile_width,
				(int) size.y / module->voxels->tile_width,
				(int) size.z / module->voxels->tile_width, result);
		}

		/* Create packet writer. */
		packet = liscr_packet_new_writable (args->script, 0);
		if (packet == NULL)
		{
			lisys_free (result);
			return;
		}
		writer = ((LIScrPacket*) packet->data)->writer;

		/* Write dimensions. */
		if (!liarc_writer_append_uint32 (writer, (int) size.x / module->voxels->tile_width) ||
			!liarc_writer_append_uint32 (writer, (int) size.y / module->voxels->tile_width) ||
			!liarc_writer_append_uint32 (writer, (int) size.z / module->voxels->tile_width))
		{
			lisys_free (result);
			return;
		}

		/* Write voxel data. */
		for (i = 0 ; i < length ; i++)
		{
			if (!liarc_writer_append_uint16 (writer, result[i].type) ||
				!liarc_writer_append_uint8 (writer, result[i].damage) ||
				!liarc_writer_append_uint8 (writer, result[i].rotation))
			{
				lisys_free (result);
				return;
			}
		}

		/* Return data. */
		liscr_args_seti_data (args, packet);
		lisys_free (result);
	}
}

/* @luadoc
 * --- Erases a voxel near the given point.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>point: Position vector. (required)</li></ul>
 * -- @return True if terrain was erased.
 * function Voxel.erase(clss, args)
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
 * --- Finds all blocks near the given point.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>point: Position vector. (required)</li>
 * --   <li>radius: Radius.</li></ul>
 * -- @return Table of block indices and modification stamps.
 * function Voxel.find_blocks(clss, args)
 */
static void Voxel_find_blocks (LIScrArgs* args)
{
	int sx;
	int sy;
	int sz;
	int index;
	int line;
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

	/* Initialize arguments. */
	if (!liscr_args_gets_vector (args, "point", &point))
		return;
	liscr_args_gets_float (args, "radius", &radius);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	line = module->voxels->blocks_per_line * module->voxels->sectors->count;

	/* Calculate sight volume. */
	size = limat_vector_init (radius, radius, radius);
	min = limat_vector_subtract (point, size);
	max = limat_vector_add (point, size);
	sectors = lialg_range_new_from_aabb (&min, &max, module->voxels->sectors->width);
	sectors = lialg_range_clamp (sectors, 0, module->voxels->sectors->count - 1);
	blocks = lialg_range_new_from_aabb (&min, &max, module->voxels->sectors->width / module->voxels->blocks_per_line);
	blocks = lialg_range_clamp (blocks, 0, module->voxels->blocks_per_line * module->voxels->sectors->count - 1);

	/* Loop through visible sectors. */
	LIALG_RANGE_FOREACH (iter0, sectors)
	{
		/* Get voxel sector. */
		sector = lialg_sectors_data_index (module->voxels->sectors, "voxel", iter0.index, 0);
		if (sector == NULL)
			continue;

		/* Calculate visible block range. */
		livox_sector_get_offset (sector, &sx, &sy, &sz);
		sx *= module->voxels->blocks_per_line;
		sy *= module->voxels->blocks_per_line;
		sz *= module->voxels->blocks_per_line;
		range.min = 0;
		range.max = module->voxels->blocks_per_line;
		range.minx = LIMAT_MAX (blocks.minx - sx, 0);
		range.miny = LIMAT_MAX (blocks.miny - sy, 0);
		range.minz = LIMAT_MAX (blocks.minz - sz, 0);
		range.maxx = LIMAT_MIN (blocks.maxx - sx, module->voxels->blocks_per_line - 1);
		range.maxy = LIMAT_MIN (blocks.maxy - sy, module->voxels->blocks_per_line - 1);
		range.maxz = LIMAT_MIN (blocks.maxz - sz, module->voxels->blocks_per_line - 1);

		/* Loop through visible blocks. */
		LIALG_RANGE_FOREACH (iter1, range)
		{
			block = livox_sector_get_block (sector, iter1.x, iter1.y, iter1.z);
			stamp = livox_block_get_stamp (block);
			index = (sx + iter1.x) + (sy + iter1.y) * line + (sz + iter1.z) * line * line;
			liscr_args_setf_float (args, index, stamp);
		}
	}
}

/* @luadoc
 * --- Finds information on a material.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>id: Material ID.</li></ul>
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
 * --- Finds the voxel nearest to the given point.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>match: Tiles to search for. ("all"/"empty"/"full")</li>
 * --   <li>point: Position vector. (required)</li></ul>
 * -- @return Tile and vector, or nil.
 * function Voxel.find_voxel(clss, args)
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
 * --- Gets the data of a voxel block.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>index: Block index. (required)</li>
 * --   <li>packet: Packet writer.</li>
 * --   <li>type: Packet type.</li></ul>
 * -- @return Packet writer or nil.
 * function Voxel.get_block(clss, args)
 */
static void Voxel_get_block (LIScrArgs* args)
{
	int tmp;
	int index;
	int type = 1;
	LIExtModule* module;
	LIScrData* data = NULL;
	LIScrPacket* packet;
	LIVoxSector* sector;
	LIVoxBlockAddr addr;

	/* Get block index. */
	if (!liscr_args_gets_int (args, "index", &index))
		return;
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	tmp = index;
	addr.block[0] = tmp % module->voxels->blocks_per_line;
	addr.sector[0] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;
	tmp /= module->voxels->blocks_per_line * module->voxels->sectors->count;
	addr.block[1] = tmp % module->voxels->blocks_per_line;
	addr.sector[1] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;
	tmp /= module->voxels->blocks_per_line * module->voxels->sectors->count;
	addr.block[2] = tmp % module->voxels->blocks_per_line;
	addr.sector[2] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;

	/* Get block. */
	sector = lialg_sectors_data_offset (module->program->sectors, "voxel",
		addr.sector[0], addr.sector[1], addr.sector[2], 0);
	if (sector == NULL)
		return;

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
		!livox_sector_write_block (sector, addr.block[0], addr.block[1], addr.block[2], packet->writer))
	{
		liscr_data_unref (data, NULL);
		return;
	}
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Gets the material configuration of the voxel system.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>type: Packet type.</li></ul>
 * -- @return Packet writer.
 * function Voxel.get_materials(clss, args)
 */
static void Voxel_get_materials (LIScrArgs* args)
{
	int type = 1;
	LIExtModule* module;
	LIScrData* data;
	LIScrPacket* packet;

	/* Create packet. */
	liscr_args_gets_int (args, "type", &type);
	data = liscr_packet_new_writable (args->script, type);
	if (data == NULL)
		return;
	packet = data->data;

	/* Build packet. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	livox_manager_write_materials (module->voxels, packet->writer);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Inserts a voxel near the given point.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>point: Position vector. (required)</li>
 * --   <li>tile: Tile. (required)</li></ul>
 * -- @return True if terrain was filled.
 * function Voxel.insert(clss, args)
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
 * --- Replaces the voxel near the given point.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>point: Position vector. (required)</li>
 * --   <li>tile: Tile. (required)</li></ul>
 * -- @return True if terrain was replaced.
 * function Voxel.replace(clss, args)
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
 * --- Rotates a voxel near the given point.
 * --
 * -- @param clss Voxel class.
 * -- @param args Argumennts.<ul>
 * --   <li>axis: Axis of rotation ("x"/"y"/"z" or 1/2/3)</li>
 * --   <li>point: Position vector. (required)</li>
 * --   <li>step: Number of steps to rotate.</li></ul>
 * -- @return True if a voxel was rotated.
 * function Voxel.rotate(clss, args)
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
 * --- Saves the terrain of the currently loaded sectors.
 * --
 * -- @param clss Voxel class.
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
		if (!liext_tiles_write (module, server->sql))
			lisys_error_report ();
	}
}

/* @luadoc
 * --- Sets the contents of a voxel block.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>packet: Packet reader.</li></ul>
 * -- @return True on success.
 * Voxel.set_block(clss, args)
 */
static void Voxel_set_block (LIScrArgs* args)
{
	uint8_t skip;
	uint32_t index;
	uint32_t tmp;
	LIExtModule* module;
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
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	tmp = index;
	addr.block[0] = tmp % module->voxels->blocks_per_line;
	addr.sector[0] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;
	tmp /= module->voxels->blocks_per_line * module->voxels->sectors->count;
	addr.block[1] = tmp % module->voxels->blocks_per_line;
	addr.sector[1] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;
	tmp /= module->voxels->blocks_per_line * module->voxels->sectors->count;
	addr.block[2] = tmp % module->voxels->blocks_per_line;
	addr.sector[2] = tmp / module->voxels->blocks_per_line % module->voxels->sectors->count;

	/* Find or create sector. */
	sector = lialg_sectors_data_offset (module->voxels->sectors, "voxel",
		addr.sector[0], addr.sector[1], addr.sector[2], 1);
	if (sector == NULL)
		return;

	/* Read block data. */
	if (!livox_sector_read_block (sector, addr.block[0], addr.block[1], addr.block[2], packet->reader))
		return;

	/* Indicate success. */
	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * --- Sets the materials used by the voxel system.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>1,packet: Packet reader.</li></li>
 * -- @return True on success.
 * Voxel.set_materials(clss, args)
 */
static void Voxel_set_materials (LIScrArgs* args)
{
	LIArcReader* reader;
	LIExtModule* module;
	LIScrData* data;
	LIScrPacket* packet;

	/* Get packet. */
	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_PACKET, &data) &&
	    !liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
		return;
	packet = data->data;

	/* Get reader. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (packet->writer)
	{
		reader = liarc_reader_new (
			liarc_writer_get_buffer (packet->writer),
			liarc_writer_get_length (packet->writer));
		if (reader == NULL)
			return;
	}
	else
		reader = packet->reader;

	/* Read materials. */
	liscr_args_seti_bool (args, liext_tiles_set_materials (module, reader));
	if (packet->writer)
		liarc_reader_free (reader);
}

/* @luadoc
 * --- Number of blocks per sector edge.
 * -- @name Voxel.blocks_per_line
 * -- @class table
 */
static void Voxel_getter_blocks_per_line (LIScrArgs* args)
{
	LIExtModule* self;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	liscr_args_seti_int (args, self->voxels->blocks_per_line);
}
static void Voxel_setter_blocks_per_line (LIScrArgs* args)
{
	int count;
	LIExtModule* self;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (liscr_args_geti_int (args, 0, &count))
	{
		if (!livox_manager_configure (self->voxels, count, self->voxels->tiles_per_line))
			lisys_error_report ();
	}
}

/* @luadoc
 * --- Fill type for empty sectors.
 * -- @name Voxel.fill
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

/* @luadoc
 * --- Number of tiles per sector edge.
 * -- @name Voxel.tiles_per_line
 * -- @class table
 */
static void Voxel_getter_tiles_per_line (LIScrArgs* args)
{
	LIExtModule* self;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	liscr_args_seti_int (args, self->voxels->tiles_per_line);
}
static void Voxel_setter_tiles_per_line (LIScrArgs* args)
{
	int count;
	LIExtModule* self;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (liscr_args_geti_int (args, 0, &count))
	{
		if (!livox_manager_configure (self->voxels, self->voxels->blocks_per_line, count))
			lisys_error_report ();
	}
}

/*****************************************************************************/

void
liext_script_material (LIScrClass* self,
                       void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_MATERIAL, data);
	liscr_class_inherit (self, liscr_script_class, NULL);
	liscr_class_insert_mvar (self, "friction", Material_getter_friction, NULL);
	liscr_class_insert_mvar (self, "name", Material_getter_name, NULL);
	liscr_class_insert_mvar (self, "model", Material_getter_model, NULL);
}

void
liext_script_tile (LIScrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_TILE, data);
	liscr_class_inherit (self, liscr_script_class, NULL);
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
	liscr_class_inherit (self, liscr_script_class, NULL);
	liscr_class_insert_cfunc (self, "copy_region", Voxel_copy_region);
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
	liscr_class_insert_cvar (self, "blocks_per_line", Voxel_getter_blocks_per_line, Voxel_setter_blocks_per_line);
	liscr_class_insert_cvar (self, "fill", Voxel_getter_fill, Voxel_setter_fill);
	liscr_class_insert_cvar (self, "tiles_per_line", Voxel_getter_tiles_per_line, Voxel_setter_tiles_per_line);
}

/** @} */
/** @} */
