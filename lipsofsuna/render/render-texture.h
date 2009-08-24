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
 * \addtogroup lirndTexture Texture
 * @{
 */

#ifndef __RENDER_TEXTURE_H__
#define __RENDER_TEXTURE_H__

#include <image/lips-image.h>
#include <model/lips-model.h>
#include "render.h"
#include "render-types.h"

struct _lirndTexture
{
	int id;
	int width;
	int height;
	char* name;
	GLuint texture;
	limdlTextureType type;
	limdlTextureFlags flags;
	lirndImage* image;
	struct
	{
		GLenum magfilter;
		GLenum minfilter;
		GLenum wraps;
		GLenum wrapt;
	} params;
};

void
lirnd_texture_init (lirndTexture* self,
                    limdlTexture* texture);

void
lirnd_texture_free (lirndTexture* self);

void
lirnd_texture_set_image (lirndTexture* self,
                         lirndImage*   value);

#endif

/** @} */
/** @} */
