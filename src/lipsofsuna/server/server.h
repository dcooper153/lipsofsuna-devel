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

#ifndef __SERVER_H__
#define __SERVER_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/main.h>
#include "server-types.h"

struct _LISerServer
{
	/* Server. */
	LIMaiProgram* program;

	/* Program. */
	LIAlgSectors* sectors;
	LICalCallbacks* callbacks;
	LIEngEngine* engine;
	LIPthPaths* paths;
	LIScrScript* script;
};

LIAPICALL (LISerServer*, liser_server_new, (
	LIPthPaths* paths,
	const char* args));

LIAPICALL (void, liser_server_free, (
	LISerServer* self));

LIAPICALL (int, liser_server_main, (
	LISerServer* self));

#endif
