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
#include <grapple/grapple.h>
#include <ai/lips-ai.h>
#include <algorithm/lips-algorithm.h>
#include <config/lips-config.h>
#include <engine/lips-engine.h>
#include <physics/lips-physics.h>
#include <script/lips-script.h>
#include "server-event.h"
#include "server-network.h"
#include "server-object.h"
#include "server-paths.h"
#include "server-types.h"

struct _lisrvServer
{
	uint32_t debug;
	int quit;
	lialgStrdic* extensions;
	liengEngine* engine;
	liscrScript* script;
	lisrvNetwork* network;
	lisrvPaths* paths;
	struct
	{
		licfgBans* bans;
		licfgHost* host;
	} config;
	struct
	{
		liscrData* object;
	} events;
	struct
	{
		liaiPathSolver* path_solver;
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
lisrv_server_new (const char* name);

void
lisrv_server_free (lisrvServer* self);

void
lisrv_server_event (lisrvServer* self,
                    int          type,
                                 ...) __LI_ATTRIBUTE_SENTINEL;

int
lisrv_server_load_extension (lisrvServer* self,
                             const char*  name);

int
lisrv_server_insert_ban (lisrvServer* self,
                         const char*  ip);

int
lisrv_server_main (lisrvServer* self);

int
lisrv_server_remove_ban (lisrvServer* self,
                         const char*  ip);

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

#endif

/** @} */
/** @} */
