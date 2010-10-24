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

#ifndef __RENDER_BUFFER_H__
#define __RENDER_BUFFER_H__

#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/video.h>
#include "render-material.h"
#include "render-types.h"

enum
{
	LIREN_BUFFER_TYPE_DYNAMIC,
	LIREN_BUFFER_TYPE_STATIC,
	LIREN_BUFFER_TYPE_STREAM
};

struct _LIRenBuffer
{
	int type;
	GLuint index_buffer;
	GLuint vertex_array;
	GLuint vertex_buffer;
	LIRenFormat vertex_format;
	struct
	{
		int count;
	} indices;
	struct
	{
		int count;
	} vertices;
};

LIAPICALL (int, liren_buffer_init, (
	LIRenBuffer*       self,
	const void*        index_data,
	int                index_count,
	const LIRenFormat* vertex_format,
	const void*        vertex_data,
	int                vertex_count,
	int                type));

LIAPICALL (void, liren_buffer_free, (
	LIRenBuffer* self));

LIAPICALL (void*, liren_buffer_lock_indices, (
	LIRenBuffer* self,
	int          write));

LIAPICALL (void*, liren_buffer_lock_vertices, (
	LIRenBuffer* self,
	int          write));

LIAPICALL (void, liren_buffer_unlock_indices, (
	LIRenBuffer* self,
	void*        data));

LIAPICALL (void, liren_buffer_unlock_vertices, (
	LIRenBuffer* self,
	void*        data));

LIAPICALL (void, liren_buffer_upload_vertices, (
	LIRenBuffer* self,
	int          start,
	int          count,
	const void*  data));

LIAPICALL (int, liren_buffer_get_size, (
	LIRenBuffer* self));

#endif
