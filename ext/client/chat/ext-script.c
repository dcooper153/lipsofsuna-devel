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
#include "ext-entry.h"
#include "ext-history.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Chat"
 * ---
 * -- Send chat messages.
 * -- @name ChatEntry
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Sends a chat message to the server.
 * --
 * -- @param self Chat class.
 * -- @param message String.
 * function Chat.send(self, message)
 */
static int
Chat_send (lua_State* lua)
{
	const char* value;
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CHAT);
	value = luaL_checkstring (lua, 2);
	/* TODO: Send flags. */

	liext_module_send (module, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Type chat messages.
 * -- @name ChatEntry
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new chat entry widget.
 * --
 * -- @param self Chat entry class.
 * -- @param table Optional table of arguments.
 * -- @return New chat entry widget.
 * function ChatEntry.new(self, table)
 */
static int
ChatEntry_new (lua_State* lua)
{
	liextModule* module;
	liscrData* self;
	liscrScript* script;
	liwdgWidget* widget;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CHAT_ENTRY);

	/* Allocate widget. */
	widget = liext_chat_entry_new (module->module->widgets, module);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Allocate userdata. */
	self = liscr_data_new (script, widget, LIEXT_SCRIPT_CHAT_ENTRY);
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
	liscr_data_unref (self, NULL);

	return 1;
}

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
	self = liscr_data_new (script, widget, LIEXT_SCRIPT_CHAT_HISTORY);
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

/*****************************************************************************/

void
liextChatScript (liscrClass* self,
                 void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_CHAT, data);
	liscr_class_insert_func (self, "send", Chat_send);
}

void
liextChatEntryScript (liscrClass* self,
                      void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_CHAT_ENTRY, data);
	liscr_class_inherit (self, licliWidgetScript);
	liscr_class_insert_func (self, "new", ChatEntry_new);
}

void
liextChatHistoryScript (liscrClass* self,
                        void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_CHAT_HISTORY, data);
	liscr_class_inherit (self, licliWidgetScript);
	liscr_class_insert_func (self, "new", ChatHistory_new);
}

/** @} */
/** @} */
/** @} */
