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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliClient Client
 * @{
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <algorithm/lips-algorithm.h>
#include <callback/lips-callback.h>
#include <binding/lips-binding.h>
#include <engine/lips-engine.h>
#include <paths/lips-paths.h>
#include <render/lips-render.h>
#include <script/lips-script.h>
#include <server/lips-server.h>
#include <thread/lips-thread.h>
#include <video/lips-video.h>
#include <widget/lips-widget.h>
#include "client-callbacks.h"
#include "client-extension.h"
#include "client-network.h"
#include "client-object.h"
#include "client-types.h"
#include "client-window.h"

// FIXME
#define LI_CLIENT_ROTATION_SPEED 1.5f
#define LI_CLIENT_SELECT_MAXIMUM_COUNT 128
#define LI_CLIENT_SELECT_RECTANGLE_SIZE 1
extern const libndAction li_binding_actions[];
const libndAction* li_binding_action_get (const char* identifier);

#define LICLI_EVENT_TYPE_ACTION 0

struct _licliClient
{
	/* Persistent. */
	char* root;
	licliWindow* window;
	lividCalls video;

	/* Module specific. */
	int moving;
	int quit;
	char* name;
	char* path;
	char* login;
	char* password;
	char* camera_node;
	lialgCamera* camera;
	lialgSectors* sectors;
	lialgStrdic* extensions;
	libndManager* bindings;
	licalCallbacks* callbacks;
	licliNetwork* network;
	liengEngine* engine;
	lipthPaths* paths;
	lirndRender* render;
	lirndScene* scene;
	liscrScript* script;
	lisrvServer* server;
	lithrThread* server_thread;
	liwdgManager* widgets;
};

licliClient*
licli_client_new (lividCalls* video,
                  const char* path,
                  const char* name);

void
licli_client_free (licliClient* self);

int
licli_client_connect (licliClient* self,
                      const char*  name,
                      const char*  pass);

licliExtension*
licli_client_find_extension (licliClient* self,
                             const char*  name);

liengObject*
licli_client_find_object (licliClient* self,
                          uint32_t     id);

int
licli_client_handle_packet (licliClient* self,
                            int          type,
                            liarcReader* reader);

int
licli_client_host (licliClient* self);

int
licli_client_load_extension (licliClient* self,
                             const char*  name);

int
licli_client_main (licliClient* self);

void
licli_client_render (licliClient* self);

void
licli_client_send (licliClient* self,
                   liarcWriter* writer,
                   int          flags);

int
licli_client_update (licliClient* self,
                     float        secs);

int
licli_client_get_moving (licliClient* self);

void
licli_client_set_moving (licliClient* self,
                         int          value);

liengObject*
licli_client_get_player (licliClient* self);

/*licliClient*
licli_client_new (licliClient* client,
                  const char*  path,
                  const char*  name,
                  const char*  login,
                  const char*  password);*/

#endif

/** @} */
/** @} */
