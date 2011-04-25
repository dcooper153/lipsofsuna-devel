/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#ifndef __RENDER32_TEXTURE_H__
#define __RENDER32_TEXTURE_H__

#include <lipsofsuna/image.h>
#include <lipsofsuna/model.h>
#include "render.h"
#include "render-types.h"

struct _LIRenTexture32
{
	int id;
	int width;
	int height;
	LIMdlTextureType type;
	LIRenImage32* image;
};

LIAPICALL (void, liren_texture32_init, (
	LIRenTexture32* self));

LIAPICALL (void, liren_texture32_free, (
	LIRenTexture32* self));

LIAPICALL (void, liren_texture32_set_image, (
	LIRenTexture32* self,
	LIRenImage32*   value));

LIAPICALL (GLenum, liren_texture32_get_target, (
	const LIRenTexture32* self));

LIAPICALL (GLuint, liren_texture32_get_texture, (
	const LIRenTexture32* self));

#endif
