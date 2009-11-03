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
 * \addtogroup limdlBone Bone
 * @{
 */

#include <stdio.h>
#include "model-bone.h"
#include "model-node.h"

int
limdl_bone_read (limdlNode*   self,
                 liarcReader* reader)
{
	limdlBone* bone = &self->bone;

	if (!liarc_reader_get_float (reader, &bone->length.x) ||
		!liarc_reader_get_float (reader, &bone->length.y) ||
		!liarc_reader_get_float (reader, &bone->length.z))
		return 0;

	return 1;
}

int
limdl_bone_write (const limdlNode* self,
                  liarcWriter*     writer)
{
	const limdlBone* bone = &self->bone;

	liarc_writer_append_float (writer, bone->length.x);
	liarc_writer_append_float (writer, bone->length.y);
	liarc_writer_append_float (writer, bone->length.z);

	return !writer->error;
}


/**
 * \brief Gets the head of the bone in global coordinates.
 *
 * \param self Bone node.
 * \param head Return location for the head.
 */
void
limdl_bone_get_pose_head (const limdlNode* self,
                          limatVector*     head)
{
	*head = self->transform.global.position;
}

/**
 * \brief Gets the tail of the bone in global coordinates.
 *
 * \param self Bone node.
 * \param tail Return location for the tail.
 */
void
limdl_bone_get_pose_tail (const limdlNode* self,
                          limatVector*     tail)
{
	*tail = self->bone.tail;
}

/** @} */
/** @} */
