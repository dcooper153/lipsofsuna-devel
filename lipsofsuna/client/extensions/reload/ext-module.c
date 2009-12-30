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
 * \addtogroup liextcliReload Reload
 * @{
 */

#include <client/lips-client.h>
#include "ext-module.h"
#include "ext-reload.h"

licliExtensionInfo liextInfo =
{
	LICLI_EXTENSION_VERSION, "Reload",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliModule* module)
{
	liextModule* self;

	self = lisys_calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->reload = liext_reload_new (module);
	if (self->reload == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	liscr_script_create_class (module->script, "Reload", liextReloadScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (licalHandle));
	liext_reload_free (self->reload);
	lisys_free (self);
}

/** @} */
/** @} */
/** @} */
