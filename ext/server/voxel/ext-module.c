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
	if (!lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_CLIENT, 1,
	     	private_object_client, self, self->calls + 0) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_MOTION, 1,
	     	private_object_motion, self, self->calls + 1) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_VISIBILITY, 1,
	     	private_object_visibility, self, self->calls + 2) ||
	    !lieng_engine_insert_call (server->engine, LIENG_CALLBACK_SECTOR_LOAD, 0,
	     	private_sector_load, self, self->calls + 3) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_TICK, 0,
	     	private_tick, self, self->calls + 4))
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
}

void
liext_module_fill_sphere (liextModule*       self,
                          const limatVector* center,
                          float              radius,
                          liengTile          terrain)
{
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
}

int
liext_module_write (liextModule* self,
                    liarcSql*    sql)
{
	const char* query;
	sqlite3_stmt* statement;
	lialgU32dicIter iter;

	/* Create material table. */
	query = "CREATE TABLE IF NOT EXISTS voxel_materials "
		"(id INTEGER PRIMARY KEY,name TEXT,shi REAL,"
		"dif0 REAL,dif1 REAL,dif2 REAL,dif3 REAL,"
		"spe0 REAL,spe1 REAL,spe2 REAL,spe3 REAL);";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Create sector table. */
	query = "CREATE TABLE IF NOT EXISTS voxel_sectors "
		"(id INTEGER PRIMARY KEY,data BLOB);";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Save materials. */
/*	LI_FOREACH_ASSOCID (iter, self->materials)
	{
		if (!liext_material_write (iter.value, sql))
			return 0;
	}*/

	/* Save terrain. */
	LI_FOREACH_U32DIC (iter, self->server->engine->sectors)
	{
		if (!liext_sector_write (iter.value, sql))
			return 0;
	}

	return 1;
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
	liext_sector_read (sector, self->server->sql);

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
	liengSector* sector;

	/* Rebuild modified terrain blocks. */
	LI_FOREACH_U32DIC (iter, self->server->engine->sectors)
	{
		sector = iter.value;
		if (!sector->dirty)
			continue;
		for (i = z = 0 ; z < LIENG_BLOCKS_PER_LINE ; z++)
		for (y = 0 ; y < LIENG_BLOCKS_PER_LINE ; y++)
		for (x = 0 ; x < LIENG_BLOCKS_PER_LINE ; x++, i++)
		{
			if (!sector->blocks[i].dirty)
				continue;
			lieng_sector_build_block (sector, x, y, z);
		}
	}

	/* Update listeners. */
	LI_FOREACH_PTRDIC (iter1, self->listeners)
		liext_listener_update (iter1.value, secs);

	/* Mark all terrain as clean. */
	LI_FOREACH_U32DIC (iter, self->server->engine->sectors)
	{
		sector = iter.value;
		for (i = z = 0 ; z < LIENG_BLOCKS_PER_LINE ; z++)
		for (y = 0 ; y < LIENG_BLOCKS_PER_LINE ; y++)
		for (x = 0 ; x < LIENG_BLOCKS_PER_LINE ; x++, i++)
			sector->blocks[i].dirty = 0;
		sector->dirty = 0;
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
