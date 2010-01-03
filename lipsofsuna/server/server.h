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
 * \addtogroup liser Server
 * @{
 * \addtogroup LISerServer Server
 * @{
 */

#ifndef __SERVER_H__
#define __SERVER_H__

#include <pthread.h>
#include <sys/time.h>
#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/config.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/paths.h>
#include <lipsofsuna/physics.h>
#include <lipsofsuna/script.h>
#include "server-network.h"
#include "server-object.h"
#include "server-types.h"

struct _LISerServer
{
	uint32_t debug;
	int quit;
	LIAlgSectors* sectors;
	LIAlgStrdic* extensions;
	LIArcSql* sql;
	LICalCallbacks* callbacks;
	LIEngEngine* engine;
	LIPthPaths* paths;
	LIScrScript* script;
	LISerNetwork* network;
	struct
	{
		LICfgBans* bans;
		LICfgHost* host;
	} config;
	struct
	{
		LIArcWriter* resources;
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

LISerServer*
liser_server_new (LIPthPaths* paths);

void
liser_server_free (LISerServer* self);

LISerExtension*
liser_server_find_extension (LISerServer* self,
                             const char*  name);

int
liser_server_insert_ban (LISerServer* self,
                         const char*  ip);

int
liser_server_load_extension (LISerServer* self,
                             const char*  name);

int
liser_server_main (LISerServer* self);

int
liser_server_remove_ban (LISerServer* self,
                         const char*  ip);

int
liser_server_save (LISerServer* self);

void
liser_server_shutdown (LISerServer* self);

int
liser_server_update (LISerServer* self,
                     float        secs);

int
liser_server_get_banned (LISerServer* self,
                         const char*  address);

double
liser_server_get_time (const LISerServer* self);

uint32_t
liser_server_get_unique_object (const LISerServer* self);

#endif

/** @} */
/** @} */
