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

#include <client/lips-client.h>
#include "ext-inventory.h"

liextInventory*
liext_inventory_new (liextModule* module)
{
	liextInventory* self;

	self = calloc (1, sizeof (liextInventory));
	if (self == NULL)
		return NULL;
	self->module = module;

	return self;
}

void
liext_inventory_free (liextInventory* self)
{
#warning No need to free inventory objects since they are garbage collected, right?
#if 0
	int i;

	for (i = 0 ; i < self->slots.count ; i++)
	{
		if (self->slots.array[i] != NULL)
			lieng_object_ref (self->slots.array[i], -1);
	}
#endif
	free (self->slots.array);
	free (self);
}

void
liext_inventory_clear (liextInventory* self)
{
	int i;

	for (i = 0 ; i < self->slots.count ; i++)
	{
		if (self->slots.array[i] != NULL)
		{
			lieng_object_ref (self->slots.array[i], -1);
			self->slots.array[i] = NULL;
		}
	}
}

liengObject*
liext_inventory_get_object (liextInventory* self,
                            int             slot)
{
	if (slot < 0 || slot >= self->slots.count)
		return NULL;
	return self->slots.array[slot];
}

int
liext_inventory_set_object (liextInventory* self,
                            int             slot,
                            uint16_t        object)
{
	if (slot < 0 || slot >= self->slots.count)
		return 0;
	if (object != LINET_INVALID_MODEL)
	{
		if (self->slots.array[slot] == NULL)
		{
			self->slots.array[slot] = licli_object_new (self->module->module, 0, 0);
			if (self->slots.array[slot] != NULL)
			{
				lieng_object_set_model_code (self->slots.array[slot], object);
				lieng_object_ref (self->slots.array[slot], 1);
			}
		}
		else
			lieng_object_set_model_code (self->slots.array[slot], object);
	}
	else if (self->slots.array[slot] != NULL)
	{
		lieng_object_ref (self->slots.array[slot], -1);
		self->slots.array[slot] = NULL;
	}

	return 1;
}

int
liext_inventory_get_size (liextInventory* self)
{
	return self->slots.count;
}

int
liext_inventory_set_size (liextInventory* self,
                          int             value)
{
	int i;
	liengObject** tmp;

	/* Clear removed slots. */
	for (i = value ; i < self->slots.count ; i++)
	{
		if (self->slots.array[i] != NULL)
			lieng_object_ref (self->slots.array[i], -1);
	}

	/* Resize inventory. */
	if (value < self->slots.count)
	{
		tmp = realloc (self->slots.array, value * sizeof (liengObject*));
		if (tmp != NULL || !value)
			self->slots.array = tmp;
	}
	else
	{
		tmp = realloc (self->slots.array, value * sizeof (liengObject*));
		if (tmp == NULL && value)
			return 0;
		self->slots.array = tmp;
	}

	/* Clear newly created slots. */
	if (value > self->slots.count)
	{
		tmp += self->slots.count;
		memset (tmp, 0, (value - self->slots.count) * sizeof (liengObject*));
	}
	self->slots.count = value;

	return 1;
}

/** @} */
/** @} */
/** @} */
