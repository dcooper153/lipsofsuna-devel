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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSlots Slots
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/client.h>

#define LIEXT_SCRIPT_SLOTS "Slots"

typedef struct _LIExtModule LIExtModule;
struct _LIExtModule
{
	LIAlgU32dic* dictionary;
	LICalHandle calls[1];
	LICliClient* client;
};

LIExtModule*
liext_module_new (LIMaiProgram* program);

void
liext_module_free (LIExtModule* self);

void
liext_module_clear_slots (LIExtModule* self,
                          LIEngObject* owner);

int
liext_module_set_slots (LIExtModule* self,
                        LIEngObject* owner,
                        const char*  node,
                        const char*  model);

/*****************************************************************************/

void
liext_script_slots (LIScrClass* self,
                    void*       data);

#endif

/** @} */
/** @} */
/** @} */
