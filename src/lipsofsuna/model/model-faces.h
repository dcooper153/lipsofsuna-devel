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

#ifndef __MODEL_FACES_H__
#define __MODEL_FACES_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "model-types.h"

typedef struct _LIMdlFaces LIMdlFaces;
struct _LIMdlFaces
{
	int material;
	struct
	{
		int count;
		uint32_t* array;
		int capacity;
	} indices;
};

LIAPICALL (void, limdl_faces_free, (
	LIMdlFaces* self));

LIAPICALL (int, limdl_faces_read, (
	LIMdlFaces*  self,
	LIArcReader* reader));

LIAPICALL (int, limdl_faces_write, (
	LIMdlFaces*  self,
	LIArcWriter* writer));

#endif
