/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup libnd Binding
 * @{
 * \addtogroup libndManager Manager
 * @{
 */

#ifndef __BINDING_MANAGER_H__
#define __BINDING_MANAGER_H__

#include <system/lips-system.h>
#include "binding.h"
#include "binding-action.h"

typedef struct _libndManager libndManager;
struct _libndManager
{
	libndAction* actions;
	libndBinding* bindings;
};

libndManager*
libnd_manager_new ();

void
libnd_manager_free (libndManager* self);

int
libnd_manager_event (libndManager* self,
                     libndType     type,
                     uint32_t      code,
                     uint32_t      mods,
                     float         value);

libndAction*
libnd_manager_find_action (libndManager* self,
                           const char*   id);

void
libnd_manager_insert_action (libndManager* self,
                             libndAction*  action);

void
libnd_manager_remove_action (libndManager* self,
                             libndAction*  action);

void
libnd_manager_insert_binding (libndManager* self,
                              libndBinding* binding);

void
libnd_manager_remove_binding (libndManager* self,
                              libndBinding* binding);

#endif

/** @} */
/** @} */
