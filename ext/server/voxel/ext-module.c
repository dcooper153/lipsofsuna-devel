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
#include "ext-voxel.h"
#include "ext-module.h"

#define LISTENER_POSITION_EPSILON 3.0f

static int
private_object_motion (liextModule* self,
                       liengObject* object);

static int
private_object_visibility (liextModule* self,
                           liengObject* object,
                           int          visible);

static int
private_pack_block (liextModule* self,
                    liarcWriter* writer,
                    int          x,
                    int          y,
                    int          z);

/*****************************************************************************/

lisrvExtensionInfo liextInfo =
{
	LISRV_EXTENSION_VERSION, "Voxel",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (lisrvServer* server)
{
	liextModule* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->radius = 20.0f;
	self->server = server;
	self->listeners = lialg_ptrdic_new ();
	if (self->listeners == NULL)
	{
		free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_MOTION, 1,
	     	private_object_motion, self, self->calls + 0) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_VISIBILITY, 1,
	     	private_object_visibility, self, self->calls + 1))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_insert_class (server->script, "Voxel", liextVoxelScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	lialgPtrdicIter iter;

	/* FIXME: Remove the class here. */

	if (self->listeners != NULL)
	{
		LI_FOREACH_PTRDIC (iter, self->listeners)
			liext_voxel_free (iter.value);
		lialg_ptrdic_free (self->listeners);
	}

	lieng_engine_remove_calls (self->server->engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));
	free (self);
}

void
liext_module_fill_box (liextModule*       self,
                       const limatVector* min,
                       const limatVector* max,
                       liengTile          terrain)
{
	lialgU32dicIter iter1;
	liarcWriter* writer;
	liengRange range;
	liengRangeIter rangeiter;
	liengSector* sector;
	limatAabb aabb;

	/* Modify sectors. */
	range = lieng_range_new_from_aabb (min, max, LIENG_SECTOR_WIDTH, 0, 256);
	LIENG_FOREACH_RANGE (rangeiter, range)
	{
		sector = lieng_engine_load_sector (self->server->engine, rangeiter.index);
		if (sector == NULL)
			continue;
		aabb.min = limat_vector_subtract (*min, sector->origin);
		aabb.max = limat_vector_subtract (*max, sector->origin);
		lieng_sector_fill_aabb (sector, &aabb, terrain);
	}

	/* Notify clients. */
	writer = liarc_writer_new_packet (LIEXT_VOXEL_PACKET_BOX);
	if (writer != NULL)
	{
		liarc_writer_append_float (writer, min->x);
		liarc_writer_append_float (writer, min->y);
		liarc_writer_append_float (writer, min->z);
		liarc_writer_append_float (writer, max->x);
		liarc_writer_append_float (writer, max->y);
		liarc_writer_append_float (writer, max->z);
		liarc_writer_append_uint32 (writer, terrain);
		LI_FOREACH_U32DIC (iter1, self->server->network->clients)
		{
#warning FIXME: Terrain edit events are sent to all clients.
			lisrv_client_send (iter1.value, writer, GRAPPLE_RELIABLE);
		}
	}
}

void
liext_module_fill_sphere (liextModule*       self,
                          const limatVector* center,
                          float              radius,
                          liengTile          terrain)
{
	lialgU32dicIter iter1;
	liarcWriter* writer;
	liengRange range;
	liengRangeIter rangeiter;
	liengSector* sector;
	limatVector vector;

	/* Modify sectors. */
	range = lieng_range_new_from_sphere (center, radius, LIENG_SECTOR_WIDTH, 0, 256);
	LIENG_FOREACH_RANGE (rangeiter, range)
	{
		sector = lieng_engine_load_sector (self->server->engine, rangeiter.index);
		if (sector == NULL)
			continue;
		vector = limat_vector_subtract (*center, sector->origin);
		lieng_sector_fill_sphere (sector, &vector, radius, terrain);
	}

	/* Notify clients. */
	writer = liarc_writer_new_packet (LIEXT_VOXEL_PACKET_SPHERE);
	if (writer != NULL)
	{
		liarc_writer_append_float (writer, center->x);
		liarc_writer_append_float (writer, center->y);
		liarc_writer_append_float (writer, center->z);
		liarc_writer_append_float (writer, radius);
		liarc_writer_append_uint32 (writer, terrain);
		LI_FOREACH_U32DIC (iter1, self->server->network->clients)
		{
#warning FIXME: Terrain edit events are sent to all clients.
			lisrv_client_send (iter1.value, writer, GRAPPLE_RELIABLE);
		}
	}
}

/*****************************************************************************/

static int
private_object_motion (liextModule* self,
                       liengObject* object)
{
	int radius;
	liarcWriter* writer;
	liextVoxel* listener;
	limatTransform transform;
	struct { int x, y, z; } center, pos;

	if (LISRV_OBJECT (object)->client == NULL)
		return 1;

	/* Find terrain vision subscription. */
	listener = lialg_ptrdic_find (self->listeners, object);
	if (listener == NULL)
		return 1;

	/* Calculate vision volume. */
	lieng_object_get_transform (object, &transform);
	radius = (int)(self->radius / LIENG_BLOCK_WIDTH + 0.5f);
	center.x = transform.position.x / LIENG_BLOCK_WIDTH;
	center.y = transform.position.y / LIENG_BLOCK_WIDTH;
	center.z = transform.position.z / LIENG_BLOCK_WIDTH;

	/* Check for position change. */
	if (center.x == listener->x &&
	    center.y == listener->y &&
	    center.z == listener->z)
		return 1;
	writer = NULL;

	/* Send newly seen terrain. */
	for (pos.z = center.z - radius ; pos.z <= center.z + radius ; pos.z++)
	for (pos.y = center.y - radius ; pos.y <= center.y + radius ; pos.y++)
	for (pos.x = center.x - radius ; pos.x <= center.x + radius ; pos.x++)
	{
		if (listener->x - listener->radius <= pos.x && pos.x <= listener->x + listener->radius &&
		    listener->y - listener->radius <= pos.y && pos.y <= listener->y + listener->radius &&
		    listener->z - listener->radius <= pos.z && pos.z <= listener->z + listener->radius)
			continue;
		if (writer == NULL)
			writer = liarc_writer_new_packet (LIEXT_VOXEL_PACKET_DIFF);
		if (writer == NULL)
			return 1;
		if (!private_pack_block (self, writer, pos.x, pos.y, pos.z))
		{
			liarc_writer_free (writer);
			return 1;
		}
	}
	if (writer != NULL)
	{
		lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}

	/* Store new vision center. */
	listener->x = center.x;
	listener->y = center.y;
	listener->z = center.z;
	listener->radius = radius;

	return 1;
}

static int
private_object_visibility (liextModule* self,
                           liengObject* object,
                           int          visible)
{
	int radius;
	liarcWriter* writer;
	liextVoxel* listener;
	limatTransform transform;
	struct { int x, y, z; } center, pos;

	if (LISRV_OBJECT (object)->client == NULL)
		return 1;

	if (visible)
	{
		/* Calculate vision volume. */
		lieng_object_get_transform (object, &transform);
		radius = (int)(self->radius / LIENG_BLOCK_WIDTH + 0.5f);
		center.x = transform.position.x / LIENG_BLOCK_WIDTH;
		center.y = transform.position.y / LIENG_BLOCK_WIDTH;
		center.z = transform.position.z / LIENG_BLOCK_WIDTH;

		/* Subscribe to terrain updates. */
		lieng_object_get_transform (object, &transform);
		listener = liext_voxel_new (center.x, center.y, center.z, radius);
		if (listener == NULL)
			return 1;
		if (!lialg_ptrdic_insert (self->listeners, object, listener))
			liext_voxel_free (listener);

		/* Send newly seen terrain. */
		/* FIXME: Not implemented yet! */
		writer = liarc_writer_new_packet (LIEXT_VOXEL_PACKET_DIFF);
		if (writer == NULL)
			return 1;
		for (pos.z = center.z - radius ; pos.z <= center.z + radius ; pos.z++)
		for (pos.y = center.y - radius ; pos.y <= center.y + radius ; pos.y++)
		for (pos.x = center.x - radius ; pos.x <= center.x + radius ; pos.x++)
		{
			if (!private_pack_block (self, writer, pos.x, pos.y, pos.z))
			{
				liarc_writer_free (writer);
				return 1;
			}
		}
		lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}
	else
	{
		/* Unsubscribe from terrain updates. */
		listener = lialg_ptrdic_find (self->listeners, object);
		if (listener == NULL)
			return 1;
		lialg_ptrdic_remove (self->listeners, object);
		liext_voxel_free (listener);
	}

	return 1;
}

static int
private_pack_block (liextModule* self,
                    liarcWriter* writer,
                    int          x,
                    int          y,
                    int          z)
{
	uint32_t id;
	liengBlock* block;
	liengSector* sector;

	/* Find sector. */
	id = LIENG_SECTOR_INDEX (
		x / LIENG_BLOCKS_PER_LINE,
		y / LIENG_BLOCKS_PER_LINE,
		z / LIENG_BLOCKS_PER_LINE);
	sector = lieng_engine_find_sector (self->server->engine, id);
	if (sector == NULL)
		return 0;

	/* Find block. */
	id = LIENG_BLOCK_INDEX (
		x % LIENG_BLOCKS_PER_LINE,
		y % LIENG_BLOCKS_PER_LINE,
		z % LIENG_BLOCKS_PER_LINE);
	block = sector->blocks + id;

	/* Send block data. */
	if (!liarc_writer_append_uint8 (writer, sector->x) ||
	    !liarc_writer_append_uint8 (writer, sector->y) ||
	    !liarc_writer_append_uint8 (writer, sector->z) ||
	    !liarc_writer_append_uint16 (writer, id) ||
	    !lieng_block_write (block, writer))
	{
		liarc_writer_free (writer);
		return 0;
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
