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
 * \addtogroup liextsrvSpawner Spawner    
 * @{
 */

#ifndef __EXT_SPAWNER_H__
#define __EXT_SPAWNER_H__

#include <server/lips-server.h>
#include <system/lips-system.h>
#include "ext-module.h"

/**
 * \brief Per object spawner logic.
 */
typedef struct _liextSpawner liextSpawner;
struct _liextSpawner
{
	int active;
	int limit;
	float delay;
	float timer;
	licalHandle calls[1];
	liengObject* object;
	liextModule* module;
	liscrData* script;
	lisrvServer* server;
	struct
	{
		int count;
		liscrData** array;
	} spawns;
};

liextSpawner*
liext_spawner_new (liextModule* module);


void
liext_spawner_free (liextSpawner* self);

int
liext_spawner_serialize (liextSpawner*   self,
                         liarcSerialize* serialize);

int
liext_spawner_get_active (liextSpawner* self);

int
liext_spawner_set_active (liextSpawner* self,
                          int           value);

int
liext_spawner_set_object (liextSpawner* self,
                          liengObject*  object);

int
liext_spawner_set_limit (liextSpawner* self,
                         int           value);

#endif

/** @} */
/** @} */
/** @} */
