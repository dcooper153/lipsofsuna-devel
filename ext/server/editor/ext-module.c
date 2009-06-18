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
 * \addtogroup liextsrvEditor Editor      
 * @{
 */

#include <server/lips-server.h>
#include "ext-editor.h"
#include "ext-module.h"

lisrvExtensionInfo liextInfo =
{
	LISRV_EXTENSION_VERSION, "Editor",
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
	self->editor = liext_editor_new (server);
	if (self->editor == NULL)
	{
		free (self);
		return NULL;
	}

	liscr_script_create_class (server->script, "Editor", liextEditorScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	/* FIXME: Remove the class here. */
	liext_editor_free (self->editor);
	free (self);
}

/** @} */
/** @} */
/** @} */
