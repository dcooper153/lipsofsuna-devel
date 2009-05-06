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
 * \addtogroup liextcliInventory Inventory
 * @{
 */

#ifndef __EXT_INVENTORY_H__
#define __EXT_INVENTORY_H__

#include "ext-module.h"

struct _liextInventory
{
	liextModule* module;
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

void
liext_inventory_clear (liextInventory* self);

liengObject*
liext_inventory_get_object (liextInventory* self,
                            int             slot);

int
liext_inventory_set_object (liextInventory* self,
                            int             slot,
                            uint16_t        object);

int
liext_inventory_get_size (liextInventory* self);

int
liext_inventory_set_size (liextInventory* self,
                          int             value);

#endif

/** @} */
/** @} */
/** @} */
