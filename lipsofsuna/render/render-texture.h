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
 * \addtogroup liren Render
 * @{
 * \addtogroup LIRenTexture Texture
 * @{
 */

#ifndef __RENDER_TEXTURE_H__
#define __RENDER_TEXTURE_H__

#include <lipsofsuna/image.h>
#include <lipsofsuna/model.h>
#include "render.h"
#include "render-types.h"

struct _LIRenTexture
{
	int id;
	int width;
	int height;
	char* name;
	GLuint texture;
	LIMdlTextureType type;
	LIMdlTextureFlags flags;
	LIRenImage* image;
	struct
	{
		GLenum magfilter;
		GLenum minfilter;
		GLenum wraps;
		GLenum wrapt;
	} params;
};

void
liren_texture_init (LIRenTexture* self,
                    LIMdlTexture* texture);

void
liren_texture_free (LIRenTexture* self);

void
liren_texture_set_image (LIRenTexture* self,
                         LIRenImage*   value);

#endif

/** @} */
/** @} */
