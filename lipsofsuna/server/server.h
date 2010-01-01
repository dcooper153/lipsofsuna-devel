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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvServer Server
 * @{
 */

#ifndef __SERVER_H__
#define __SERVER_H__

#include <pthread.h>
#include <sys/time.h>
#include <algorithm/lips-algorithm.h>
#include <callback/lips-callback.h>
#include <config/lips-config.h>
#include <engine/lips-engine.h>
#include <paths/lips-paths.h>
#include <physics/lips-physics.h>
#include <script/lips-script.h>
#include "server-network.h"
#include "server-object.h"
#include "server-types.h"

struct _lisrvServer
{
	uint32_t debug;
	int quit;
	lialgSectors* sectors;
	lialgStrdic* extensions;
	liarcSql* sql;
	licalCallbacks* callbacks;
	liengEngine* engine;
	lipthPaths* paths;
	liscrScript* script;
	lisrvNetwork* network;
	struct
	{
		licfgBans* bans;
		licfgHost* host;
	} config;
	struct
	{
		liarcWriter* resources;
	} helper;
	struct
	{
		pthread_mutex_t bans;
	} mutexes;
	struct
	{
		struct timeval start;
	} time;
};

lisrvServer*
lisrv_server_new (lipthPaths* paths);

void
lisrv_server_free (lisrvServer* self);

lisrvExtension*
lisrv_server_find_extension (lisrvServer* self,
                             const char*  name);

int
lisrv_server_insert_ban (lisrvServer* self,
                         const char*  ip);

int
lisrv_server_load_extension (lisrvServer* self,
                             const char*  name);

int
lisrv_server_main (lisrvServer* self);

int
lisrv_server_remove_ban (lisrvServer* self,
                         const char*  ip);

int
lisrv_server_save (lisrvServer* self);

void
lisrv_server_shutdown (lisrvServer* self);

int
lisrv_server_update (lisrvServer* self,
                     float        secs);

int
lisrv_server_get_banned (lisrvServer* self,
                         const char*  address);

double
lisrv_server_get_time (const lisrvServer* self);

uint32_t
lisrv_server_get_unique_object (const lisrvServer* self);

#endif

/** @} */
/** @} */
