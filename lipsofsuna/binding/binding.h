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
 * \addtogroup libnd Binding
 * @{
 * \addtogroup libndBinding Binding
 * @{
 */

#ifndef __BINDING_H__
#define __BINDING_H__

#include <system/lips-system.h>
#include "binding-action.h"
#include "binding-types.h"

struct _libndBinding
{
	uint32_t code;
	uint32_t mods;
	uint8_t priority;
	float multiplier;
	char* params;
	void* userdata;
	libndType type;
	libndBinding* prev;
	libndBinding* next;
	libndAction* action;
	libndManager* manager;
};

libndBinding*
libnd_binding_new (libndManager* manager,
                   libndType     type,
                   libndAction*  action,
                   const char*   params,
                   uint32_t      code,
                   uint32_t      mods,
                   float         multiplier);

void
libnd_binding_free (libndBinding* self);

void*
libnd_binding_get_userdata (libndBinding* self);

void
libnd_binding_set_userdata (libndBinding* self,
                            void*         value);

#endif

/** @} */
/** @} */
