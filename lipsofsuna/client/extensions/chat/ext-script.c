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
#include <script/lips-script.h>
#include "ext-history.h"
#include "ext-module.h"

/* @luadoc
 * ---
 * -- Display chat messages.
 * -- @name ChatHistory
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new chat history widget.
 * --
 * -- @param self Chat history class.
 * -- @param args Arguments.
 * -- @return New chat history widget.
 * function ChatHistory.new(self, args)
 */
static void ChatHistory_new (liscrArgs* args)
{
	liextModule* module;
	liscrData* data;
	liwdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CHAT_HISTORY);
	self = liext_chat_history_new (module->module->widgets, module->module);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_CHAT_HISTORY, licli_script_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Appends a message to the chat history widget.
 * --
 * -- Arguments:
 * -- text: String.
 * --
 * -- @param self Chat history widget.
 * -- @param args Arguments.
 * function ChatHistory.append(self, args)
 */
static void ChatHistory_append (liscrArgs* args)
{
	const char* str;

	if (liscr_args_gets_string (args, "text", &str))
		liext_chat_history_append (args->self, str);
}

/*****************************************************************************/

void
liextChatHistoryScript (liscrClass* self,
                        void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_CHAT_HISTORY, data);
	liscr_class_inherit (self, licliWidgetScript, module->module);
	liscr_class_insert_cfunc (self, "new", ChatHistory_new);
	liscr_class_insert_mfunc (self, "append", ChatHistory_append);
}

/** @} */
/** @} */
/** @} */
