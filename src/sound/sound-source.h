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
 * \addtogroup lisndSource Source
 * @{
 */

#ifndef __SOUND_SOURCE_H__
#define __SOUND_SOURCE_H__

#include "sound-sample.h"
#include "sound-system.h"
#include <math/lips-math.h>

typedef struct _lisndSource lisndSource;
struct _lisndSource
{
	ALuint source;
	ALint queued;
};

lisndSource*
lisnd_source_new (lisndSystem* system);

lisndSource*
lisnd_source_new_with_sample (lisndSystem* system,
                              lisndSample* sample);

void
lisnd_source_free (lisndSource* self);

void
lisnd_source_queue_sample (lisndSource* self,
                           lisndSample* sample);

int
lisnd_source_update (lisndSource* self);

void
lisnd_source_set_looping (lisndSource* self,
                          int          looping);

int
lisnd_source_get_playing (lisndSource* self);

void
lisnd_source_set_playing (lisndSource* self,
                          int          playing);

void
lisnd_source_set_position (lisndSource*       self,
                           const limatVector* value);

void
lisnd_source_set_velocity (lisndSource*       self,
                           const limatVector* value);

#endif

/** @} */
/** @} */

