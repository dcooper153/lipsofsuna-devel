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
 * \addtogroup liextsrvEvents Events
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <script/lips-script.h>
#include <server/lips-server.h>

#define LIEXT_SCRIPT_EVENTS "Lips.Events"

enum
{
	LIEXT_EVENT_ANIMATION,
	LIEXT_EVENT_CONTROL,
	LIEXT_EVENT_EFFECT,
	LIEXT_EVENT_LOGIN,
	LIEXT_EVENT_LOGOUT,
	LIEXT_EVENT_PACKET,
	LIEXT_EVENT_SIMULATE,
	LIEXT_EVENT_TICK,
	LIEXT_EVENT_VISIBILITY,
	LIEXT_EVENT_MAX
};

typedef struct _liextModule liextModule;
struct _liextModule
{
	licalHandle calls[10];
	lisrvServer* server;
};

liextModule*
liext_module_new (lisrvServer* server);

void
liext_module_free (liextModule* self);

void
liext_module_event (liextModule* self,
                    int          type,
                                 ...) __LI_ATTRIBUTE_SENTINEL;

/*****************************************************************************/

void
liextEventsScript (liscrClass* self,
                   void*       data);

#endif

/** @} */
/** @} */
/** @} */
