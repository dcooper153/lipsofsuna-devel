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
 * \addtogroup LIExtTiles Tiles
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "core/tiles"
 * --- Use dynamic voxel terrain.
 * -- @name Voxel
 * -- @class table
 */

/* @luadoc
 * --- Copies a terrain region into a packet.
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>point: Tile index vector.</li>
 * --   <li>sector: Sector index.</li>
 * --   <li>size: Region size, in tiles.</li></ul>
 * -- @return Packet writer.
 * function Voxel.copy_region(clss, args)
 */
static void Voxel_copy_region (LIScrArgs* args)
{
	int i;
	int length;
	int sector;
	int offset[3];
	LIArcWriter* writer;
	LIExtModule* module;
	LIMatVector point;
	LIMatVector size;
	LIScrData* packet;
	LIVoxVoxel* result;

	/* Get region offset and size. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (liscr_args_gets_int (args, "sector", &sector))
	{
		lialg_sectors_index_to_offset (module->program->sectors, sector,
			offset + 0, offset + 1, offset + 2);
		point = limat_vector_init (offset[0], offset[1], offset[2]);
		point = limat_vector_multiply (point, module->voxels->tiles_per_line);
		size.x = size.y = size.z = module->voxels->tiles_per_line;
		length = module->voxels->tiles_per_sector;
	}
	else if (liscr_args_gets_vector (args, "point", &point) &&
	         liscr_args_gets_vector (args, "size", &size))
	{
		if (point.x < 0.0f || point.y < 0.0f || point.z < 0.0f ||
		    size.x < 1.0f || size.y < 1.0f || size.z < 1.0f)
			return;
		length = (int) size.x * (int) size.y * (int) size.z;
	}
	else
		return;

	/* Read voxel data. */
	result = lisys_calloc (length, sizeof (LIVoxVoxel));
	if (result == NULL)
		return;
	livox_manager_copy_voxels (module->voxels,
		(int) point.x, (int) point.y, (int) point.z,
		(int) size.x, (int) size.y, (int) size.z, result);

	/* Create packet writer. */
	packet = liscr_packet_new_writable (args->script, 0);
	if (packet == NULL)
	{
		lisys_free (result);
		return;
	}
	writer = ((LIScrPacket*) liscr_data_get_data (packet))->writer;

	/* Write dimensions. */
	if (!liarc_writer_append_uint32 (writer, (int) size.x) ||
		!liarc_writer_append_uint32 (writer, (int) size.y) ||
		!liarc_writer_append_uint32 (writer, (int) size.z))
	{
		lisys_free (result);
		return;
	}

	/* Write voxel data. */
	for (i = 0 ; i < length ; i++)
	{
		if (!livox_voxel_write (result + i, writer))
		{
			lisys_free (result);
			return;
		}
	}

	/* Return data. */
	liscr_args_seti_data (args, packet);
	lisys_free (result);
}

/* @luadoc
 * --- Fills a terrain region.
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>point: Tile index vector. (required)</li>
 * --   <li>size: Size vector. (required)</li>
 * --   <li>tile: Tile data.</ul>
 * function Voxel.fill_region(clss, args)
 */
static void Voxel_fill_region (LIScrArgs* args)
{
	int i;
	int count;
	int type = 0;
	LIExtModule* module;
	LIMatVector pos;
	LIMatVector size;
	LIVoxVoxel tile;
	LIVoxVoxel* tiles;

	/* Handle arguments. */
	if (!liscr_args_gets_vector (args, "point", &pos) ||
	    !liscr_args_gets_vector (args, "size", &size))
		return;
	if (size.x < 1.0f || size.y < 1.0f || size.z < 1.0f)
		return;
	liscr_args_gets_int (args, "tile", &type);
	livox_voxel_init (&tile, type);

	/* Allocate tiles. */
	count = (int) size.x * (int) size.y * (int) size.z;
	tiles = lisys_calloc (count, sizeof (LIVoxVoxel));
	if (tiles == NULL)
		return;
	for (i = 0 ; i < count ; i++)
		tiles[i] = tile;

	/* Paste tiles to the map. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	livox_manager_paste_voxels (module->voxels,
		(int) pos.x, (int) pos.y, (int) pos.z,
		(int) size.x, (int) size.y, (int) size.z, tiles);
	lisys_free (tiles);
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
	LIScrClass* clss;
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
		clss = liscr_script_find_class (args->script, LIEXT_SCRIPT_MATERIAL);
		data = liscr_data_new (args->script, material, clss, livox_material_free);
		if (data == NULL)
		{
			livox_material_free (material);
			return;
		}
		liscr_args_seti_data (args, data);
		liscr_data_unref (data);
	}
}

/* @luadoc
 * --- Finds the tile nearest to the given point.
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>match: Tiles to search for. ("all"/"empty"/"full")</li>
 * --   <li>point: Position vector in world space. (required)</li></ul>
 * -- @return Tile and voxel index vector, or nil.
 * function Voxel.find_tile(clss, args)
 */
static void Voxel_find_tile (LIScrArgs* args)
{
	int index[3];
	int flags = LIVOX_FIND_ALL;
	float radius = 0.0f;
	const char* tmp;
	LIExtModule* module;
	LIMatVector point;
	LIMatVector result;
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

		/* Search radius. */
		liscr_args_gets_float (args, "radius", &radius);
		radius = LIMAT_MAX (0.0f, radius);

		/* Find voxel. */
		voxel = livox_manager_find_voxel (module->voxels, flags, &point, radius, index);
		if (voxel == NULL)
			return;

		/* Return values. */
		liscr_args_seti_int (args, voxel->type);
		result = limat_vector_init (index[0], index[1], index[2]);
		liscr_args_seti_vector (args, &result);
	}
}

/* @luadoc
 * --- Gets the data of a voxel block.
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
		packet = liscr_data_get_data (data);
	}
	else
	{
		packet = liscr_data_get_data (data);
		if (packet->writer == NULL)
			return;
		liscr_data_ref (data);
	}

	/* Build packet. */
	if (!liarc_writer_append_uint32 (packet->writer, index) ||
		!livox_sector_write_block (sector, addr.block[0], addr.block[1], addr.block[2], packet->writer))
	{
		liscr_data_unref (data);
		return;
	}
	liscr_args_seti_data (args, data);
	liscr_data_unref (data);
}

/* @luadoc
 * --- Gets the contents of a tile.
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>1,point: Tile index vector. (required)</li></ul>
 * -- @return Tile.
 * function Voxel.get_tile(clss, args)
 */
static void Voxel_get_tile (LIScrArgs* args)
{
	int lim;
	LIExtModule* module;
	LIMatVector point;
	LIVoxVoxel voxel;

	if (!liscr_args_geti_vector (args, 0, &point) &&
	    !liscr_args_gets_vector (args, "point", &point))
		return;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	lim = module->voxels->tiles_per_line * module->program->sectors->count;
	if (point.x < 0.0f || point.x >= lim ||
	    point.y < 0.0f || point.y >= lim ||
	    point.z < 0.0f || point.z >= lim)
		return;
	livox_manager_get_voxel (module->voxels, (int) point.x, (int) point.y, (int) point.z, &voxel);
	liscr_args_seti_int (args, voxel.type);
}

/* @luadoc
 * --- Intersects a ray with map tiles.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>1,start_point: Ray start point in world space.</li>
 * --   <li>2,end_point: Ray end point in world space.</li></ul>
 * -- @return Position vector in world space, tile index vector.
 * function Voxel.intersect_ray(self, args)
 */
static void Voxel_intersect_ray (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector ray0;
	LIMatVector ray1;
	LIMatVector point;
	LIMatVector tile;

	/* Get arguments. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (!liscr_args_geti_vector (args, 0, &ray0) &&
	    !liscr_args_gets_vector (args, "start_point", &ray0))
		return;
	if (!liscr_args_geti_vector (args, 1, &ray1) &&
	    !liscr_args_gets_vector (args, "end_point", &ray1))
		return;

	/* Intersect with terrain. */
	ray0 = limat_vector_multiply (ray0, 1.0f / module->voxels->tile_width);
	ray1 = limat_vector_multiply (ray1, 1.0f / module->voxels->tile_width);
	if (livox_manager_intersect_ray (module->voxels, &ray0, &ray1, &point, &tile))
	{
		point = limat_vector_multiply (point, module->voxels->tile_width);
		liscr_args_seti_vector (args, &point);
		liscr_args_seti_vector (args, &tile);
	}
}

/* @luadoc
 * --- Pastes a terrain region from a packet to the map.
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>packet: Data packet.</li>
 * --   <li>point: Tile index vector.</li>
 * --   <li>sector: Sector index.</li></ul>
 * function Voxel.paste_region(clss, args)
 */
static void Voxel_paste_region (LIScrArgs* args)
{
	int i;
	int length;
	int sector;
	int offset[3];
	uint8_t tmp;
	uint32_t size[3];
	LIArcReader* reader;
	LIExtModule* module;
	LIMatVector point;
	LIScrData* data;
	LIScrPacket* packet;
	LIVoxVoxel* voxels;

	/* Get region offset. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (liscr_args_gets_int (args, "sector", &sector))
	{
		lialg_sectors_index_to_offset (module->program->sectors, sector,
			offset + 0, offset + 1, offset + 2);
		point = limat_vector_init (offset[0], offset[1], offset[2]);
		point = limat_vector_multiply (point, module->voxels->tiles_per_line);
	}
	else if (liscr_args_gets_vector (args, "point", &point))
	{
		if (point.x < 0.0f || point.y < 0.0f || point.z < 0.0f)
			return;
	}
	else
		return;

	/* Get terrain data. */
	if (!liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);
	if (packet->reader == NULL)
	{
		reader = liarc_reader_new (
			liarc_writer_get_buffer (packet->writer),
			liarc_writer_get_length (packet->writer));
		if (reader == NULL)
			return;
	}
	else
		reader = packet->reader;

	/* Read dimensions. */
	if (!liarc_reader_get_uint8 (reader, &tmp) ||
	    !liarc_reader_get_uint32 (reader, size + 0) ||
	    !liarc_reader_get_uint32 (reader, size + 1) ||
	    !liarc_reader_get_uint32 (reader, size + 2))
	{
		if (packet->reader == NULL)
			liarc_reader_free (reader);
		return;
	}
	length = size[0] * size[1] * size[2];
	if (!length)
	{
		if (packet->reader == NULL)
			liarc_reader_free (reader);
		return;
	}

	/* Allocate space for voxel data. */
	voxels = lisys_calloc (length, sizeof (LIVoxVoxel));
	if (voxels == NULL)
	{
		if (packet->reader == NULL)
			liarc_reader_free (reader);
		return;
	}

	/* Read voxel data. */
	for (i = 0 ; i < length ; i++)
	{
		if (!livox_voxel_read (voxels + i, reader))
		{
			if (packet->reader == NULL)
				liarc_reader_free (reader);
			lisys_free (voxels);
			return;
		}
	}

	/* Paste voxel data to the map. */
	livox_manager_paste_voxels (module->voxels,
		(int) point.x, (int) point.y, (int) point.z,
		size[0], size[1], size[2], voxels);
	if (packet->reader == NULL)
		liarc_reader_free (reader);
	lisys_free (voxels);
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
	packet = liscr_data_get_data (data);
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
 * --- Sets the contents of a tile.
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>1,point: Tile index vector.</li>
 * --   <li>2,tile: Tile number.</li></ul>
 * function Voxel.set_tile(clss, args)
 */
static void Voxel_set_tile (LIScrArgs* args)
{
	int lim;
	int type;
	LIExtModule* module;
	LIMatVector point;
	LIVoxVoxel voxel;

	if (!liscr_args_geti_vector (args, 0, &point) &&
	    !liscr_args_gets_vector (args, "point", &point))
		return;
	if (!liscr_args_geti_int (args, 1, &type) &&
	    !liscr_args_gets_int (args, "tile", &type))
		return;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	lim = module->voxels->tiles_per_line * module->program->sectors->count;
	if (point.x < 0.0f || point.x >= lim ||
	    point.y < 0.0f || point.y >= lim ||
	    point.z < 0.0f || point.z >= lim)
		return;
	livox_voxel_init (&voxel, type);
	livox_manager_set_voxel (module->voxels, (int) point.x, (int) point.y, (int) point.z, &voxel);
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

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	if (self->voxels->fill)
		liscr_args_seti_int (args, self->voxels->fill);
}
static void Voxel_setter_fill (LIScrArgs* args)
{
	int type = 0;
	LIExtModule* self;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	liscr_args_geti_int (args, 0, &type);
	livox_manager_set_fill (self->voxels, type);
}

/* @luadoc
 * --- List of material IDs.
 * -- @name Voxel.materials
 * -- @class table
 */
static void Voxel_getter_materials (LIScrArgs* args)
{
	LIAlgU32dicIter iter;
	LIExtModule* module;
	LIVoxMaterial* material;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	LIALG_U32DIC_FOREACH (iter, module->voxels->materials)
	{
		material = iter.value;
		liscr_args_seti_int (args, material->id);
	}
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

void liext_script_voxel (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_VOXEL, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "copy_region", Voxel_copy_region);
	liscr_class_insert_cfunc (self, "fill_region", Voxel_fill_region);
	liscr_class_insert_cfunc (self, "find_blocks", Voxel_find_blocks);
	liscr_class_insert_cfunc (self, "find_material", Voxel_find_material);
	liscr_class_insert_cfunc (self, "find_tile", Voxel_find_tile);
	liscr_class_insert_cfunc (self, "get_block", Voxel_get_block);
	liscr_class_insert_cfunc (self, "get_tile", Voxel_get_tile);
	liscr_class_insert_cfunc (self, "intersect_ray", Voxel_intersect_ray);
	liscr_class_insert_cfunc (self, "paste_region", Voxel_paste_region);
	liscr_class_insert_cfunc (self, "set_block", Voxel_set_block);
	liscr_class_insert_cfunc (self, "set_tile", Voxel_set_tile);
	liscr_class_insert_cvar (self, "blocks_per_line", Voxel_getter_blocks_per_line, Voxel_setter_blocks_per_line);
	liscr_class_insert_cvar (self, "fill", Voxel_getter_fill, Voxel_setter_fill);
	liscr_class_insert_cvar (self, "materials", Voxel_getter_materials, NULL);
	liscr_class_insert_cvar (self, "tiles_per_line", Voxel_getter_tiles_per_line, Voxel_setter_tiles_per_line);
}

/** @} */
/** @} */
