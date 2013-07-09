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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtTerrain Terrain
 * @{
 */

#include "terrain-face-iterator.h"

/**
 * \brief Initializes the face iterator.
 * \param stick Stick containing the face.
 * \param vx0 X index of the first vertex.
 * \param vz0 Z indes of the first vertex.
 * \param vx1 X index of the second vertex.
 * \param vz1 Z indes of the second vertex.
 */
void liext_terrain_face_iterator_init (
	LIExtTerrainFaceIterator* self,
	LIExtTerrainStick*        stick,
	int                       vx0,
	int                       vz0,
	int                       vx1,
	int                       vz1)
{
	self->vx0 = vx0;
	self->vz0 = vz1;
	self->vx1 = vx1;
	self->vz1 = vz1;
	self->stick_y = 0.0f;
	self->stick_y0 = 0.0f;
	self->stick_y1 = 0.0f;
	self->stick = stick;
}

/**
 * \brief Iterates while culling a neighbor stick.
 * \param bot0 Coordinates of the first bottom vertex of the neighbor face.
 * \param bot1 Coordinates of the second bottom vertex of the neighbor face.
 * \param top0 Coordinates of the first top vertex of the neighbor face.
 * \param top1 Coordinates of the second top vertex of the neighbor face.
 * \return One if culled. Zero otherwise.
 */
int liext_terrain_face_iterator_cull (
	LIExtTerrainFaceIterator* self,
	const LIMatVector*        bot0,
	const LIMatVector*        bot1,
	const LIMatVector*        top0,
	const LIMatVector*        top1)
{
	float stick_y_top;
	float stick_y0_top;
	float stick_y1_top;
	LIExtTerrainStick* s;

	/* Check that there are neighbor sticks left. */
	/* If the wall starts above all the neighbor sticks or there are no
	   neighbors at all, then no culling can be done. */
	if (self->stick == NULL)
		return 0;

	/* Check for the bottom culling offset. */
	/* If an empty stick caused the stick pointer to start above the bottom
	   of the culled wall, then no culling can be done. */
	if (self->stick_y0 > bot0->y || self->stick_y1 > bot1->y)
		return 0;

	/* Find the bottom of the neighbor wall. */
	/* The culled wall may start well above the current stick pointer.
	   To simplify things and reduce future iteration, the stick pointer
	   and the Y offsets are rewound to the first neighbor stick that
	   starts below the wall. */
	stick_y_top = self->stick_y;
	stick_y0_top = self->stick_y0;
	stick_y1_top = self->stick_y1;
	for ( ; self->stick != NULL ; self->stick = self->stick->next)
	{
		stick_y_top += self->stick->height;
		stick_y0_top = stick_y_top + self->stick->vertices[self->vx0][self->vz0].offset;
		stick_y1_top = stick_y_top + self->stick->vertices[self->vx1][self->vz1].offset;
		if (stick_y0_top > bot0->y || stick_y1_top > bot1->y)
			break;
		self->stick_y = stick_y_top;
		self->stick_y0 = stick_y0_top;
		self->stick_y1 = stick_y1_top;
	}
	if (self->stick == NULL)
		return 0;
	lisys_assert(self->stick_y0 <= bot0->y);
	lisys_assert(self->stick_y1 <= bot1->y);

	/* Skip empty sticks. */
	/* The stick pointer is currently at the bottommost stick that is
	   still below the bottom edge of the wall. If it is an empty stick,
	   the culling fails as the bottom of the wall is not occluded. */
	if (self->stick->material == 0)
	{
		self->stick_y += self->stick->height;
		self->stick_y0 = self->stick_y + self->stick->vertices[self->vx0][self->vz0].offset;
		self->stick_y1 = self->stick_y + self->stick->vertices[self->vx1][self->vz1].offset;
		self->stick = self->stick->next;
		return 0;
	}

	/* Find the top of the neighbor wall. */
	/* If the neighbor wall extends past the culled wall, then culling
	   should be done. If an empty stick or the end of the column occur
	   before that, no culling can be done. */
	stick_y_top = self->stick_y;
	stick_y0_top = self->stick_y0;
	stick_y1_top = self->stick_y1;
	for (s = self->stick ; s != NULL && s->material != 0 ; s = s->next)
	{
		stick_y_top += s->height;
		stick_y0_top = stick_y_top + s->vertices[self->vx0][self->vz0].offset;
		stick_y1_top = stick_y_top + s->vertices[self->vx1][self->vz1].offset;
		if (stick_y0_top >= top0->y && stick_y1_top >= top1->y)
			return 1;
	}

	return 0;
}

/** @} */
/** @} */
