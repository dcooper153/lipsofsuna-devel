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

#ifndef __RENDER_MESH_H__
#define __RENDER_MESH_H__

#include "lipsofsuna/system.h"
#include "lipsofsuna/video.h"
#include "render-types.h"

typedef struct _LIRenMesh LIRenMesh;
struct _LIRenMesh
{
	int sizes[3];
	int counts[3];
	GLuint buffers[3];
	GLuint arrays[2];
};

LIAPICALL (int, liren_mesh_init, (
	LIRenMesh* self,
	void*      index_data,
	int        index_count,
	void*      vertex_data,
	int        vertex_count));

LIAPICALL (void, liren_mesh_clear, (
	LIRenMesh* self));

LIAPICALL (void, liren_mesh_deform, (
	LIRenMesh* self));

LIAPICALL (void*, liren_mesh_lock_vertices, (
	const LIRenMesh* self));

LIAPICALL (void, liren_mesh_unlock_vertices, (
	const LIRenMesh* self));

LIAPICALL (void, liren_mesh_get_format, (
	const LIRenMesh* self,
	LIRenFormat*     value));

#endif

