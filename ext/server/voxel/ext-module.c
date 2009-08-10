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
	livox_manager_set_sql (self->voxels, server->sql);
	if (!livox_manager_load_materials (self->voxels))
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
liext_module_write (liextModule* self,
                    liarcSql*    sql)
{
	return livox_manager_write (self->voxels);
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
	livox_manager_load_sector (self->voxels, index);

	return 1;
}

static int
private_tick (liextModule* self,
              float        secs)
{
	lialgPtrdicIter iter1;

	livox_manager_mark_updates (self->voxels);
	LI_FOREACH_PTRDIC (iter1, self->listeners)
		liext_listener_update (iter1.value, secs);
	livox_manager_update_marked (self->voxels);

	return 1;
}

/** @} */
/** @} */
/** @} */
