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
 * \addtogroup lisnd Sound 
 * @{
 * \addtogroup lisndManager Manager 
 * @{
 */

#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

#include <algorithm/lips-algorithm.h>
#include "sound-sample.h"
#include "sound-system.h"

typedef struct _lisndManager lisndManager;
struct _lisndManager
{
	lisndSystem* system;
	lialgStrdic* samples;
};

lisndManager*
lisnd_manager_new (lisndSystem* system);

void
lisnd_manager_free (lisndManager* self);

void
lisnd_manager_clear (lisndManager* self);

void
lisnd_manager_set_listener (lisndManager*      self,
                            const limatVector* pos,
                            const limatVector* vel,
                            const limatVector* dir,
                            const limatVector* up);

lisndSample*
lisnd_manager_get_sample (lisndManager* self,
                          const char*   name);

int
lisnd_manager_set_sample (lisndManager* self,
                          const char*   name,
                          const char*   path);

#endif

/** @} */
/** @} */

