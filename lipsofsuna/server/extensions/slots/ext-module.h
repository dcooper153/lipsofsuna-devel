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
 * \addtogroup liextsrvSlots Slots
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <script/lips-script.h>

#define LIEXT_SCRIPT_SLOTS "Lips.Slots"

typedef enum _liextSlotType liextSlotType;
typedef struct _liextSlot liextSlot;
typedef struct _liextSlots liextSlots;
typedef struct _liextModule liextModule;

struct _liextModule
{
	lialgPtrdic* dictionary;
	licalHandle calls[1];
	lisrvServer* server;
};

liextModule*
liext_module_new (lisrvServer* server);

void
liext_module_free (liextModule* self);

/*****************************************************************************/

void
liextSlotsScript (liscrClass* self,
                  void*       data);

#endif

/** @} */
/** @} */
/** @} */
