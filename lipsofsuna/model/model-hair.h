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
 * \addtogroup limdlHair Hair
 * @{
 */

#ifndef __MODEL_HAIR_H__
#define __MODEL_HAIR_H__

#include <archive/lips-archive.h>
#include <math/lips-math.h>
#include <string/lips-string.h>
#include "model-types.h"

struct _limdlHairNode
{
	limatVector position;
	float size;
};

struct _limdlHair
{
	int count;
	limdlHairNode* nodes;
};

#ifdef __cplusplus
extern "C" {
#endif

int
limdl_hair_read (limdlHair* self,
                 liReader*  reader);

int
limdl_hair_write (limdlHair*   self,
                  liarcWriter* writer);

#ifdef __cplusplus
}
#endif

struct _limdlHairs
{
	int material;
	int count;
	limdlHair* hairs;
};

#ifdef __cplusplus
extern "C" {
#endif

void
limdl_hairs_free (limdlHairs* self);

int
limdl_hairs_read (limdlHairs* self,
                  liReader*   reader);

int
limdl_hairs_write (limdlHairs*  self,
                   liarcWriter* writer);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
