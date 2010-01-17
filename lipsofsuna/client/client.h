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
 * \addtogroup LICliClient Client
 * @{
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/binding.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/paths.h>
#include <lipsofsuna/render.h>
#include <lipsofsuna/script.h>
#include <lipsofsuna/server.h>
#include <lipsofsuna/thread.h>
#include <lipsofsuna/video.h>
#include <lipsofsuna/widget.h>
#include "client-callbacks.h"
#include "client-extension.h"
#include "client-network.h"
#include "client-object.h"
#include "client-types.h"
#include "client-window.h"

// FIXME
#define LICLI_ROTATION_SPEED 1.5f
#define LICLI_SELECT_MAXIMUM_COUNT 128
#define LICLI_SELECT_RECTANGLE_SIZE 1
extern const LIBndAction li_binding_actions[];
const LIBndAction* li_binding_action_get (const char* identifier);

#define LICLI_EVENT_TYPE_ACTION 0

struct _LICliClient
{
	/* Persistent. */
	float fps;
	float tick;
	char* root;
	LICliWindow* window;
	LIVidCalls video;

	/* Module specific. */
	int moving;
	char* name;
	char* path;
	char* login;
	char* password;
	char* camera_node;
	LIAlgCamera* camera;
	LIBndManager* bindings;
	LICliNetwork* network;
	LIPthPaths* paths;
	LIRenRender* render;
	LIRenScene* scene;
	LISerServer* server;
	LIThrThread* server_thread;
	LIWdgManager* widgets;

	/* Module specific, shared with server. */
	int quit;
	LIAlgSectors* sectors;
	LIAlgStrdic* extensions;
	LICalCallbacks* callbacks;
	LIEngEngine* engine;
	LIScrScript* script;
};

LICliClient*
licli_client_new (LIVidCalls* video,
                  const char* path,
                  const char* name);

void
licli_client_free (LICliClient* self);

int
licli_client_connect (LICliClient* self,
                      const char*  addr,
                      int          port,
                      int          udp,
                      const char*  name,
                      const char*  pass);

LICliExtension*
licli_client_find_extension (LICliClient* self,
                             const char*  name);

LIEngObject*
licli_client_find_object (LICliClient* self,
                          uint32_t     id);

int
licli_client_handle_packet (LICliClient* self,
                            int          type,
                            LIArcReader* reader);

int
licli_client_host (LICliClient* self);

int
licli_client_load_extension (LICliClient* self,
                             const char*  name);

int
licli_client_main (LICliClient* self);

void
licli_client_render (LICliClient* self);

void
licli_client_send (LICliClient* self,
                   LIArcWriter* writer,
                   int          flags);

int
licli_client_update (LICliClient* self,
                     float        secs);

int
licli_client_get_moving (LICliClient* self);

void
licli_client_set_moving (LICliClient* self,
                         int          value);

LIEngObject*
licli_client_get_player (LICliClient* self);

/*LICliClient*
licli_client_new (LICliClient* client,
                  const char*  path,
                  const char*  name,
                  const char*  login,
                  const char*  password);*/

#endif

/** @} */
/** @} */
