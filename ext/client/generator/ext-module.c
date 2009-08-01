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
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#include <client/lips-client.h>
#include "ext-dialog.h"
#include "ext-module.h"

licliExtensionInfo liextInfo =
{
	LICLI_EXTENSION_VERSION, "Generator",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliModule* module)
{
	liextModule* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->module = module;

	/* Create dialog. */
	self->dialog = liext_dialog_new (module->widgets, self);
	if (self->dialog == NULL)
	{
		free (self);
		return NULL;
	}
	liwdg_manager_insert_window (module->widgets, self->dialog);
	liwdg_widget_set_visible (self->dialog, 0);

	/* Register scripts. */
	liscr_script_create_class (module->script, "Generator", liextGeneratorScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	/* FIXME: Remove the class here. */
	liwdg_manager_remove_window (self->module->widgets, self->dialog);
	liwdg_widget_free (self->dialog);
	free (self);
}

int
liext_module_save (liextModule* self)
{
	return liext_dialog_save (LIEXT_DIALOG (self->dialog));
}

/** @} */
/** @} */
/** @} */
