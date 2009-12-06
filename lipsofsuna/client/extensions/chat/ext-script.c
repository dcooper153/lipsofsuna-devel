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
 * -- @param table Optional table of arguments.
 * -- @return New chat history widget.
 * function ChatHistory.new(self, table)
 */
static int
ChatHistory_new (lua_State* lua)
{
	liextModule* module;
	liscrData* self;
	liscrScript* script;
	liwdgWidget* widget;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CHAT_HISTORY);

	/* Allocate widget. */
	widget = liext_chat_history_new (module->module->widgets, module->module);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Allocate userdata. */
	self = liscr_data_new (script, widget, LIEXT_SCRIPT_CHAT_HISTORY, licli_script_widget_free);
	if (self == NULL)
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
	}
	liwdg_widget_set_userdata (widget, self);

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liscr_pushdata (lua, self);
	return 1;
}

/* @luadoc
 * ---
 * -- Appends a message to the chat history widget.
 * --
 * -- @param self Chat history widget.
 * -- @param message Message.
 * function ChatHistory.append(self, message)
 */
static int
ChatHistory_append (lua_State* lua)
{
	const char* str;
	liscrData* data;

	data = liscr_checkdata (lua, 1, LIEXT_SCRIPT_CHAT_HISTORY);
	str = luaL_checkstring (lua, 2);

	liext_chat_history_append (data->data, str);
	return 0;
}

/*****************************************************************************/

void
liextChatHistoryScript (liscrClass* self,
                        void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_CHAT_HISTORY, data);
	liscr_class_inherit (self, licliWidgetScript, module->module);
	liscr_class_insert_func (self, "new", ChatHistory_new);
	liscr_class_insert_func (self, "append", ChatHistory_append);
}

/** @} */
/** @} */
/** @} */
