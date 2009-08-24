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

#ifndef __MODEL_ANIMATION_H__
#define __MODEL_ANIMATION_H__

#include "model-ipo.h"

typedef struct _limdlAnimation limdlAnimation;
struct _limdlAnimation
{
	char* name;
	float duration;
	float blendin;
	float blendout;
	struct
	{
		int count;
		limdlIpo* array;
	} ipos;
};

limdlIpo*
limdl_animation_find_curve (limdlAnimation* self,
                            const char*     name);

float
limdl_animation_get_duration (const limdlAnimation* self);

float
limdl_animation_get_weight (const limdlAnimation* self,
                            float                 time,
                            float                 sweight,
                            float                 mweight,
                            float                 eweight);

#endif

/** @} */
/** @} */
