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
 * \addtogroup liextsrvInventory Inventory
 * @{
 */

#ifndef __EXT_SKILLS_H__
#define __EXT_SKILLS_H__

#include <algorithm/lips-algorithm.h>
#include <server/lips-server.h>
#include <system/lips-system.h>
#include "ext-module.h"

struct _liextInventory
{
	int id;
	lialgU32dic* listeners;
	liextModule* module;
	liscrData* script;
	lisrvServer* server;
	struct
	{
		int count;
		liengObject** array;
	} slots;
};

liextInventory*
liext_inventory_new (liextModule* module);

void
liext_inventory_free (liextInventory* self);

liengObject*
liext_inventory_find_listener (liextInventory* self,
                               int             id);

int
liext_inventory_insert_listener (liextInventory* self,
                                 liengObject*    value);

void
liext_inventory_remove_listener (liextInventory* self,
                                 liengObject*    value);

int
liext_inventory_get_id (const liextInventory* self);

liengObject*
liext_inventory_get_object (liextInventory* self,
                            int             slot);

int
liext_inventory_set_object (liextInventory* self,
                            int             slot,
                            liengObject*    object);

int
liext_inventory_get_size (liextInventory* self);

int
liext_inventory_set_size (liextInventory* self,
                          int             value);

#endif

/** @} */
/** @} */
/** @} */
