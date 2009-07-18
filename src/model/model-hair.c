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
 * \addtogroup limdlHair Hair
 * @{
 */

#include <stdlib.h>
#include "model-hair.h"

int
limdl_hair_read (limdlHair* self,
                 liReader*  reader)
{
	uint32_t i;
	uint32_t count;
	limdlHairNode* node;

	/* Read node count. */
	if (!li_reader_get_uint32 (reader, &count))
		return 0;

	/* Allocate nodes. */
	self->nodes.array = calloc (count, sizeof (limdlHairNode));
	if (self->nodes.array == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	self->nodes.count = count;

	/* Read nodes. */
	for (i = 0 ; i < count ; i++)
	{
		node = self->nodes.array + i;
		if (!li_reader_get_float (reader, &node->position.x) ||
		    !li_reader_get_float (reader, &node->position.y) ||
		    !li_reader_get_float (reader, &node->position.z) ||
		    !li_reader_get_float (reader, &node->size))
			return 0;
	}

	return 1;
}

int
limdl_hair_write (limdlHair*   self,
                  liarcWriter* writer)
{
	int i;
	limdlHairNode* node;

	/* Write node count. */
	if (!liarc_writer_append_uint32 (writer, self->nodes.count))
		return 0;

	/* Write nodes. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array + i;
		if (!liarc_writer_append_float (writer, node->position.x) ||
		    !liarc_writer_append_float (writer, node->position.y) ||
		    !liarc_writer_append_float (writer, node->position.z) ||
		    !liarc_writer_append_float (writer, node->size))
			return 0;
	}

	return 1;
}

/** @} */
/** @} */
