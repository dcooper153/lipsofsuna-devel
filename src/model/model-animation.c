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
 * \addtogroup limdlAnimation Animation
 * @{
 */

#include <stdio.h>
#include "model-animation.h"

/**
 * \brief Finds an interpolation curve by name.
 *
 * \param self Animation.
 * \param name Name.
 * \return Interpolation curve or NULL.
 */
limdlIpo*
limdl_animation_find_curve (limdlAnimation* self,
                            const char*     name)
{
	int i;
	limdlIpo* ipo;

	for (i = 0 ; i < self->ipos.count ; i++)
	{
		ipo = self->ipos.array + i;
		if (!strcmp (ipo->name, name))
			return ipo;
	}

	return NULL;
}

/**
 * \brief Gets the duration of the animation in seconds.
 *
 * \param self Animation.
 * \return Duration in seconds.
 */
float
limdl_animation_get_duration (const limdlAnimation* self)
{
	return self->duration;
}

/**
 * \brief Gets the weight of the animation at certain point.
 *
 * The three weights passed allow different blending patters,
 * mostly needed for looping:
 *
 * - Loop once: [0,1,0]
 * - Loop twice: [0,1,1] [1,1,0]
 * - Loop thrice: [0,1,1] [1,1,1] [1,1,0]
 * - Loop forever: [1,1,1]
 *
 * \param self Animation.
 * \param time Time into the animation in seconds.
 * \param sweight Starting weight.
 * \param mweight Middle weight.
 * \param eweight Ending weight.
 * \return Value within range from 0.0 to 1.0.
 */
float
limdl_animation_get_weight (const limdlAnimation* self,
                            float                 time,
                            float                 sweight,
                            float                 mweight,
                            float                 eweight)
{
	if (time < 0.0f)
		return sweight;
	if (time < self->blendin)
		return sweight + (mweight - sweight) * (time / self->blendin);
	if (time < self->duration - self->blendout)
		return mweight;
	if (time < self->duration)
		return mweight + (eweight - mweight) * (1.0f - (self->duration - time) / self->blendout);
	return eweight;
}

/** @} */
/** @} */
