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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvVoxel Voxel
 * @{
 */

#include <network/lips-network.h>
#include <server/lips-server.h>
#include "ext-listener.h"

/**
 * \brief Creates a new terrain listener.
 *
 * \param module Module.
 * \param object Object.
 * \param radius Listening radius.
 * \return New listener or NULL.
 */
liextListener*
liext_listener_new (liextModule* module,
                    liengObject* object,
                    int          radius)
{
	liextListener* self;

	self = lisys_calloc (1, sizeof (liextListener));
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
liext_listener_free (liextListener* self)
{
	lialgMemdicIter iter;

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
liext_listener_cache (liextListener* self,
                      int            sector,
                      int            block,
                      int            stamp)
{
	liextBlockKey key;
	liextListenerBlock* value;

	key.sector = sector;
	key.block = block;
	value = lialg_memdic_find (self->cache, &key, sizeof (liextBlockKey));
	if (value != NULL)
	{
		value->stamp = stamp;
		return 1;
	}
	value = lisys_calloc (1, sizeof (liextListenerBlock));
	if (value == NULL)
		return 0;
	value->stamp = stamp;
	if (!lialg_memdic_insert (self->cache, &key, sizeof (liextBlockKey), value))
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
 * \return Nonzero on success.
 */
int
liext_listener_update (liextListener* self,
                       float          secs)
{
	int x;
	int y;
	int z;
	int stamp;
	lialgRange sectors;
	lialgRange blocks;
	lialgRange range;
	lialgRangeIter iter0;
	lialgRangeIter iter1;
	liarcWriter* writer = NULL;
	liengObject* object;
	limatTransform transform;
	limatVector min;
	limatVector max;
	limatVector size;
	lisrvClient* client;
	livoxBlock* block;
	livoxSector* sector;

	/* TODO: Early exit if listener has not moved and sectors have not changed. */

	/* Get client. */
	object = self->object;
	assert (object != NULL);
	client = LISRV_OBJECT (object)->client;
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
		sector = livox_manager_find_sector (self->module->voxels, iter0.index);
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
		lisrv_client_send (client, writer, GRAPPLE_RELIABLE);
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
liext_listener_get_cached (const liextListener* self,
                           int                  sector,
                           int                  block,
                           int                  stamp)
{
	liextBlockKey key;
	liextListenerBlock* value;

	key.sector = sector;
	key.block = block;
	value = lialg_memdic_find (self->cache, &key, sizeof (liextBlockKey));
	if (value == NULL || value->stamp != stamp)
		return 0;

	return 1;
}

/** @} */
/** @} */
/** @} */
