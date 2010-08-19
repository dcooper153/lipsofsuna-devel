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
 * \addtogroup LIRenBuffer Buffer
 * @{
 */

#ifndef __RENDER_BUFFER_H__
#define __RENDER_BUFFER_H__

#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/video.h>
#include "render-material.h"
#include "render-types.h"

enum
{
	LIREN_BUFFER_TYPE_CPU,
	LIREN_BUFFER_TYPE_GPU
};

struct _LIRenBuffer
{
	GLuint buffer;
	GLenum target;
	LIRenFormat format;
	struct
	{
		int count;
		void* array;
	} elements;
};

LIAPICALL (int, liren_buffer_init_index, (
	LIRenBuffer* self,
	const void*  data,
	int          count,
	int          type));

LIAPICALL (int, liren_buffer_init_vertex, (
	LIRenBuffer*       self,
	const LIRenFormat* format,
	const void*        data,
	int                count,
	int                type));

LIAPICALL (void, liren_buffer_free, (
	LIRenBuffer* self));

LIAPICALL (void*, liren_buffer_lock, (
	LIRenBuffer* self,
	int          write));

LIAPICALL (void, liren_buffer_unlock, (
	LIRenBuffer* self,
	void*        data));

#endif

/** @} */
/** @} */
