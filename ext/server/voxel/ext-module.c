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
#include "ext-module.h"

#define LISTENER_POSITION_EPSILON 3.0f

static int
private_object_client (liextModule* self,
                       liengObject* object);

static int
private_object_client_login (liextModule* self,
                             liengObject* object);

static int
private_object_motion (liextModule* self,
                       liengObject* object);

static int
private_object_visibility (liextModule* self,
                           liengObject* object,
                           int          visible);

static int
private_sector_load (liextModule* self,
                     liengSector* sector);

static int
private_tick (liextModule* self,
              float        secs);

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
	lialgU32dicIter iter;
	liextModule* self;
	livoxMaterial* material;

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

	/* Create voxel manager. */
	self->voxels = livox_manager_new (server->engine->physics, NULL, NULL);
	if (self->voxels == NULL)
	{
		liext_module_free (self);
		return NULL;
	}
	if (!livox_manager_load_materials (self->voxels, server->sql))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Create assign packet. */
	self->assign_packet = liarc_writer_new_packet (LIEXT_VOXEL_PACKET_ASSIGN);
	if (self->assign_packet == NULL)
	{
		liext_module_free (self);
		return NULL;
	}
	LI_FOREACH_U32DIC (iter, self->voxels->materials)
	{
		material = iter.value;
		if (!livox_material_write_to_stream (material, self->assign_packet))
		{
			liext_module_free (self);
			return NULL;
		}
	}

	/* Register callbacks. */
	if (!lieng_engine_insert_call (server->engine, LISRV_CALLBACK_CLIENT_LOGIN, 1,
	     	private_object_client_login, self, self->calls + 0) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_CLIENT, 1,
	     	private_object_client, self, self->calls + 1) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_MOTION, 1,
	     	private_object_motion, self, self->calls + 2) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_VISIBILITY, 1,
	     	private_object_visibility, self, self->calls + 3) ||
	    !lieng_engine_insert_call (server->engine, LIENG_CALLBACK_SECTOR_LOAD, 0,
	     	private_sector_load, self, self->calls + 4) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_TICK, 0,
	     	private_tick, self, self->calls + 5))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (server->script, "Voxel", liextVoxelScript, self);

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
			liext_listener_free (iter.value);
		lialg_ptrdic_free (self->listeners);
	}
	if (self->voxels != NULL)
		livox_manager_free (self->voxels);
	if (self->assign_packet != NULL)
		liarc_writer_free (self->assign_packet);

	lieng_engine_remove_calls (self->server->engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));
	free (self);
}

int
liext_module_erase_point (liextModule*       self,
                          const limatVector* point)
{
	int x, y, z;
	int index;
	limatVector p;
	livoxSector* sector;

	x = (int)(point->x / LIVOX_SECTOR_WIDTH);
	y = (int)(point->y / LIVOX_SECTOR_WIDTH);
	z = (int)(point->z / LIVOX_SECTOR_WIDTH);
	p.x = point->x - x * LIVOX_SECTOR_WIDTH;
	p.y = point->y - y * LIVOX_SECTOR_WIDTH;
	p.z = point->z - z * LIVOX_SECTOR_WIDTH;

	index = LIVOX_SECTOR_INDEX (x, y, z);
	sector = livox_manager_load_sector (self->voxels, index, self->server->sql);
	if (sector == NULL)
		return 0;

	return livox_sector_erase_point (sector, &p);
}

void
liext_module_fill_box (liextModule*       self,
                       const limatVector* min,
                       const limatVector* max,
                       livoxVoxel         terrain)
{
	liengRange range;
	liengRangeIter rangeiter;
	limatAabb aabb;
	limatVector origin;
	livoxSector* sector;

	/* Modify sectors. */
	range = lieng_range_new_from_aabb (min, max, LIENG_SECTOR_WIDTH, 0, 256);
	LIENG_FOREACH_RANGE (rangeiter, range)
	{
		sector = livox_manager_load_sector (self->voxels, rangeiter.index, self->server->sql);
		if (sector == NULL)
			continue;
		livox_sector_get_origin (sector, &origin);
		aabb.min = limat_vector_subtract (*min, origin);
		aabb.max = limat_vector_subtract (*max, origin);
		livox_sector_fill_aabb (sector, &aabb, terrain);
	}
}

void
liext_module_fill_sphere (liextModule*       self,
                          const limatVector* center,
                          float              radius,
                          livoxVoxel         terrain)
{
	liengRange range;
	liengRangeIter rangeiter;
	limatVector origin;
	limatVector vector;
	livoxSector* sector;

	/* Modify sectors. */
	range = lieng_range_new_from_sphere (center, radius, LIENG_SECTOR_WIDTH, 0, 256);
	LIENG_FOREACH_RANGE (rangeiter, range)
	{
		sector = livox_manager_load_sector (self->voxels, rangeiter.index, self->server->sql);
		if (sector == NULL)
			continue;
		livox_sector_get_origin (sector, &origin);
		vector = limat_vector_subtract (*center, origin);
		livox_sector_fill_sphere (sector, &vector, radius, terrain);
	}
}

int
liext_module_write (liextModule* self,
                    liarcSql*    sql)
{
	return livox_manager_write (self->voxels, sql);
}

/*****************************************************************************/

static int
private_object_client (liextModule* self,
                       liengObject* object)
{
	liextListener* listener;

	/* Unsubscribe from terrain updates. */
	listener = lialg_ptrdic_find (self->listeners, object);
	if (listener != NULL)
	{
		lialg_ptrdic_remove (self->listeners, object);
		liext_listener_free (listener);
	}

	return 1;
}

static int
private_object_client_login (liextModule* self,
                             liengObject* object)
{
	/* Send the material database to the client. */
	lisrv_client_send (LISRV_OBJECT (object)->client, self->assign_packet, GRAPPLE_RELIABLE);

	return 1;
}

static int
private_object_motion (liextModule* self,
                       liengObject* object)
{
	liextListener* listener;

	if (LISRV_OBJECT (object)->client == NULL)
		return 1;

	/* Mark listener as moved. */
	listener = lialg_ptrdic_find (self->listeners, object);
	if (listener != NULL)
		listener->moved = 1;

	return 1;
}

static int
private_object_visibility (liextModule* self,
                           liengObject* object,
                           int          visible)
{
	liextListener* listener;

	if (LISRV_OBJECT (object)->client == NULL)
		return 1;
	if (visible)
	{
		/* Subscribe to terrain updates. */
		listener = lialg_ptrdic_find (self->listeners, object);
		if (listener != NULL)
		{
			listener->moved = 1;
			return 1;
		}
		listener = liext_listener_new (self, object, self->radius);
		if (listener == NULL)
			return 1;
		if (!lialg_ptrdic_insert (self->listeners, object, listener))
		{
			liext_listener_free (listener);
			return 1;
		}
		listener->moved = 1;
		return 1;
	}
	else
	{
		/* Unsubscribe from terrain updates. */
		listener = lialg_ptrdic_find (self->listeners, object);
		if (listener == NULL)
			return 1;
		lialg_ptrdic_remove (self->listeners, object);
		liext_listener_free (listener);
	}

	return 1;
}

static int
private_sector_load (liextModule* self,
                     liengSector* sector)
{
	uint32_t index;

	index = LIVOX_SECTOR_INDEX (sector->x, sector->y, sector->z);
	livox_manager_load_sector (self->voxels, index, self->server->sql);

	return 1;
}

static int
private_tick (liextModule* self,
              float        secs)
{
	int i;
	int x;
	int y;
	int z;
	lialgU32dicIter iter;
	lialgPtrdicIter iter1;
	livoxBlock* block;
	livoxSector* sector;

	/* Rebuild modified terrain blocks. */
	LI_FOREACH_U32DIC (iter, self->voxels->sectors)
	{
		sector = iter.value;
		if (!livox_sector_get_dirty (sector))
			continue;
		for (i = z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
		for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
		for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++, i++)
		{
			block = livox_sector_get_block (sector, i);
			if (!livox_block_get_dirty (block))
				continue;
			livox_sector_build_block (sector, x, y, z);
		}
	}

	/* Update listeners. */
	LI_FOREACH_PTRDIC (iter1, self->listeners)
		liext_listener_update (iter1.value, secs);

	/* Mark all terrain as clean. */
	LI_FOREACH_U32DIC (iter, self->voxels->sectors)
	{
		sector = iter.value;
		for (i = z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
		for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
		for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++, i++)
		{
			block = livox_sector_get_block (sector, i);
			livox_block_set_dirty (block, 0);
		}
		livox_sector_set_dirty (sector, 0);
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
