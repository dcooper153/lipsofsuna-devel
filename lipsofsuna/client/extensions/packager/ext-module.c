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
 * \addtogroup liextcliPackager Packager
 * @{
 */

#include <client/lips-client.h>
#include "ext-module.h"
#include "ext-packager.h"

licliExtensionInfo liextInfo =
{
	LICLI_EXTENSION_VERSION, "Packager",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliClient* client)
{
	liextModule* self;

	self = lisys_calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->client = client;
	self->packager = liext_packager_new (client);
	if (self->packager == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	liscr_script_create_class (client->script, "Packager", liextPackagerScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	/* FIXME: Remove the class here. */
	liext_packager_free (self->packager);
	lisys_free (self);
}

/** @} */
/** @} */
/** @} */
