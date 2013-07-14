/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __EXT_TERRAIN_FACE_ITERATOR_H__
#define __EXT_TERRAIN_FACE_ITERATOR_H__

#include "lipsofsuna/model.h"
#include "terrain-stick.h"

typedef struct _LIExtTerrainFaceIterator LIExtTerrainFaceIterator;
struct _LIExtTerrainFaceIterator
{
	int vx0;
	int vz0;
	int vx1;
	int vz1;
	float stick_y;
	float stick_y0;
	float stick_y1;
	LIExtTerrainStick* stick;
};

void liext_terrain_face_iterator_init (
	LIExtTerrainFaceIterator* self,
	LIExtTerrainStick*        stick,
	int                       vx0,
	int                       vz0,
	int                       vx1,
	int                       vz1);

void liext_terrain_face_iterator_emit (
	LIExtTerrainFaceIterator* self,
	LIMdlBuilder*             builder,
	float                     u,
	float                     v,
	const LIMatVector*        normal,
	const LIMatVector*        bot0,
	const LIMatVector*        bot1,
	const LIMatVector*        top0,
	const LIMatVector*        top1);

#endif
