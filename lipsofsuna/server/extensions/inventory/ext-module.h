/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup liextsrvInventory Inventory 
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/engine.h>
#include <lipsofsuna/script.h>
#include <lipsofsuna/server.h>

#define LIEXT_SCRIPT_INVENTORY "Lips.Inventory"

typedef struct _LIExtInventory LIExtInventory;
typedef struct _LIExtModule LIExtModule;

struct _LIExtModule
{
	LIAlgU32dic* dictionary;
	LICalHandle calls[2];
	LISerServer* server;
};

LIExtModule*
liext_module_new (LISerServer* server);

void
liext_module_free (LIExtModule* self);

LIExtInventory*
liext_module_find_inventory (LIExtModule* self,
                             uint32_t     id);

/*****************************************************************************/

void
liext_script_skill (LIScrClass* self,
                  void*       data);

void
liext_script_inventory (LIScrClass* self,
                      void*       data);

#endif

/** @} */
/** @} */
/** @} */
