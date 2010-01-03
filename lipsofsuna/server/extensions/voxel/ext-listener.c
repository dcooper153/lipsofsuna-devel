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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvVoxel Voxel
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/server.h>
#include "ext-listener.h"

/**
 * \brief Creates a new terrain listener.
 *
 * \param module Module.
 * \param object Object.
 * \param radius Listening radius.
 * \return New listener or NULL.
 */
LIExtListener*
liext_listener_new (LIExtModule* module,
                    LIEngObject* object,
                    int          radius)
{
	LIExtListener* self;

	self = lisys_calloc (1, sizeof (LIExtListener));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->object = object;
	self->radius = radius;
	self->cache = lialg_memdic_new ();
	if (self->cache == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the terrain listener.
 *
 * \param self Listener.
 */
void
liext_listener_free (LIExtListener* self)
{
	LIAlgMemdicIter iter;

	if (self->cache != NULL)
	{
		LI_FOREACH_MEMDIC (iter, self->cache)
			lisys_free (iter.value);
		lialg_memdic_free (self->cache);
	}
	lisys_free (self);
}

/**
 * \brief Adds a block to the vision cache.
 *
 * \param self Listener.
 * \param sector Sector index.
 * \param block Block index.
 * \param stamp Block modification stamp.
 * \return Nonzero on success.
 */
int
liext_listener_cache (LIExtListener* self,
                      int            sector,
                      int            block,
                      int            stamp)
{
	LIExtBlockKey key;
	LIExtListenerBlock* value;

	key.sector = sector;
	key.block = block;
	value = lialg_memdic_find (self->cache, &key, sizeof (LIExtBlockKey));
	if (value != NULL)
	{
		value->stamp = stamp;
		return 1;
	}
	value = lisys_calloc (1, sizeof (LIExtListenerBlock));
	if (value == NULL)
		return 0;
	value->stamp = stamp;
	if (!lialg_memdic_insert (self->cache, &key, sizeof (LIExtBlockKey), value))
	{
		lisys_free (value);
		return 0;
	}

	return 1;
}

/**
 * \brief Updates the listener state.
 *
 * Checks the terrain near the listener for changes and sends the changes to
 * the client assigned to the listener. Also takes care of tracking known
 * terrain in order to avoid resending already known terrain blocks.
 *
 * \param self Listener.
 * \param secs Number of seconds since the last update.
 * \return Nonzero on success.
 */
int
liext_listener_update (LIExtListener* self,
                       float          secs)
{
	int x;
	int y;
	int z;
	int stamp;
	LIAlgRange sectors;
	LIAlgRange blocks;
	LIAlgRange range;
	LIAlgRangeIter iter0;
	LIAlgRangeIter iter1;
	LIArcWriter* writer = NULL;
	LIEngObject* object;
	LIMatTransform transform;
	LIMatVector min;
	LIMatVector max;
	LIMatVector size;
	LISerClient* client;
	LIVoxBlock* block;
	LIVoxSector* sector;

	/* TODO: Early exit if listener has not moved and sectors have not changed. */

	/* Get client. */
	object = self->object;
	assert (object != NULL);
	client = LISER_OBJECT (object)->client;
	assert (client != NULL);

	/* Calculate sight volume. */
	lieng_object_get_transform (object, &transform);
	size = limat_vector_init (self->radius, self->radius, self->radius);
	min = limat_vector_subtract (transform.position, size);
	max = limat_vector_add (transform.position, size);
	sectors = lialg_range_new_from_aabb (&min, &max, LIVOX_SECTOR_WIDTH);
	sectors = lialg_range_clamp (sectors, 0, LIVOX_SECTORS_PER_LINE - 1);
	blocks = lialg_range_new_from_aabb (&min, &max, LIVOX_BLOCK_WIDTH);
	blocks = lialg_range_clamp (blocks, 0, LIVOX_SECTORS_PER_LINE * LIVOX_BLOCKS_PER_LINE - 1);

	/* Loop through visible sector. */
	LIALG_RANGE_FOREACH (iter0, sectors)
	{
		/* Get dirty sector. */
		sector = lialg_sectors_data_index (self->module->voxels->sectors, "voxel", iter0.index, 0);
		if (sector == NULL)
			continue;
		if (!self->moved && !livox_sector_get_dirty (sector))
			continue;

		/* Calculate visible block range. */
		livox_sector_get_offset (sector, &x, &y, &z);
		range.min = 0;
		range.max = LIVOX_BLOCKS_PER_LINE;
		range.minx = LI_MAX (blocks.minx - x * LIVOX_BLOCKS_PER_LINE, 0);
		range.miny = LI_MAX (blocks.miny - y * LIVOX_BLOCKS_PER_LINE, 0);
		range.minz = LI_MAX (blocks.minz - z * LIVOX_BLOCKS_PER_LINE, 0);
		range.maxx = LI_MIN (blocks.maxx - x * LIVOX_BLOCKS_PER_LINE, LIVOX_BLOCKS_PER_LINE - 1);
		range.maxy = LI_MIN (blocks.maxy - y * LIVOX_BLOCKS_PER_LINE, LIVOX_BLOCKS_PER_LINE - 1);
		range.maxz = LI_MIN (blocks.maxz - z * LIVOX_BLOCKS_PER_LINE, LIVOX_BLOCKS_PER_LINE - 1);

		/* Loop through visible blocks. */
		LIALG_RANGE_FOREACH (iter1, range)
		{
			block = livox_sector_get_block (sector, iter1.index);
			stamp = livox_block_get_stamp (block);

			/* Check if alreach cached. */
			if (!self->moved && !livox_block_get_dirty (block))
				continue;
			if (liext_listener_get_cached (self, iter0.index, iter1.index, stamp))
				continue;

			/* Write block. */
			if (writer == NULL)
				writer = liarc_writer_new_packet (LIEXT_VOXEL_PACKET_DIFF);
			if (writer == NULL)
				return 0;
			if (!liarc_writer_append_uint8 (writer, x) ||
				!liarc_writer_append_uint8 (writer, y) ||
				!liarc_writer_append_uint8 (writer, z) ||
				!liarc_writer_append_uint16 (writer, iter1.index) ||
				!livox_block_write (block, writer))
			{
				liarc_writer_free (writer);
				return 0;
			}

			/* Cache block. */
			liext_listener_cache (self, iter0.index, iter1.index, stamp);
		}
	}

	/* Send packet. */
	if (writer != NULL)
	{
		liser_client_send (client, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}

	return 1;
}

/**
 * \brief Checks if a block is in the vision cache.
 *
 * \param self Listener.
 * \param sector Sector index.
 * \param block Block index.
 * \param stamp Block modification stamp.
 * \return Nonzero if in cache.
 */
int
liext_listener_get_cached (const LIExtListener* self,
                           int                  sector,
                           int                  block,
                           int                  stamp)
{
	LIExtBlockKey key;
	LIExtListenerBlock* value;

	key.sector = sector;
	key.block = block;
	value = lialg_memdic_find (self->cache, &key, sizeof (LIExtBlockKey));
	if (value == NULL || value->stamp != stamp)
		return 0;

	return 1;
}

/** @} */
/** @} */
/** @} */
