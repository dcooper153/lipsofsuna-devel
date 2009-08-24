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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <script/lips-script.h>

#define LIEXT_SCRIPT_CHAT_HISTORY "Lips.ChatHistory"

typedef struct _liextModule liextModule;
struct _liextModule
{
	licliModule* module;
};

liextModule*
liext_module_new (licliModule* module);

void
liext_module_free (liextModule* self);

/*****************************************************************************/

void
liextChatHistoryScript (liscrClass* self,
                        void*       data);

#endif

/** @} */
/** @} */
/** @} */
