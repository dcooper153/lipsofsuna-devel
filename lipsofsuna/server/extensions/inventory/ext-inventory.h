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
 * \addtogroup liextsrvInventory Inventory
 * @{
 */

#ifndef __EXT_SKILLS_H__
#define __EXT_SKILLS_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/server.h>
#include <lipsofsuna/system.h>
#include "ext-module.h"

struct _LIExtInventory
{
	int id;
	LIAlgU32dic* listeners;
	LIExtModule* module;
	LIScrData* script;
	LISerServer* server;
	struct
	{
		int count;
		LIEngObject** array;
	} slots;
};

LIExtInventory*
liext_inventory_new (LIExtModule* module);

void
liext_inventory_free (LIExtInventory* self);

LIEngObject*
liext_inventory_find_listener (LIExtInventory* self,
                               int             id);

int
liext_inventory_insert_listener (LIExtInventory* self,
                                 LIEngObject*    value);

void
liext_inventory_remove_listener (LIExtInventory* self,
                                 LIEngObject*    value);

void
liext_inventory_reset_listener (LIExtInventory* self,
                                LIEngObject*    value);

int
liext_inventory_get_id (const LIExtInventory* self);

LIEngObject*
liext_inventory_get_object (LIExtInventory* self,
                            int             slot);

int
liext_inventory_set_object (LIExtInventory* self,
                            int             slot,
                            LIEngObject*    object);

int
liext_inventory_get_size (LIExtInventory* self);

int
liext_inventory_set_size (LIExtInventory* self,
                          int             value);

#endif

/** @} */
/** @} */
/** @} */
