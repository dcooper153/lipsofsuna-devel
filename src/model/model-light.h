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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlLight Light
 * @{
 */

#ifndef __MODEL_LIGHT_H__
#define __MODEL_LIGHT_H__

#include <archive/lips-archive.h>
#include <math/lips-math.h>
#include <string/lips-string.h>
#include "model-types.h"

struct _limdlLight
{
	int flags;
	float color[3];
	float equation[3];
	struct
	{
		float fov;
		float nearz;
		float farz;
	} projection;
	struct
	{
		float cutoff;
		float exponent;
	} spot;
};

#ifdef __cplusplus
extern "C" {
#endif

int
limdl_light_read (limdlNode* self,
                  liReader*  reader);

int
limdl_light_write (const limdlNode* self,
                   liarcWriter*     writer);

void
limdl_light_get_modelview (const limdlNode* self,
                           limatMatrix*     value);

void
limdl_light_get_projection (const limdlNode* self,
                            limatMatrix*     value);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
