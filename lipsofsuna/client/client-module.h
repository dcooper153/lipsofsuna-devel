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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliModule Module
 * @{
 */

#ifndef __CLIENT_MODULE_H__
#define __CLIENT_MODULE_H__

#include <algorithm/lips-algorithm.h>
#include <binding/lips-binding.h>
#include <engine/lips-engine.h>
#include <paths/lips-paths.h>
#include <render/lips-render.h>
#include <script/lips-script.h>
#include <server/lips-server.h>
#include <thread/lips-thread.h>
#include <widget/lips-widget.h>
#include "client.h"
#include "client-callbacks.h"
#include "client-extension.h"
#include "client-network.h"
#include "client-object.h"
#include "client-types.h"
#include "client-window.h"

struct _licliModule
{
	int moving;
	int quit;
	char* name;
	char* path;
	char* login;
	char* password;
	char* camera_node;
	lialgStrdic* extensions;
	libndManager* bindings;
	licliClient* client;
	licliNetwork* network;
	licliWindow* window;
	liengCamera* camera;
	liengEngine* engine;
	lipthPaths* paths;
	lirndRender* render;
	lirndScene* scene;
	liscrScript* script;
	lisrvServer* server;
	lithrThread* server_thread;
	liwdgManager* widgets;
};

licliModule*
licli_module_new (licliClient* client,
                  const char*  path,
                  const char*  name,
                  const char*  login,
                  const char*  password);

void
licli_module_free (licliModule* self);

int
licli_module_connect (licliModule* self,
                      const char*  name,
                      const char*  pass);

liengObject*
licli_module_find_object (licliModule* self,
                          uint32_t     id);

int
licli_module_handle_packet (licliModule* self,
                            int          type,
                            liarcReader* reader);

int
licli_module_host (licliModule* self);

int
licli_module_load_extension (licliModule* self,
                             const char*  name);

void
licli_module_main (licliModule* self);

void
licli_module_render (licliModule* self);

void
licli_module_send (licliModule* self,
                   liarcWriter* writer,
                   int          flags);

int
licli_module_update (licliModule* self,
                     float        secs);

int
licli_module_get_moving (licliModule* self);

void
licli_module_set_moving (licliModule* self,
                         int          value);

liengObject*
licli_module_get_player (licliModule* self);

#endif

/** @} */
/** @} */
