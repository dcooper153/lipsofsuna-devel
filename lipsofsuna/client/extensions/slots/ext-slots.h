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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSlots Slots      
 * @{
 */

#ifndef __EXT_SLOTS_H__
#define __EXT_SLOTS_H__

#include "ext-module.h"
#include "ext-slot.h"

typedef struct _liextSlots liextSlots;
struct _liextSlots
{
	liextModule* module;
	lialgStrdic* slots;
	liengObject* object;
};

liextSlots*
liext_slots_new (liextModule* module,
                 liengObject* object);

void
liext_slots_free (liextSlots* self);

void
liext_slots_clear (liextSlots* self);

void
liext_slots_clear_object (liextSlots*  self,
                          liengObject* object);

int
liext_slots_set_slot (liextSlots* self,
                      const char* slot,
                      const char* node,
                      int         model);

#endif

/** @} */
/** @} */
/** @} */
