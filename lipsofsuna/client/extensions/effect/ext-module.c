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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliEffect Effect
 * @{
 */

#include <client/lips-client.h>
#include "ext-module.h"

licliExtensionInfo liextInfo =
{
	LICLI_EXTENSION_VERSION, "Effect",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliClient* client)
{
	liextModule* self;
	liscrClass* clss;

	self = lisys_calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->client = client;

	clss = liscr_script_create_class (client->script, "Effect", liextEffectScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	/* FIXME: Remove the class here. */
	lisys_free (self);
}

/** @} */
/** @} */
/** @} */
