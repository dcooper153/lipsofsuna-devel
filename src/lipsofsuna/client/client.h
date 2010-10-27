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

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/main.h>
#include <lipsofsuna/paths.h>
#include <lipsofsuna/render.h>
#include <lipsofsuna/script.h>
#include <lipsofsuna/server.h>
#include <lipsofsuna/thread.h>
#include <lipsofsuna/video.h>
#include <lipsofsuna/widget.h>
#include "client-callbacks.h"
#include "client-types.h"
#include "client-window.h"

// FIXME
#define LICLI_SELECT_MAXIMUM_COUNT 128
#define LICLI_SELECT_RECTANGLE_SIZE 1

struct _LICliClient
{
	/* Persistent. */
	char* root;
	LICliWindow* window;

	/* Program. */
	LIAlgSectors* sectors;
	LICalCallbacks* callbacks;
	LIEngEngine* engine;
	LIMaiProgram* program;
	LIScrScript* script;

	/* Module. */
	int moving;
	char* name;
	char* path;
	LIPthPaths* paths;
	LIRenRender* render;
	LIRenScene* scene;
	LISerServer* server;
	LIThrThread* server_thread;
};

LIAPICALL (LICliClient*, licli_client_new, (
	const char* path,
	const char* name));

LIAPICALL (void, licli_client_free, (
	LICliClient* self));

LIAPICALL (void, licli_client_free_module, (
	LICliClient* self));

LIAPICALL (int, licli_client_host, (
	LICliClient* self,
	const char*  args));

LIAPICALL (int, licli_client_load_module, (
	LICliClient* self,
	const char*  name,
	const char*  args));

LIAPICALL (int, licli_client_main, (
	LICliClient* self));

LIAPICALL (void, licli_client_render, (
	LICliClient* self));

LIAPICALL (int, licli_client_get_moving, (
	LICliClient* self));

LIAPICALL (void, licli_client_set_moving, (
	LICliClient* self,
	int          value));

#endif
