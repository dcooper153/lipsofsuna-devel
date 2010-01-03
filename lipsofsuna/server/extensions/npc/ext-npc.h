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
 * \addtogroup liextsrvNpc Npc      
 * @{
 */

#ifndef __EXT_NPC_H__
#define __EXT_NPC_H__

#include <lipsofsuna/server.h>
#include <lipsofsuna/system.h>
#include "ext-module.h"

struct _LIExtNpc
{
	LICalHandle calls[1];
	LIEngObject* owner;
	LIEngObject* target;
	LIExtModule* module;
	LIScrData* path;
	LIScrData* script;
	int active;
	int alert;
	float radius;
	float refresh;
	float timer;
};

LIExtNpc*
liext_npc_new (LIExtModule* module);

void
liext_npc_free (LIExtNpc* self);

int
liext_npc_get_active (LIExtNpc* self);

int
liext_npc_set_active (LIExtNpc* self,
                      int       value);

int
liext_npc_set_owner (LIExtNpc*    self,
                     LIEngObject* value);

void
liext_npc_set_path (LIExtNpc*  self,
                    LIScrData* path);

void
liext_npc_set_target (LIExtNpc*    self,
                      LIEngObject* object);

#endif

/** @} */
/** @} */
/** @} */
