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
 * \addtogroup lisndSystem System
 * @{
 */

#ifndef __SOUND_SYSTEM_H__
#define __SOUND_SYSTEM_H__

#include <AL/al.h>
#include <AL/alc.h>
#include <math/lips-math.h>

typedef struct _lisndSystem lisndSystem;
struct _lisndSystem
{
	ALCdevice* device;
	ALCcontext* context;
};

lisndSystem*
lisnd_system_new ();

void
lisnd_system_free (lisndSystem* self);

void
lisnd_system_set_listener (lisndSystem*       self,
                           const limatVector* pos,
                           const limatVector* vel,
                           const limatVector* dir,
                           const limatVector* up);

#endif

/** @} */
/** @} */

