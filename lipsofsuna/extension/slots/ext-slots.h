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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSlots Slots      
 * @{
 */

#ifndef __EXT_SLOTS_H__
#define __EXT_SLOTS_H__

#include "ext-module.h"
#include "ext-slot.h"

typedef struct _LIExtSlots LIExtSlots;
struct _LIExtSlots
{
	LIExtModule* module;
	LIAlgStrdic* slots;
	LIEngObject* object;
};

LIExtSlots*
liext_slots_new (LIExtModule* module,
                 LIEngObject* object);

void
liext_slots_free (LIExtSlots* self);

void
liext_slots_clear (LIExtSlots* self);

void
liext_slots_clear_object (LIExtSlots*  self,
                          LIEngObject* object);

int
liext_slots_set_slot (LIExtSlots* self,
                      const char* slot,
                      const char* node,
                      LIEngModel* model);

#endif

/** @} */
/** @} */
/** @} */
