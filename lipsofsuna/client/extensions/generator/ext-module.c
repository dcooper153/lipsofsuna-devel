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

static int
private_packet (liextModule* self,
                int          type,
                liarcReader* reader);

/*****************************************************************************/

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
	self = lisys_calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->module = module;

	/* Create dialog. */
	self->editor = liext_editor_new (module->widgets, self);
	if (self->editor == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lieng_engine_insert_call (module->engine, LICLI_CALLBACK_PACKET, 100,
	     	private_packet, self, self->calls + 0))
	{
		liwdg_widget_free (self->editor);
		lisys_free (self);
		return NULL;
	}

	/* Register scripts. */
	liscr_script_create_class (module->script, "Generator", liextGeneratorScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	/* Remove callbacks. */
	lieng_engine_remove_calls (self->module->engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));

	/* FIXME: Remove the class here. */
	if (self->script == NULL)
		liwdg_widget_free (self->editor);
	lisys_free (self);
}

int
liext_module_save (liextModule* self)
{
	return liext_editor_save (LIEXT_EDITOR (self->editor));
}

/*****************************************************************************/

static int
private_packet (liextModule* self,
                int          type,
                liarcReader* reader)
{
	reader->pos = 1;
	if (type == LIEXT_VOXEL_PACKET_ASSIGN)
		liext_editor_reset (LIEXT_EDITOR (self->editor), reader);

	return 1;
}

/** @} */
/** @} */
/** @} */
