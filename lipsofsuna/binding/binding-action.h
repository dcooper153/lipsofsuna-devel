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
 * \addtogroup libnd Binding
 * @{
 * \addtogroup LIBndAction Action
 * @{
 */

#ifndef __BINDING_ACTION_H__
#define __BINDING_ACTION_H__

#include "binding-types.h"

struct _LIBndAction
{
	int enabled;
	char* identifier;
	char* name;
	char* description;
	void* data;
	LIBndAction* prev;
	LIBndAction* next;
	libndCallback callback;
	LIBndManager* manager;
};

LIBndAction*
libnd_action_new (LIBndManager* manager,
                  const char*   identifier,
                  const char*   name,
                  const char*   description,
                  libndCallback callback,
                  void*         data);

void
libnd_action_free (LIBndAction* self);

int
libnd_action_get_enabled (LIBndAction* self);

void
libnd_action_set_enabled (LIBndAction* self,
                          int          value);

void*
libnd_action_get_userdata (LIBndAction* self);

void
libnd_action_set_userdata (LIBndAction* self,
                           void*        value);

#endif

/** @} */
/** @} */
