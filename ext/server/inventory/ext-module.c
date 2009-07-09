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
#include <server/lips-server.h>
#include "ext-module.h"
#include "ext-inventory.h"

static int
private_tick (liextModule* self,
              float        secs);

/*****************************************************************************/

lisrvExtensionInfo liextInfo =
{
	LISRV_EXTENSION_VERSION, "Inventory",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (lisrvServer* server)
{
	liextModule* self;

	self = calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->server = server;
	self->dictionary = lialg_u32dic_new ();
	if (self->dictionary == NULL)
	{
		free (self);
		return NULL;
	}
	if (!lieng_engine_insert_call (server->engine, LISRV_CALLBACK_TICK, 0,
	     	private_tick, self, self->calls + 0))
	{
		liext_module_free (self);
		return NULL;
	}

	liscr_script_create_class (server->script, "Inventory", liextInventoryScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	/* FIXME: Remove the class here. */
	lialg_u32dic_free (self->dictionary);
	lieng_engine_remove_calls (self->server->engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));
	free (self);
}

liextInventory*
liext_module_find_inventory (liextModule* self,
                             uint32_t     id)
{
	return lialg_u32dic_find (self->dictionary, id);
}

/*****************************************************************************/

static int
private_tick (liextModule* self,
              float        secs)
{
    /* FIXME. */
	return 1;
}

/** @} */
/** @} */
/** @} */
