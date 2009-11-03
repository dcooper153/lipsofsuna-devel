/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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

#include <client/lips-client.h>
#include "ext-module.h"

static int
private_block_free (liextModule*      self,
                    livoxUpdateEvent* event);

static int
private_block_load (liextModule*      self,
                    livoxUpdateEvent* event);

static int
private_packet (liextModule* self,
                licliModule* module,
                liarcReader* reader);

static int
private_tick (liextModule* self,
              float        secs);

static int
private_voxel_assign (liextModule* self,
                      liarcReader* reader);

static int
private_voxel_diff (liextModule* self,
                    liarcReader* reader);

/*****************************************************************************/

licliExtensionInfo liextInfo =
{
	LICLI_EXTENSION_VERSION, "Voxel",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliModule* module)
{
	liextModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->module = module;

	/* Allocate voxel manager. */
	self->voxels = livox_manager_new ();
	if (self->voxels == NULL)
	{
		liext_module_free (self);
		return NULL;
	}

	/* Allocate blocks. */
	self->blocks = lialg_memdic_new ();
	if (self->blocks == NULL)
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register module callbacks. */
	if (!lieng_engine_insert_call (module->engine, LICLI_CALLBACK_PACKET, 1,
	     	private_packet, self, self->calls + 0) ||
	    !lieng_engine_insert_call (module->engine, LICLI_CALLBACK_TICK, 1,
	     	private_tick, self, self->calls + 1))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register voxel callbacks. */
	if (!lical_callbacks_insert_callback (self->voxels->callbacks, LIVOX_CALLBACK_FREE_BLOCK, 1,
	     	private_block_free, self, self->calls1 + 0) ||
	    !lical_callbacks_insert_callback (self->voxels->callbacks, LIVOX_CALLBACK_LOAD_BLOCK, 1,
	     	private_block_load, self, self->calls1 + 1))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register class. */
	liscr_script_create_class (module->script, "Voxel", liextVoxelScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	lialgMemdicIter iter;

	/* FIXME: Remove the class here. */

	/* Free callbacks. */
	lieng_engine_remove_calls (self->module->engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));
	lical_callbacks_remove_callbacks (self->voxels->callbacks, self->calls1,
		sizeof (self->calls1) / sizeof (licalHandle));

	/* Free blocks. */
	if (self->blocks != NULL)
	{
		LI_FOREACH_MEMDIC (iter, self->blocks)
			liext_block_free (iter.value);
		lialg_memdic_free (self->blocks);
	}

	/* Free resources. */
	if (self->voxels != NULL)
		livox_manager_free (self->voxels);
	lisys_free (self);
}

/*****************************************************************************/

static int
private_block_free (liextModule*      self,
                    livoxUpdateEvent* event)
{
	return 1;
}

static int
private_block_load (liextModule*      self,
                    livoxUpdateEvent* event)
{
	liextBlock* eblock;
	liextBlockAddr addr;
	limatVector offset;
	limatVector vector;
	livoxBlock* vblock;
	livoxSector* vsector;

	/* Find sector. */
	vsector = livox_manager_find_sector (self->voxels, LIVOX_SECTOR_INDEX (
		event->sector[0], event->sector[1], event->sector[2]));
	if (vsector == NULL)
		return 1;

	/* Find block. */
	vblock = livox_sector_get_block (vsector, LIVOX_BLOCK_INDEX (
		event->block[0], event->block[1], event->block[2]));
	addr.sector[0] = event->sector[0];
	addr.sector[1] = event->sector[1];
	addr.sector[2] = event->sector[2];
	addr.block[0] = event->block[0];
	addr.block[1] = event->block[1];
	addr.block[2] = event->block[2];
	eblock = lialg_memdic_find (self->blocks, &addr, sizeof (addr));
	if (eblock == NULL)
	{
		eblock = liext_block_new (self->module);
		if (eblock == NULL)
			return 1;
		if (!lialg_memdic_insert (self->blocks, &addr, sizeof (addr), eblock))
		{
			liext_block_free (eblock);
			return 1;
		}
	}

	/* Build block. */
	vector = limat_vector_init (event->sector[0], event->sector[1], event->sector[2]);
	vector = limat_vector_multiply (vector, LIVOX_SECTOR_WIDTH);
	offset = limat_vector_init (event->block[0], event->block[1], event->block[2]);
	offset = limat_vector_multiply (offset, LIVOX_BLOCK_WIDTH);
	offset = limat_vector_add (offset, vector);
	if (!liext_block_build (eblock, self, vblock, &offset))
	{
		lialg_memdic_remove (self->blocks, &addr, sizeof (addr));
		liext_block_free (eblock);
	}

	return 1;
}

static int
private_packet (liextModule* self,
                licliModule* module,
                liarcReader* reader)
{
	uint8_t type;

	reader->pos = 0;
	if (!liarc_reader_get_uint8 (reader, &type))
		return 1;
	switch (type)
	{
		case LIEXT_VOXEL_PACKET_ASSIGN:
			private_voxel_assign (self, reader);
			break;
		case LIEXT_VOXEL_PACKET_DIFF:
			private_voxel_diff (self, reader);
			break;
	}

	return 1;
}

static int
private_tick (liextModule* self,
              float        secs)
{
	livox_manager_update (self->voxels, secs);

	return 1;
}

static int
private_voxel_assign (liextModule* self,
                      liarcReader* reader)
{
	livoxMaterial* material;

	while (!liarc_reader_check_end (reader))
	{
		material = livox_material_new_from_stream (reader);
		if (material == NULL)
			return 0;
		if (!livox_manager_insert_material (self->voxels, material))
			livox_material_free (material);
	}

	return 1;
}

static int
private_voxel_diff (liextModule* self,
                    liarcReader* reader)
{
	uint8_t sectorx;
	uint8_t sectory;
	uint8_t sectorz;
	uint16_t blockid;
	livoxBlock* block;
	livoxSector* sector;

	while (!liarc_reader_check_end (reader))
	{
		/* Read block offset. */
		if (!liarc_reader_get_uint8 (reader, &sectorx) ||
		    !liarc_reader_get_uint8 (reader, &sectory) ||
		    !liarc_reader_get_uint8 (reader, &sectorz) ||
		    !liarc_reader_get_uint16 (reader, &blockid))
			return 0;
		if (blockid >= LIVOX_BLOCKS_PER_SECTOR)
			return 0;

		/* Find block. */
		sector = livox_manager_create_sector (self->voxels,
			LIENG_SECTOR_INDEX (sectorx, sectory, sectorz));
		if (sector == NULL)
			return 0;
		block = livox_sector_get_block (sector, blockid);

		/* Read block data. */
		if (!livox_block_read (block, self->voxels, reader))
			return 0;
		if (livox_block_get_dirty (block))
			livox_sector_set_dirty (sector, 1);
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
