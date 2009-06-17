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
 * \addtogroup liextsrvSlots Slots
 * @{
 */

#ifndef __EXT_SLOTS_H__
#define __EXT_SLOTS_H__

#include <server/lips-server.h>
#include <system/lips-system.h>
#include "ext-module.h"

#define LIEXT_SLOTS(o) ((liextSlots*)(o))

enum _liextSlotType
{
	LIEXT_SLOT_TYPE_PUBLIC,
	LIEXT_SLOT_TYPE_PRIVATE,
	LIEXT_SLOT_TYPE_MAX
};

struct _liextSlot
{
	char* name;
	char* node; /* FIXME: Should be an index to node list. */
	liextSlotType type;
	liengObject* object;
};

struct _liextSlots
{
	liengObject* owner;
	liextModule* module;
	liscrData* script;
	lisrvServer* server;
	struct
	{
		int count;
		liextSlot* array;
	} slots;
};

liextSlots*
liext_slots_new (liextModule* module);

void
liext_slots_free (liextSlots* self);

liextSlot*
liext_slots_find_slot (liextSlots* self,
                       const char* name);

int
liext_slots_insert_slot (liextSlots*   self,
                         liextSlotType type,
                         const char*   name,
                         const char*   node);

int
liext_slots_get_object (liextSlots*   self,
                        const char*   slot,
                        liengObject** object);

int
liext_slots_set_object (liextSlots*  self,
                        const char*  slot,
                        liengObject* object);

liengObject*
liext_slots_get_owner (liextSlots* self);

int
liext_slots_set_owner (liextSlots*  self,
                       liengObject* value);

#endif

/** @} */
/** @} */
/** @} */
