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
 * \addtogroup LIExtSpawner Spawner    
 * @{
 */

#ifndef __EXT_SPAWNER_H__
#define __EXT_SPAWNER_H__

#include "ext-module.h"

/**
 * \brief Per object spawner logic.
 */
struct _LIExtSpawner
{
	int active;
	int limit;
	float delay;
	float timer;
	LICalHandle calls[1];
	LIEngObject* owner;
	LIExtModule* module;
	LIScrData* script;
	struct
	{
		int count;
		LIScrData** array;
	} spawns;
};

LIExtSpawner*
liext_spawner_new (LIExtModule* module);


void
liext_spawner_free (LIExtSpawner* self);

int
liext_spawner_get_active (LIExtSpawner* self);

int
liext_spawner_set_active (LIExtSpawner* self,
                          int           value);

int
liext_spawner_set_owner (LIExtSpawner* self,
                         LIEngObject*  value);

int
liext_spawner_set_limit (LIExtSpawner* self,
                         int           value);

#endif

/** @} */
/** @} */
