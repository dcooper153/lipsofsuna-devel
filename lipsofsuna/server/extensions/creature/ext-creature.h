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
 * \addtogroup liextsrvCreature Creature
 * @{
 */

#ifndef __EXT_CREATURE_H__
#define __EXT_CREATURE_H__

#include <server/lips-server.h>
#include <system/lips-system.h>

typedef struct _liextCreature liextCreature;
struct _liextCreature
{
	int active;
	int controls;
	licalHandle calls[1];
	liengObject* object;
	liscrData* data;
	lisrvServer* server;
};

liextCreature*
liext_creature_new (lisrvServer* server);

void
liext_creature_free (liextCreature* self);

int
liext_creature_get_active (liextCreature* self);

int
liext_creature_set_active (liextCreature* self,
                           int            value);

int
liext_creature_set_object (liextCreature* self,
                           liengObject*   object);

#endif

/** @} */
/** @} */
/** @} */
