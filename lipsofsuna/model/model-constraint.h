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
 * \addtogroup limdlConstraint Constraint
 * @{
 */

#ifndef __MODEL_CONSTRAINT_H__
#define __MODEL_CONSTRAINT_H__

#include <archive/lips-archive.h>
#include "model-pose.h"
#include "model-types.h"

enum
{
	LIMDL_CONSTRAINT_COPY_ROTATION,
	LIMDL_CONSTRAINT_INVERSE_KINEMATICS,
	LIMDL_CONSTRAINT_LIMIT_ROTATION,
};

union _limdlConstraint
{
	int type;
	struct
	{
		int type;
		char* node_name;
		char* target_name;
	} copy_rotation;
	struct
	{
		int type;
		int chain_length;
		char* node_name;
		char* target_name;
	} inverse_kinematics;
	struct
	{
		int type;
		float min[3];
		float max[3];
		char* node_name;
	} limit_rotation;
};

#ifdef __cplusplus
extern "C" {
#endif

limdlConstraint*
limdl_constraint_new_from_stream (liarcReader* reader);

void
limdl_constraint_free (limdlConstraint* self);

void
limdl_constraint_solve (limdlConstraint* self,
                        limdlPose*       pose);

int
limdl_constraint_write (const limdlConstraint* self,
                        liarcWriter*           writer);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
