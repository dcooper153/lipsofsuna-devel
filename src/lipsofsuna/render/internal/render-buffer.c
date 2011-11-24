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

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenBuffer Buffer
 * @{
 */

#include "render-internal.h"

/**
 * \brief Allocates a vertex buffer.
 * \param render Renderer.
 * \param index_data Pointer to an array of 32-bit unsigned indices, or NULL.
 * \param index_count Number of indices to allocate.
 * \param vertex_format Vertex format.
 * \param vertex_data Pointer to vertex format specific array of vertices, or NULL.
 * \param vertex_count Number of vertices to allocate.
 * \param type Usage type.
 * \return Vertex buffer or NULL.
 */
LIRenBuffer* liren_buffer_new (
	LIRenRender*       render,
	const void*        index_data,
	int                index_count,
	const LIRenFormat* vertex_format,
	const void*        vertex_data,
	int                vertex_count,
	int                type)
{
	LIRenBuffer* self;

	lisys_assert (vertex_format->vtx_format);

	self = lisys_calloc (1, sizeof (LIRenBuffer));
	if (self == NULL)
		return NULL;

	/* Initialize the backed. */
	/* TODO */

	return self;
}

/**
 * \brief Frees the vertex buffer.
 * \param self Buffer.
 */
void liren_buffer_free (
	LIRenBuffer* self)
{
	/* TODO */
	lisys_free (self);
}

/** @} */
/** @} */
/** @} */
