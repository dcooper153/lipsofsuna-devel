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

#ifndef __RENDER_BUFFER_TEXTURE_H__
#define __RENDER_BUFFER_TEXTURE_H__

#include "lipsofsuna/video.h"

typedef struct _LIRenBufferTexture LIRenBufferTexture;
struct _LIRenBufferTexture
{
	int size;
	GLuint buffer;
	GLuint texture;
};

LIAPICALL (void, liren_buffer_texture_init, (
	LIRenBufferTexture* self,
	void*               data,
	int                 size));

LIAPICALL (void, liren_buffer_texture_clear, (
	LIRenBufferTexture* self));

LIAPICALL (void, liren_buffer_texture_upload, (
	LIRenBufferTexture* self,
	int                 size,
	void*               data));

#endif
