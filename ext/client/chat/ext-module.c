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
 * \addtogroup liextcliChat Chat          
 * @{
 */

#include <client/lips-client.h>
#include "ext-entry.h"
#include "ext-history.h"
#include "ext-module.h"

licliExtensionInfo liextInfo =
{
	LICLI_EXTENSION_VERSION, "Chat",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliModule* module)
{
	liextModule* self;

	self = calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->module = module;

	liscr_script_insert_class (module->script, "Chat", liextChatScript, self);
	liscr_script_insert_class (module->script, "ChatEntry", liextChatEntryScript, self);
	liscr_script_insert_class (module->script, "ChatHistory", liextChatHistoryScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	/* FIXME: Remove the class here. */
	free (self);
}

int
liext_module_send (liextModule* self,
                   const char*  message)
{
	liarcWriter* writer;

	writer = liarc_writer_new_packet (LI_CLIENT_COMMAND_CHAT);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint8 (writer, 0);
	liarc_writer_append_string (writer, message);
	licli_module_send (self->module, writer, GRAPPLE_RELIABLE);
	liarc_writer_free (writer);

	return 1;
}

/** @} */
/** @} */
/** @} */
