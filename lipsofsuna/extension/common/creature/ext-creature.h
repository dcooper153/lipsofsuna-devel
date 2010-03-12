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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtCreature Creature
 * @{
 */

#ifndef __EXT_CREATURE_H__
#define __EXT_CREATURE_H__

#include "ext-module.h"

typedef struct _LIExtCreature LIExtCreature;
struct _LIExtCreature
{
	int active;
	float move;
	LICalHandle calls[1];
	LIEngObject* object;
	LIExtModule* module;
	LIScrData* script;
};

LIExtCreature*
liext_creature_new (LIExtModule* module);

void
liext_creature_free (LIExtCreature* self);

int
liext_creature_get_active (LIExtCreature* self);

int
liext_creature_set_active (LIExtCreature* self,
                           int            value);

int
liext_creature_set_object (LIExtCreature* self,
                           LIEngObject*   object);

#endif

/** @} */
/** @} */
