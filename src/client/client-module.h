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
#include <render/lips-render.h>
#include <script/lips-script.h>
#include <server/lips-server.h>
#include <sound/lips-sound.h>
#include <widget/lips-widget.h>
#include "client.h"
#include "client-callbacks.h"
#include "client-extension.h"
#include "client-network.h"
#include "client-object.h"
#include "client-paths.h"
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
	lialgStrdic* extensions;
	libndManager* bindings;
	licfgEffects* effects;
	licliClient* client;
	licliNetwork* network;
	licliPaths* paths;
	licliWindow* window;
	liengCamera* camera;
	liengEngine* engine;
	liscrScript* script;
	lisrvServer* server;
#ifndef LI_DISABLE_SOUND
	lisndManager* sound;
	lisndSource* music;
#endif
	liwdgManager* widgets;
	struct
	{
		liscrData* module;
	} events;
};

licliModule*
licli_module_new (licliClient* client,
                  const char*  name,
                  const char*  login,
                  const char*  password);

void
licli_module_free (licliModule* self);

int
licli_module_connect (licliModule* self,
                      const char*  name,
                      const char*  pass);

void
licli_module_event (licliModule* self,
                    int          type,
                                 ...) __LI_ATTRIBUTE_SENTINEL;

liengObject*
licli_module_find_object (licliModule* self,
                          uint32_t     id);

#ifndef LI_DISABLE_SOUND
lisndSample*
licli_module_find_sample_by_id (licliModule* self,
                                int          id);

lisndSample*
licli_module_find_sample_by_name (licliModule* self,
                                  const char*  name);
#endif

int
licli_module_handle_packet (licliModule* self,
                            int          type,
                            liReader*    reader);

int
licli_module_host (licliModule* self);

int
licli_module_load_extension (licliModule* self,
                             const char*  name);

void
licli_module_main (licliModule* self);

int
licli_module_pick (licliModule*    self,
                   int             x,
                   int             y,
                   lirndSelection* result);

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
