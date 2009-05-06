/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlIpo Ipo
 * @{
 */

#include <stdio.h>
#include <assert.h>
#include "model-ipo.h"

float
limdl_ipo_get_duration (const limdlIpo* self)
{
	int i;
	float tmp;
	float duration = 0.0f;

	for (i = 0 ; i < LIMDL_IPO_CHANNEL_NUM ; i++)
	{
		tmp = limdl_ipo_channel_get_duration (self->channels + i);
		if (duration < tmp)
			duration = tmp;
	}

	return duration;
}

void
limdl_ipo_get_location (const limdlIpo* self,
                        float           time,
                        limatVector*    vector)
{
	vector->x = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_LOCX, time);
	vector->y = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_LOCY, time);
	vector->z = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_LOCZ, time);
}

void
limdl_ipo_get_quaternion (const limdlIpo*  self,
                          float            time,
                          limatQuaternion* quaternion)
{
	quaternion->x = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_QUATX, time);
	quaternion->y = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_QUATY, time);
	quaternion->z = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_QUATZ, time);
	quaternion->w = limdl_ipo_channel_get_value (self->channels + LIMDL_IPO_CHANNEL_QUATW, time);
	*quaternion = limat_quaternion_normalize (*quaternion);
}

/** @} */
/** @} */
