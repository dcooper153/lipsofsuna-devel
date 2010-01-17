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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtSlots Slots
 * @{
 */

#ifndef __EXT_SLOTS_H__
#define __EXT_SLOTS_H__

#include <lipsofsuna/main.h>
#include <lipsofsuna/system.h>
#include "ext-module.h"

#define LIEXT_SLOTS(o) ((LIExtSlots*)(o))

enum _LIExtSlotType
{
	LIEXT_SLOT_TYPE_PUBLIC,
	LIEXT_SLOT_TYPE_PRIVATE,
	LIEXT_SLOT_TYPE_MAX
};

struct _LIExtSlot
{
	char* name;
	char* node; /* FIXME: Should be an index to node list. */
	LIExtSlotType type;
	LIEngObject* object;
};

struct _LIExtSlots
{
	LIEngObject* owner;
	LIExtModule* module;
	LIScrData* script;
	struct
	{
		int count;
		LIExtSlot* array;
	} slots;
};

LIExtSlots*
liext_slots_new (LIExtModule* module);

void
liext_slots_free (LIExtSlots* self);

LIExtSlot*
liext_slots_find_slot (LIExtSlots* self,
                       const char* name);

int
liext_slots_insert_slot (LIExtSlots*   self,
                         LIExtSlotType type,
                         const char*   name,
                         const char*   node);

int
liext_slots_get_object (LIExtSlots*   self,
                        const char*   slot,
                        LIEngObject** object);

int
liext_slots_set_object (LIExtSlots*  self,
                        const char*  slot,
                        LIEngObject* object);

LIEngObject*
liext_slots_get_owner (LIExtSlots* self);

int
liext_slots_set_owner (LIExtSlots*  self,
                       LIEngObject* value);

#endif

/** @} */
/** @} */
