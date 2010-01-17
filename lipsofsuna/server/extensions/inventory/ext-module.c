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
 * \addtogroup LIExtInventory Inventory 
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/server.h>
#include "ext-module.h"
#include "ext-inventory.h"

static int
private_object_client (LIExtModule* self,
                       LIEngObject* object);

static int
private_tick (LIExtModule* self,
              float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_info =
{
	LIMAI_EXTENSION_VERSION, "Inventory",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LIMaiProgram* program)
{
	LIExtModule* self;

	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;
	self->dictionary = lialg_u32dic_new ();
	if (self->dictionary == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->ptrdic = lialg_ptrdic_new ();
	if (self->ptrdic == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	if (!lical_callbacks_insert (program->callbacks, program->engine, "object-client", 0, private_object_client, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "tick", 0, private_tick, self, self->calls + 1))
	{
		liext_module_free (self);
		return NULL;
	}

	liscr_script_create_class (program->script, "Inventory", liext_script_inventory, self);

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	lialg_u32dic_free (self->dictionary);
	lialg_ptrdic_free (self->ptrdic);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

LIExtInventory*
liext_module_find_inventory (LIExtModule* self,
                             uint32_t     id)
{
	return lialg_u32dic_find (self->dictionary, id);
}

/*****************************************************************************/

static int
private_object_client (LIExtModule* self,
                       LIEngObject* object)
{
	LIAlgU32dicIter iter;

	LIALG_U32DIC_FOREACH (iter, self->dictionary)
		liext_inventory_reset_listener (iter.value, object);

	return 1;
}

static int
private_tick (LIExtModule* self,
              float        secs)
{
    /* FIXME. */
	return 1;
}

/** @} */
/** @} */
