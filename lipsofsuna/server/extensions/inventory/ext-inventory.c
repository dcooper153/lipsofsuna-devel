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

#include <network/lips-network.h>
#include "ext-module.h"
#include "ext-inventory.h"

#define LIEXT_INVENTORY_VERSION 0xFF

static int
private_send_close (liextInventory* self,
                    liengObject*    listener);

static int
private_send_open (liextInventory* self,
                   liengObject*    listener);

static int
private_send_object (liextInventory* self,
                     int             slot,
                     liengObject*    object);

static int
private_send_remove (liextInventory* self,
                     int             slot);

/*****************************************************************************/

/**
 * \brief Creates a new inventory.
 *
 * \param module Extension module.
 * \return New inventory or NULL.
 */
liextInventory*
liext_inventory_new (liextModule* module)
{
	liextInventory* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liextInventory));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->server = module->server;

	/* Find unique ID. */
	while (!self->id)
	{
		self->id = rand ();
		if (lialg_u32dic_find (module->dictionary, self->id))
			self->id = 0;
	}

	/* Register self. */
	if (!lialg_u32dic_insert (module->dictionary, self->id, self))
	{
		free (self);
		return NULL;
	}

	/* Allocate listener dictionary. */
	self->listeners = lialg_u32dic_new ();
	if (self->listeners == NULL)
	{
		liext_inventory_free (self);
		return NULL;
	}

	/* Set default size. */
	if (!liext_inventory_set_size (self, 10))
	{
		liext_inventory_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the inventory.
 *
 * Objects are managed by the garbage collector so the stored objects
 * must not be freed here. The garbage collector is already doing it.
 *
 * \param self Inventory.
 */
void
liext_inventory_free (liextInventory* self)
{
	lialg_u32dic_remove (self->module->dictionary, self->id);
	if (self->listeners != NULL)
		lialg_u32dic_free (self->listeners);
	free (self->slots.array);
	free (self);
}

/**
 * \brief Finds a listener by id.
 *
 * \param self Inventory.
 * \param id Listener id.
 * \return Object or NULL.
 */
liengObject*
liext_inventory_find_listener (liextInventory* self,
                               int             id)
{
	return lialg_u32dic_find (self->listeners, id);
}

/**
 * \brief Adds a new inventory listener.
 *
 * \param self Inventory.
 * \param value Listener object.
 * \return Nonzero on success.
 */
int
liext_inventory_insert_listener (liextInventory* self,
                                 liengObject*    value)
{
	if (lialg_u32dic_find (self->listeners, value->id))
		return 1;
	if (!lialg_u32dic_insert (self->listeners, value->id, value))
		return 0;
	liscr_data_ref (value->script, self->script);
	private_send_open (self, value);

	return 1;
}

/**
 * \brief Removes an inventory listener.
 *
 * \param self Inventory.
 * \param value Listener object.
 */
void
liext_inventory_remove_listener (liextInventory* self,
                                 liengObject*    value)
{
	if (lialg_u32dic_find (self->listeners, value->id))
	{
		private_send_close (self, value);
		lialg_u32dic_remove (self->listeners, value->id);
		liscr_data_ref (value->script, self->script);
	}
}

/**
 * \brief Resets an inventory listener.
 *
 * This is called when the client associated to the listener has changed.
 * Causes the listener to be removed and readded so that the new inventory
 * ownership states get sent to the clients.
 *
 * \param self Inventory.
 * \param value Listener object.
 */
void
liext_inventory_reset_listener (liextInventory* self,
                                liengObject*    value)
{
	if (lialg_u32dic_find (self->listeners, value->id))
	{
		liext_inventory_remove_listener (self, value);
		liext_inventory_insert_listener (self, value);
	}
}

/**
 * \brief Gets an object in the specified slot.
 *
 * \param self Inventory.
 * \param slot Slot number.
 * \return Object or NULL.
 */
liengObject*
liext_inventory_get_object (liextInventory* self,
                            int             slot)
{
	if (slot < 0 || slot >= self->slots.count)
		return NULL;

	return self->slots.array[slot];
}

/**
 * \brief Sets an object in the specified slot.
 *
 * \param self Inventory.
 * \param slot Slot number.
 * \param object Object to set.
 * \return Nonzero on success.
 */
int
liext_inventory_set_object (liextInventory* self,
                            int             slot,
                            liengObject*    object)
{
	if (slot < 0 || slot >= self->slots.count)
		return 0;
	if (self->slots.array[slot] != NULL)
		liscr_data_unref (self->slots.array[slot]->script, self->script);
	self->slots.array[slot] = object;
	if (object != NULL)
	{
		liscr_data_ref (object->script, self->script);
		private_send_object (self, slot, object);
	}
	else
		private_send_remove (self, slot);

	return 1;
}

/**
 * \brief Gets the number of items the inventory can hold.
 *
 * \param self Inventory.
 * \return Integer.
 */
int
liext_inventory_get_size (liextInventory* self)
{
	return self->slots.count;
}

/**
 * \brief Sets the number of items the inventory can hold.
 *
 * \param self Inventory.
 * \param value Integer.
 * \return Nonzero on success.
 */
int
liext_inventory_set_size (liextInventory* self,
                          int             value)
{
	int i;
	lialgU32dicIter iter;
	liengObject** tmp;

	/* Check for changes. */
	if (value == self->slots.count)
		return 1;

	/* Set new size. */
	if (value > self->slots.count)
	{
		tmp = realloc (self->slots.array, value * sizeof (liengObject*));
		if (tmp == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			return 0;
		}
		self->slots.array = tmp;
		tmp += self->slots.count;
		memset (tmp, 0, (value - self->slots.count) * sizeof (liengObject*));
		self->slots.count = value;
	}
	else
	{
		for (i = value ; i < self->slots.count ; i++)
		{
			if (self->slots.array[i] != NULL)
				liscr_data_unref (self->slots.array[i]->script, self->script);
		}
		tmp = realloc (self->slots.array, value * sizeof (liengObject*));
		if (tmp != NULL || !value)
			self->slots.array = tmp;
		self->slots.count = value;
	}

	/* Update all listeners. */
	LI_FOREACH_U32DIC (iter, self->listeners)
	{
		private_send_close (self, iter.value);
		private_send_open (self, iter.value);
	}

	return 1;
}

int
liext_inventory_get_id (const liextInventory* self)
{
	return self->id;
}

/*****************************************************************************/

static int
private_send_close (liextInventory* self,
                    liengObject*    listener)
{
	liscrScript* script = self->module->server->script;

	if (LISRV_OBJECT (listener)->client == NULL)
		return 1;

	/* Check for callback. */
	liscr_pushdata (script->lua, self->script);
	lua_getfield (script->lua, -1, "user_removed_cb");
	if (lua_type (script->lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (script->lua, 2);
		return 1;
	}

	/* Invoke callback. */
	lua_pushvalue (script->lua, -2);
	lua_remove (script->lua, -3);
	liscr_pushdata (script->lua, listener->script);
	if (lua_pcall (script->lua, 2, 0, 0) != 0)
	{
		lisys_error_set (EINVAL, "%s", lua_tostring (script->lua, -1));
		lisys_error_report ();
		lua_pop (script->lua, 1);
	}

	return 1;
}

static int
private_send_open (liextInventory* self,
                   liengObject*    listener)
{
	liscrScript* script = self->module->server->script;

	if (LISRV_OBJECT (listener)->client == NULL)
		return 1;

	/* Check for callback. */
	liscr_pushdata (script->lua, self->script);
	lua_getfield (script->lua, -1, "user_added_cb");
	if (lua_type (script->lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (script->lua, 2);
		return 1;
	}

	/* Invoke callback. */
	lua_pushvalue (script->lua, -2);
	lua_remove (script->lua, -3);
	liscr_pushdata (script->lua, listener->script);
	if (lua_pcall (script->lua, 2, 0, 0) != 0)
	{
		lisys_error_set (EINVAL, "%s", lua_tostring (script->lua, -1));
		lisys_error_report ();
		lua_pop (script->lua, 1);
	}

	return 1;
}

static int
private_send_object (liextInventory* self,
                     int             slot,
                     liengObject*    object)
{
	lialgU32dicIter iter;
	liscrScript* script = self->module->server->script;

	LI_FOREACH_U32DIC (iter, self->listeners)
	{
		if (LISRV_OBJECT (iter.value)->client == NULL)
			continue;

		/* Check for callback. */
		liscr_pushdata (script->lua, self->script);
		lua_getfield (script->lua, -1, "item_added_cb");
		if (lua_type (script->lua, -1) != LUA_TFUNCTION)
		{
			lua_pop (script->lua, 2);
			continue;
		}

		/* Invoke callback. */
		lua_pushvalue (script->lua, -2);
		lua_remove (script->lua, -3);
		liscr_pushdata (script->lua, LIENG_OBJECT (iter.value)->script);
		lua_pushnumber (script->lua, slot + 1);
		liscr_pushdata (script->lua, object->script);
		if (lua_pcall (script->lua, 4, 0, 0) != 0)
		{
			lisys_error_set (EINVAL, "%s", lua_tostring (script->lua, -1));
			lisys_error_report ();
			lua_pop (script->lua, 1);
		}
	}

	return 1;
}

static int
private_send_remove (liextInventory* self,
                     int             slot)
{
	lialgU32dicIter iter;
	liscrScript* script = self->module->server->script;

	LI_FOREACH_U32DIC (iter, self->listeners)
	{
		if (LISRV_OBJECT (iter.value)->client == NULL)
			continue;

		/* Check for callback. */
		liscr_pushdata (script->lua, self->script);
		lua_getfield (script->lua, -1, "item_removed_cb");
		if (lua_type (script->lua, -1) != LUA_TFUNCTION)
		{
			lua_pop (script->lua, 2);
			continue;
		}

		/* Invoke callback. */
		lua_pushvalue (script->lua, -2);
		lua_remove (script->lua, -3);
		liscr_pushdata (script->lua, LIENG_OBJECT (iter.value)->script);
		lua_pushnumber (script->lua, slot + 1);
		if (lua_pcall (script->lua, 3, 0, 0) != 0)
		{
			lisys_error_set (EINVAL, "%s", lua_tostring (script->lua, -1));
			lisys_error_report ();
			lua_pop (script->lua, 1);
		}
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
