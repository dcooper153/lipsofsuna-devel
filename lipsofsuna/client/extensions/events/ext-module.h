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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvEvents Events
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/script.h>

#define LIEXT_SCRIPT_EVENTS "Lips.Events"

enum
{
	LIEXT_EVENT_ACTION = LICLI_EVENT_TYPE_ACTION,
	LIEXT_EVENT_ASSIGN,
	LIEXT_EVENT_PACKET,
	LIEXT_EVENT_OBJECT_MODEL,
	LIEXT_EVENT_OBJECT_NEW,
	LIEXT_EVENT_SELECT,
	LIEXT_EVENT_TICK,
	LIEXT_EVENT_MAX
};

typedef struct _LIExtModule LIExtModule;
struct _LIExtModule
{
	LICalHandle calls[5];
	LICliClient* client;
};

LIExtModule*
liext_module_new (LICliClient* client);

void
liext_module_free (LIExtModule* self);

void
liext_module_event (LIExtModule* self,
                    int          type,
                                 ...) LISYS_ATTR_SENTINEL;

/*****************************************************************************/

void
liext_script_events (LIScrClass* self,
                   void*       data);

#endif

/** @} */
/** @} */
/** @} */
