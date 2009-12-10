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
 * \addtogroup liextsrvNpc Npc      
 * @{
 */

#ifndef __EXT_NPC_H__
#define __EXT_NPC_H__

#include <server/lips-server.h>
#include <system/lips-system.h>
#include "ext-module.h"

struct _liextNpc
{
	licalHandle calls[1];
	liengObject* object;
	liengObject* target;
	liextModule* module;
	liscrData* data;
	int active;
	int alert;
	float radius;
	float refresh;
	float timer;
};

liextNpc*
liext_npc_new (liextModule* module);

void
liext_npc_free (liextNpc* self);

int
liext_npc_get_active (liextNpc* self);

int
liext_npc_set_active (liextNpc* self,
                      int       value);

int
liext_npc_set_object (liextNpc*    self,
                      liengObject* object);

void
liext_npc_set_target (liextNpc*    self,
                      liengObject* object);

#endif

/** @} */
/** @} */
/** @} */
