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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliVoxel Voxel
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Voxel"
 * --- Display voxel terrain.
 * -- @name Voxel
 * -- @class table
 */

/* @luadoc
 * --- Sets the contents of a voxel block.
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>packet: Packet reader.</li></ul>
 * -- @return True on success.
 * function Voxel.set_block(self, args)
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
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VOXEL);
	tmp = index;
	addr.block[0] = tmp % LIVOX_BLOCKS_PER_LINE;
	addr.sector[0] = tmp / LIVOX_BLOCKS_PER_LINE % module->voxels->sectors->count;
	tmp /= LIVOX_BLOCKS_PER_LINE * module->voxels->sectors->count;
	addr.block[1] = tmp % LIVOX_BLOCKS_PER_LINE;
	addr.sector[1] = tmp / LIVOX_BLOCKS_PER_LINE % module->voxels->sectors->count;
	tmp /= LIVOX_BLOCKS_PER_LINE * module->voxels->sectors->count;
	addr.block[2] = tmp % LIVOX_BLOCKS_PER_LINE;
	addr.sector[2] = tmp / LIVOX_BLOCKS_PER_LINE % module->voxels->sectors->count;

	/* Find or create sector. */
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
 * --- Sets the materials used by the voxel system.
 * --
 * -- @param self Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>packet: Packet reader.</li></ul>
 * -- @return True on success.
 * function Voxel.set_materials(self, args)
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

/*****************************************************************************/

void
liext_script_voxel (LIScrClass* self,
                    void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_VOXEL, data);
	liscr_class_insert_cfunc (self, "set_block", Voxel_set_block);
	liscr_class_insert_cfunc (self, "set_materials", Voxel_set_materials);
}

/** @} */
/** @} */
/** @} */
