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

#ifndef __EXT_HISTORY_H__
#define __EXT_HISTORY_H__

#include <script/lips-script.h>
#include <widget/lips-widget.h>

#define LIEXT_WIDGET_CHAT_HISTORY(o) ((liextChatHistory*)(o))

typedef struct _liextChatHistory liextChatHistory;
struct _liextChatHistory
{
	liwdgLabel base;
	licliModule* module;
	struct
	{
		int count;
		char** array;
	} lines;
};

liwdgWidget*
liext_chat_history_new (liwdgManager* manager,
                        licliModule*  module);

int
liext_chat_history_append (liextChatHistory* self,
                           const char*       message);

#endif

/** @} */
/** @} */
/** @} */
