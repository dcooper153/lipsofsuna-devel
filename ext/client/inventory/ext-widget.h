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

#ifndef __EXT_WIDGET_H__
#define __EXT_WIDGET_H__

#include <widget/lips-widget.h>
#include "ext-module.h"

#define LIEXT_INVENTORY_WIDGET(o) ((liextInventoryWidget*)(o))

typedef struct _liextInventoryWidget liextInventoryWidget;
struct _liextInventoryWidget
{
	liwdgWidget base;
	liextInventory* inventory;
	liextModule* module;
	lirndLight* light;
	liwdgSize slotsize;
};

liwdgWidget*
liext_inventory_widget_new (liextInventory* inventory);

void
liext_inventory_widget_get_slot_rect (const liextInventoryWidget* self,
                                      int                         slot,
                                      liwdgRect*                  value);

#endif

/** @} */
/** @} */
/** @} */
