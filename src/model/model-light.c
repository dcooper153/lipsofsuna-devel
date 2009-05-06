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
 * \addtogroup limdlLight Light
 * @{
 */

#include "model-light.h"
#include "model-node.h"

int
limdl_light_read (limdlNode* self,
                  liReader*  reader)
{
	uint32_t tmp;
	limdlLight* light = &self->light;

	if (!li_reader_get_uint32 (reader, &tmp) ||
	    !li_reader_get_float (reader, &light->projection.fov) ||
	    !li_reader_get_float (reader, &light->projection.near) ||
	    !li_reader_get_float (reader, &light->projection.far) ||
	    !li_reader_get_float (reader, light->color + 0) ||
	    !li_reader_get_float (reader, light->color + 1) ||
	    !li_reader_get_float (reader, light->color + 2) ||
	    !li_reader_get_float (reader, light->equation + 0) ||
	    !li_reader_get_float (reader, light->equation + 1) ||
	    !li_reader_get_float (reader, light->equation + 2) ||
	    !li_reader_get_float (reader, &light->spot.cutoff) ||
	    !li_reader_get_float (reader, &light->spot.exponent))
		return 0;
	self->light.flags = tmp;

	return 1;
}

int
limdl_light_write (const limdlNode* self,
                   liarcWriter*     writer)
{
	const limdlLight* light = &self->light;

	liarc_writer_append_uint32 (writer, light->flags);
	liarc_writer_append_float (writer, light->projection.fov);
	liarc_writer_append_float (writer, light->projection.near);
	liarc_writer_append_float (writer, light->projection.far);
	liarc_writer_append_float (writer, light->color[0]);
	liarc_writer_append_float (writer, light->color[1]);
	liarc_writer_append_float (writer, light->color[2]);
	liarc_writer_append_float (writer, light->equation[0]);
	liarc_writer_append_float (writer, light->equation[1]);
	liarc_writer_append_float (writer, light->equation[2]);
	liarc_writer_append_float (writer, light->spot.cutoff);
	liarc_writer_append_float (writer, light->spot.exponent);

	return !writer->error;
}

void
limdl_light_get_modelview (const limdlNode* self,
                           limatMatrix*     value)
{
	*value = limat_convert_transform_to_matrix (self->transform.pose);
}

void
limdl_light_get_projection (const limdlNode* self,
                            limatMatrix*     value)
{
	*value = limat_matrix_perspective (
		self->light.projection.fov, 1.0f,
		self->light.projection.near,
		self->light.projection.far);
}

/** @} */
/** @} */
