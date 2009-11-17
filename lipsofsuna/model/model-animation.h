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

#include <math/lips-math.h>

typedef struct _limdlFrame limdlFrame;
struct _limdlFrame
{
	limatTransform transform;
};

typedef struct _limdlAnimation limdlAnimation;
struct _limdlAnimation
{
	int length;
	char* name;
	float blendin;
	float blendout;
	struct
	{
		int count;
		limdlFrame* array;
	} buffer;
	struct
	{
		int count;
		char** array;
	} channels;
};

int
limdl_animation_get_channel (limdlAnimation* self,
                             const char*     name);

float
limdl_animation_get_duration (const limdlAnimation* self);

int
limdl_animation_get_transform (limdlAnimation* self,
                               const char*     name,
                               float           secs,
                               limatTransform* value);

float
limdl_animation_get_weight (const limdlAnimation* self,
                            float                 time,
                            float                 sweight,
                            float                 mweight,
                            float                 eweight);

#endif

/** @} */
/** @} */
