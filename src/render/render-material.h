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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndMaterial Material
 * @{
 */

#ifndef __RENDER_MATERIAL_H__
#define __RENDER_MATERIAL_H__

#include <image/lips-image.h>
#include "render-image.h"
#include "render-shader.h"
#include "render-types.h"

enum
{
	LIRND_MATERIAL_FLAG_BILLBOARD = 0x010000,
	LIRND_MATERIAL_FLAG_CULLFACE = 0x020000,
	LIRND_MATERIAL_FLAG_TRANSPARENCY = 0x040000,
};

struct _lirndMaterial
{
	int flags;
	float parameters[4];
	float shininess;
	float diffuse[4];
	float specular[4];
	float strand_start;
	float strand_end;
	float strand_shape;
	lirndShader* shader;
	struct
	{
		int count;
		lirndTexture* array;
	} textures;
};

lirndMaterial*
lirnd_material_new ();

void
lirnd_material_free (lirndMaterial* self);

void
lirnd_material_set_flags (lirndMaterial* self,
                          int            flags);

int
lirnd_material_set_shader (lirndMaterial* self,
                           lirndShader*   shader);

void
lirnd_material_set_texture (lirndMaterial* self,
                            int            index,
                            limdlTexture*  texture,
                            lirndImage*    image);

int
lirnd_material_set_texture_count (lirndMaterial* self,
                                  int            value);

#endif

/** @} */
/** @} */
