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
 * \addtogroup lisnd Sound 
 * @{
 * \addtogroup LISndSource Source
 * @{
 */

#ifndef __SOUND_SOURCE_H__
#define __SOUND_SOURCE_H__

#include "sound-sample.h"
#include "sound-system.h"
#include <lipsofsuna/math.h>

typedef struct _LISndSource LISndSource;
struct _LISndSource
{
	ALuint source;
	ALint queued;
};

LISndSource*
lisnd_source_new (LISndSystem* system);

LISndSource*
lisnd_source_new_with_sample (LISndSystem* system,
                              LISndSample* sample);

void
lisnd_source_free (LISndSource* self);

void
lisnd_source_queue_sample (LISndSource* self,
                           LISndSample* sample);

int
lisnd_source_update (LISndSource* self);

void
lisnd_source_set_looping (LISndSource* self,
                          int          looping);

int
lisnd_source_get_playing (LISndSource* self);

void
lisnd_source_set_playing (LISndSource* self,
                          int          playing);

void
lisnd_source_set_position (LISndSource*       self,
                           const LIMatVector* value);

void
lisnd_source_set_velocity (LISndSource*       self,
                           const LIMatVector* value);

void
lisnd_source_set_volume (LISndSource* self,
                         float        value);

#endif

/** @} */
/** @} */

