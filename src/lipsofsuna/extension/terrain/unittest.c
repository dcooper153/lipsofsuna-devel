/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#include "module.h"

/**
 * \brief Performs unittests for the internal algorithms.
 */
void liext_terrain_unittest ()
{
	LIExtTerrainStick* stick;
	LIExtTerrainStick tmp;

	stick = liext_terrain_stick_new (1, 10.0f);
	stick->vertices[0][0].offset = -10.0f;
	stick->vertices[1][0].offset = -5.0f;
	stick->vertices[0][1].offset = 10.0f;
	stick->vertices[1][1].offset = 5.0f;

	/*************************************************************************/
	/* Subtraction. */

	/* A) Is the stick completely below us?
	 * 
	 * .....SSSSSSSSSS.....
	 * XXX??...............
	 */

	tmp = *stick;
	lisys_assert (liext_terrain_stick_subtract (&tmp, -5.0f, 5.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) == 1);
	lisys_assert (tmp.height == 10.0f);
	lisys_assert (tmp.vertices[0][0].offset == -10.0f);
	lisys_assert (tmp.vertices[1][0].offset == -5.0f);
	lisys_assert (tmp.vertices[0][1].offset == 10.0f);
	lisys_assert (tmp.vertices[1][1].offset == 5.0f);

	tmp = *stick;
	lisys_assert (liext_terrain_stick_subtract (&tmp, -5.0f, 4.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 5.0f, 10.0f, -5.0f, 0.0f) == 1);
	lisys_assert (tmp.height == 10.0f);
	lisys_assert (tmp.vertices[0][0].offset == -10.0f);
	lisys_assert (tmp.vertices[1][0].offset == -5.0f);
	lisys_assert (tmp.vertices[0][1].offset == 10.0f);
	lisys_assert (tmp.vertices[1][1].offset == 5.0f);

	/* B) Is the stick completely above us?
	 * 
	 * .....SSSSSSSSSS.....
	 * ...............??XXX
	 */

	tmp = *stick;
	lisys_assert (liext_terrain_stick_subtract (&tmp, 10.0f, 5.0f,
		0.0f, -10.0f, 5.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f) == 2);
	lisys_assert (tmp.height == 10.0f);
	lisys_assert (tmp.vertices[0][0].offset == -10.0f);
	lisys_assert (tmp.vertices[1][0].offset == -10.0f);
	lisys_assert (tmp.vertices[0][1].offset == 5.0f);
	lisys_assert (tmp.vertices[1][1].offset == 5.0f);

	tmp = *stick;
	lisys_assert (liext_terrain_stick_subtract (&tmp, 11.0f, 5.0f,
		0.0f, -10.0f, 5.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f) == 2);
	lisys_assert (tmp.height == 10.0f);
	lisys_assert (tmp.vertices[0][0].offset == -10.0f);
	lisys_assert (tmp.vertices[1][0].offset == -9.0f);
	lisys_assert (tmp.vertices[0][1].offset == 6.0f);
	lisys_assert (tmp.vertices[1][1].offset == 5.0f);

	/* C) Does the stick replace us completely?
	 * 
	 * .....SSSSSSSSSS.....
	 * ..???XXXXXXXXXX???..
	 */

	lisys_assert (liext_terrain_stick_subtract (&tmp, 0.0f, 10.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) == 3);
	lisys_assert (liext_terrain_stick_subtract (&tmp, -1.0f, 11.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) == 3);
	lisys_assert (liext_terrain_stick_subtract (&tmp, -1.0f, 12.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) == 3);

	/* D) Does the stick replace part of the bottom?
	 * 
	 * .....SSSSSSSSSS.....
	 * ..???XXXXX..........
	 */

	tmp = *stick;
	lisys_assert (liext_terrain_stick_subtract (&tmp, 0.0f, 5.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) == 4);
	lisys_assert (tmp.height == 5.0f);
	lisys_assert (tmp.vertices[0][0].offset == -5.0f);
	lisys_assert (tmp.vertices[1][0].offset == -5.0f);
	lisys_assert (tmp.vertices[0][1].offset == 10.0f);
	lisys_assert (tmp.vertices[1][1].offset == 5.0f);

	tmp = *stick;
	lisys_assert (liext_terrain_stick_subtract (&tmp, -2.0f, 5.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) == 4);
	lisys_assert (tmp.height == 7.0f);
	lisys_assert (tmp.vertices[0][0].offset == -7.0f);
	lisys_assert (tmp.vertices[1][0].offset == -5.0f);
	lisys_assert (tmp.vertices[0][1].offset == 10.0f);
	lisys_assert (tmp.vertices[1][1].offset == 5.0f);

	/* E) Does the stick replace part of the top?
	 * 
	 * .....SSSSSSSSSS.....
	 * ..........XXXXX???..
	 */

	tmp = *stick;
	lisys_assert (liext_terrain_stick_subtract (&tmp, 5.0f, 5.0f,
		0.0f, 10.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f) == 5);
	lisys_assert (tmp.height == 5.0f);
	lisys_assert (tmp.vertices[0][0].offset == -5.0f);
	lisys_assert (tmp.vertices[1][0].offset == 0.0f);
	lisys_assert (tmp.vertices[0][1].offset == 0.0f);
	lisys_assert (tmp.vertices[1][1].offset == 10.0f);

	tmp = *stick;
	lisys_assert (liext_terrain_stick_subtract (&tmp, 7.0f, 5.0f,
		0.0f, -1.0f, 0.0f, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f) == 5);
	lisys_assert (tmp.height == 7.0f);
	lisys_assert (tmp.vertices[0][0].offset == -7.0f);
	lisys_assert (tmp.vertices[1][0].offset == -2.0f);
	lisys_assert (tmp.vertices[0][1].offset == 0.0f);
	lisys_assert (tmp.vertices[1][1].offset == 8.0f);

	/* F) Does the stick replace part of the middle?
	 * 
	 * .....SSSSSSSSSS.....
	 * ........XXXX........
	 */
	tmp = *stick;
	lisys_assert (liext_terrain_stick_subtract (&tmp, 2.0f, 5.0f,
		-2.0f, 5.0f, 0.0f, 5.0f, 10.0f, 20.0f, 0.0f, -50.0f) == 6);
	lisys_assert (tmp.height == 2.0f);
	lisys_assert (tmp.vertices[0][0].offset == -2.0f);
	lisys_assert (tmp.vertices[1][0].offset == 3.0f);
	lisys_assert (tmp.vertices[0][1].offset == 0.0f);
	lisys_assert (tmp.vertices[1][1].offset == 5.0f);
	lisys_assert (tmp.next != NULL);
	lisys_assert (tmp.next->height == 3.0f);
	lisys_assert (tmp.next->vertices[0][0].offset == 7.0f);
	lisys_assert (tmp.next->vertices[1][0].offset == 17.0f);
	lisys_assert (tmp.next->vertices[0][1].offset == 10.0f);
	lisys_assert (tmp.next->vertices[1][1].offset == 5.0f);
	liext_terrain_stick_free (tmp.next);

	liext_terrain_stick_free (stick);
}

/** @} */
/** @} */
