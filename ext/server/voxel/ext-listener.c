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

	self = calloc (1, sizeof (liextListener));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->object = object;
	self->radius = radius;
	self->cache = lialg_memdic_new ();
	if (self->cache == NULL)
	{
		free (self);
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
			free (iter.value);
		lialg_memdic_free (self->cache);
	}
	free (self);
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
	liextBlockValue* value;

	key.sector = sector;
	key.block = block;
	value = lialg_memdic_find (self->cache, &key, sizeof (liextBlockKey));
	if (value != NULL)
	{
		value->stamp = stamp;
		return 1;
	}
	value = calloc (1, sizeof (liextBlockValue));
	if (value == NULL)
		return 0;
	value->stamp = stamp;
	if (!lialg_memdic_insert (self->cache, &key, sizeof (liextBlockKey), value))
	{
		free (value);
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
	liarcWriter* writer = NULL;
	liengObject* object;
	liengRange sectors;
	liengRange blocks;
	liengRange range;
	liengRangeIter iter0;
	liengRangeIter iter1;
	liengSector* sector;
	limatTransform transform;
	limatVector min;
	limatVector max;
	limatVector size;
	lisrvClient* client;

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
	sectors = lieng_range_new_from_aabb (&min, &max, LIENG_SECTOR_WIDTH,
		0, LIENG_SECTORS_PER_LINE);
	blocks = lieng_range_new_from_aabb (&min, &max, LIENG_BLOCK_WIDTH,
		0, LIENG_SECTORS_PER_LINE * LIENG_BLOCKS_PER_LINE);

	/* Loop through visible sector. */
	LIENG_FOREACH_RANGE (iter0, sectors)
	{
		/* Get dirty sector. */
		sector = lieng_engine_find_sector (self->module->server->engine, iter0.index);
		if (sector == NULL)
			continue;
		if (!self->moved && !sector->dirty)
			continue;

		/* Calculate visible block range. */
		range.min = 0;
		range.max = LIENG_BLOCKS_PER_LINE;
		range.minx = LI_MAX (blocks.minx - sector->x * LIENG_BLOCKS_PER_LINE, 0);
		range.miny = LI_MAX (blocks.miny - sector->y * LIENG_BLOCKS_PER_LINE, 0);
		range.minz = LI_MAX (blocks.minz - sector->z * LIENG_BLOCKS_PER_LINE, 0);
		range.maxx = LI_MIN (blocks.maxx - sector->x * LIENG_BLOCKS_PER_LINE, LIENG_BLOCKS_PER_LINE) - 1;
		range.maxy = LI_MIN (blocks.maxy - sector->y * LIENG_BLOCKS_PER_LINE, LIENG_BLOCKS_PER_LINE) - 1;
		range.maxz = LI_MIN (blocks.maxz - sector->z * LIENG_BLOCKS_PER_LINE, LIENG_BLOCKS_PER_LINE) - 1;

		/* Loop through visible blocks. */
		LIENG_FOREACH_RANGE (iter1, range)
		{
			/* Get uncached block. */
			if (!self->moved && !sector->blocks[iter1.index].dirty)
				continue;
			if (liext_listener_get_cached (self, iter0.index, iter1.index, sector->blocks[iter1.index].stamp))
				continue;

			/* Write block. */
			if (writer == NULL)
				writer = liarc_writer_new_packet (LIEXT_VOXEL_PACKET_DIFF);
			if (writer == NULL)
				return 0;
			if (!liarc_writer_append_uint8 (writer, sector->x) ||
				!liarc_writer_append_uint8 (writer, sector->y) ||
				!liarc_writer_append_uint8 (writer, sector->z) ||
				!liarc_writer_append_uint16 (writer, iter1.index) ||
				!lieng_block_write (sector->blocks + iter1.index, writer))
			{
				liarc_writer_free (writer);
				return 0;
			}

			/* Cache block. */
			liext_listener_cache (self, iter0.index, iter1.index, sector->blocks[iter1.index].stamp);
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
	liextBlockValue* value;

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
