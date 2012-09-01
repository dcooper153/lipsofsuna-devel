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

static void private_free_sticks (
	LIExtTerrainStick* sticks);

static void private_insert_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  insert);

static void private_remove_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  remove);

/*****************************************************************************/

/**
 * \brief Draws a stick at the given Y offset.
 * \param self Terrain column.
 * \param world_y Y offset of the stick in world units.
 * \param world_h Y height of the stick in world units.
 * \param material Terrain material ID.
 * \return Nonzero on success, zero if allocating memory failed.
 */
int liext_terrain_column_add_stick (
	LIExtTerrainColumn* self,
	float               world_y,
	float               world_h,
	int                 material)
{
	float length;
	float padding;
	float isect_start_y;
	float isect_end_y;
	LIExtTerrainStick* stick1;
	LIExtTerrainStick* stick;
	LIExtTerrainStick* stick_next;
	LIExtTerrainStick* stick_prev;
	LIExtTerrainStick* isect_first;
	LIExtTerrainStick* isect_last;

	/*************************************************************************/
	/* Intersection range calculation. */

	/* Find the first intersecting stick. */
	isect_start_y = 0.0f;
	stick_prev = NULL;
	for (isect_first = self->sticks ; isect_first != NULL ; isect_first = isect_first->next)
	{
		if (isect_start_y + isect_first->height > world_y)
			break;
		isect_start_y += isect_first->height;
		stick_prev = isect_first;
	}

	/* Find the last intersecting stick. */
	/* This stick is the one that extends past the end point of the added
	   stick. If the column ends before that, it is the last stick of the
	   column. */
	if (isect_first != NULL)
	{
		isect_end_y = isect_start_y;
		for (isect_last = isect_first ; 1 ; isect_last = isect_last->next)
		{
			isect_end_y += isect_last->height;
			if (isect_last->next == NULL)
				break;
			if (isect_end_y >= world_y + world_h)
				break;
		}
	}

	/*************************************************************************/
	/* Special cases for appending. */

	/* Do not add empty sticks to the end of the list. */
	/* The section past the end of the list is automatically considered
	   empty so creating an empty stick at the end would be redundant.
	   Some of the algorithms also expect there to be no empty sticks at
	   the end so we are obliged to avoid that. */
	if (isect_first == NULL && material == 0)
		return 1;

	/* Create a new stick if adding to the end of the list. */
	/* When the stick is appended to the end of the list, we can take a
	   shortcut since no intersections can occur. However, we might have
	   to add an extra padding stick before it if there's a gap between
	   it and the previous stick. */
	if (isect_first == NULL)
	{
		/* Fill the potential gap with an empty stick. */
		padding = world_y - isect_start_y;
		if (padding > LIEXT_STICK_EPSILON)
		{
			stick = liext_terrain_stick_new (0, padding);
			if (stick == NULL)
				return 0;
			if (stick_prev != NULL)
			{
				lisys_assert (stick_prev->material != 0);
				stick_prev->next = stick;
			}
			else
				self->sticks = stick;
			stick_prev = stick;
		}

		/* Create the new stick. */
		stick = liext_terrain_stick_new (material, world_h);
		if (stick == NULL)
			return 0;
		private_insert_stick (self, stick_prev, stick);

		/* Mark the column as changed. */
		self->stamp++;

		return 1;
	}

	/*************************************************************************/
	/* Intersection range snapping. */

	/* Snap the start and end points to the end point of a stick if very close. */
	/* This will reduce the complexity of the column since pointlessly
	   short sticks will become subject to removal in the later steps.
	   The actual length of the added stick may change slightly to offset
	   for the snapping, but the difference is visually insignificant. */
	padding = world_y - isect_start_y;
	if (padding < LIEXT_STICK_EPSILON)
	{
		/*
		 *   0011111111 ---\  0011111111
		 *      2222222 ---/    22222222
		 */
		world_y = isect_start_y;
		world_h += padding;
	}
	padding = isect_end_y - world_y - world_h;
	if (padding > 0.0f && padding < LIEXT_STICK_EPSILON)
	{
		/*
		 *   1111111100 ---\  1111111100
		 *   2222222    ---/  22222222
		 */
		world_h += padding;
	}

	/*************************************************************************/
	/* Special insertion cases. */

	/* Handle a single stick being overwritten with the same material.
	 *
	 * This is an easy no operation case so skipping it here is easy
	 * and simplifies the rest of the algorithm.
	 *
	 *     1111     1111     1111     1111
	 *   + 1111   + 11     +  11    +   11
	 *   = 1111   = 1111   = 1111   = 1111
	 */
	if (isect_first == isect_last && material == isect_first->material)
		return 1;

	/* Handle the intersection being inside a single stick.
	 *
	 * This necessarily requires special handling since it is the only
	 * case in which an existing stick needs to be split in two.
	 *
	 *     1111
	 *   +  22
	 *   = 1221
	 */
	if (isect_first == isect_last && world_y > isect_start_y && world_y + world_h < isect_end_y)
	{
		stick = liext_terrain_stick_new (material, world_h);
		if (stick == NULL)
			return 0;
		stick1 = liext_terrain_stick_new (isect_first->material, isect_end_y - world_y - world_h);
		if (stick1 == NULL)
		{
			liext_terrain_stick_free (stick);
			return 0;
		}
		stick1->next = isect_first->next;
		stick->next = stick1;
		isect_first->next = stick;
		isect_first->height = world_y - isect_start_y;

		/* Mark the column as changed. */
		self->stamp++;

		return 1;
	}

	/* Handle a single stick exactly matching the full intersection.
	 *
	 * This case follows the same logic as the regular insertion case at
	 * the bottom of this function. Handling this separately eliminates
	 * memory allocations since it only requires replacements and removals.
	 * 
	 * Note that the next stick may not exist when overwriting the last stick.
	 * If that case, the imaginary next stick consists of the empty material.
	 * This leads to the special case of inserting the empty material leading
	 * to the removal of the last stick.
	 */
	if (isect_first == isect_last && world_y == isect_start_y && world_y + world_h == isect_end_y)
	{
		stick_next = isect_first->next;
		if (stick_next == NULL || stick_next->material != material)
		{
			if (stick_next == NULL && material == 0)
			{
				/*
				 *     33111100
				 *   +   0000
				 *   = 33000000
				 */
				private_remove_stick (self, stick_prev, isect_first);
			}
			else if (stick_prev == NULL || stick_prev->material != material)
			{
				/*
				 *     331111XX
				 *   +   2222
				 *   = 332222XX
				 */
				isect_first->material = material;
			}
			else
			{
				/*
				 *     221111XX
				 *   +   2222
				 *   = 222222XX
				 */
				stick_prev->height += isect_first->height;
				private_remove_stick (self, stick_prev, isect_first);
			}
		}
		else
		{
			if (stick_prev == NULL || stick_prev->material != material)
			{
				/*
				 *     33111122
				 *   +   2222
				 *   = 33222222
				 */
				stick_next->height += isect_first->height;
				private_remove_stick (self, stick_prev, isect_first);
			}
			else
			{
				/*
				 *     22333322
				 *   +   2222
				 *   = 22222222
				 */
				stick_prev->height += isect_first->height + stick_next->height;
				private_remove_stick (self, stick_prev, isect_first);
				private_remove_stick (self, stick_prev, stick_next);
			}
		}

		/* Mark the column as changed. */
		self->stamp++;

		return 1;
	}

	/*************************************************************************/
	/* Removing intersecting sticks. */

	/* Clip the first intersecting stick. */
	/* The first intersecting stick is often only partially clipped, but
	   it can be fully removed if it intersects for its entire length. */
	padding = world_y - isect_start_y;
	if (padding == 0 && world_h >= isect_first->height)
	{
		/*
		 * 111111
		 * XXXXXX
		 */
		private_remove_stick (self, stick_prev, isect_first);
	}
	else if (padding == 0)
	{
		/*
		 * 111111
		 * XXXX
		 */
		isect_first->height -= world_h;
		lisys_assert (isect_first == isect_last);
		lisys_assert (isect_first->height >= LIEXT_STICK_EPSILON);
	}
	else
	{
		/*
		 * 111111
		 *   XXXX
		 */
		stick_prev = isect_first;
		isect_first->height -= isect_start_y + isect_first->height - world_y;
		lisys_assert (isect_first->height >= LIEXT_STICK_EPSILON);
	}

	/* Remove the intersecting sticks between the first and the last. */
	/* These sticks are always removed since they always intersect fully.
	   However, if only one stick intersected, the removal is already done,
	   and the contents of the stick pointers involved may be invalid. */
	if (isect_first != isect_last)
	{
		if (stick_prev != NULL)
			stick = stick_prev->next;
		else
			stick = self->sticks;
		for ( ; stick != isect_last ; stick = stick_next)
		{
			/*
			 * 111111
			 * XXXXXX
			 */
			stick_next = stick->next;
			private_remove_stick (self, stick_prev, stick);
		}
	}

	/* Clip the last intersecting stick. */
	/* The last intersecting stick may be either fully or partially clipped.
	   However, if only one stick intersected, the clipping is already done,
	   and the contents of the stick pointers involved may be invalid. */
	if (isect_first != isect_last)
	{
		length = isect_end_y - world_y - world_h;
		if (length > 0)
		{
			/*
			 * 111111
			 * XXXX
			 */
			lisys_assert (length >= LIEXT_STICK_EPSILON);
			isect_last->height = length;
		}
		else
		{
			/*
			 * 111111
			 * XXXXXX
			 */
			private_remove_stick (self, stick_prev, isect_last);
		}
	}

	/*************************************************************************/
	/* Inserting the stick into the created gap. */

	/* Create the new stick.
	 *
	 * Due to the special cases being handled above, the following cases
	 * remain to be handled here. These cases require insertion, deleteion
	 * and replacement operations.
	 */
	if (stick_prev != NULL)
		stick_next = stick_prev->next;
	else
		stick_next = self->sticks;
	if (stick_next == NULL || stick_next->material != material)
	{
		if (stick_next == NULL && material == 0)
		{
			/*
			 *     33....00
			 *   +   0000
			 *   = 33000000
			 */
		}
		else if (stick_prev == NULL || stick_prev->material != material)
		{
			/*
			 *     33....XX
			 *   +   2222
			 *   = 332222XX
			 */
			stick = liext_terrain_stick_new (material, world_h);
			if (stick == NULL)
				return 0;
			private_insert_stick (self, stick_prev, stick);
		}
		else
		{
			/*
			 *     22....XX
			 *   +   2222
			 *   = 222222XX
			 */
			stick_prev->height += world_h;
		}
	}
	else
	{
		if (stick_prev == NULL || stick_prev->material != material)
		{
			/*
			 *     33....22
			 *   +   2222
			 *   = 33222222
			 */
			stick_next->height += world_h;
		}
		else
		{
			/*
			 *     22....22
			 *   +   2222
			 *   = 22222222
			 */
			stick_prev->height += world_h + stick_next->height;
			private_remove_stick (self, stick_prev, stick_next);
		}
	}

	return 1;
}

/**
 * \brief Frees all the sticks of the column.
 * \param self Terrain column.
 */
void liext_terrain_column_clear (
	LIExtTerrainColumn* self)
{
	private_free_sticks (self->sticks);
	self->sticks = NULL;
}

/**
 * \brief Gets the serialized contents of the column.
 * \param self Terrain column.
 * \param writer Archive write.
 * \return Nonzero on success.
 */
int liext_terrain_column_get_data (
	LIExtTerrainColumn* self,
	LIArcWriter*        writer)
{
	LIExtTerrainStick* stick;

	for (stick = self->sticks ; stick != NULL ; stick = stick->next)
	{
		if (!liarc_writer_append_uint8 (writer, 1))
			return 0;
		if (!liext_terrain_stick_get_data (stick, writer))
			return 0;
	}
	if (!liarc_writer_append_uint8 (writer, 0))
		return 0;

	return 1;
}

/**
 * \brief Sets the contents of the column from serialized data.
 * \param self Terrain column.
 * \param reader Archive reader.
 * \return Nonzero on success.
 */
int liext_terrain_column_set_data (
	LIExtTerrainColumn* self,
	LIArcReader*        reader)
{
	uint8_t tmp;
	LIExtTerrainStick* stick;
	LIExtTerrainStick* sticks_first;
	LIExtTerrainStick* sticks_last;

	/* Read the sticks. */
	sticks_first = NULL;
	sticks_last = NULL;
	while (1)
	{
		/* Check for more data. */
		if (!liarc_reader_get_uint8 (reader, &tmp))
		{
			private_free_sticks (sticks_first);
			return 0;
		}
		if (!tmp)
			break;

		/* Allocate a new stick. */
		stick = liext_terrain_stick_new (0, 1.0f);
		if (stick == NULL)
		{
			private_free_sticks (sticks_first);
			return 0;
		}

		/* Read the contents of the stick. */
		if (!liext_terrain_stick_set_data (stick, reader))
		{
			private_free_sticks (sticks_first);
			return 0;
		}

		/* Append the stick to the linked list. */
		if (sticks_last != NULL)
		{
			sticks_first = stick;
			sticks_last = stick;
		}
		else
		{
			sticks_last->next = stick;
			sticks_last = stick;
		}
	}

	/* Use the new sticks. */
	liext_terrain_column_clear (self);
	self->sticks = sticks_first;
	self->stamp++;

	return 1;
}

/*****************************************************************************/

static void private_free_sticks (
	LIExtTerrainStick* sticks)
{
	LIExtTerrainStick* stick;
	LIExtTerrainStick* stick_next;

	for (stick = sticks ; stick != NULL ; stick = stick_next)
	{
		stick_next = stick->next;
		liext_terrain_stick_free (stick);
	}
}

static void private_insert_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  insert)
{
	if (prev == NULL)
	{
		insert->next = self->sticks;
		self->sticks = insert;
	}
	else
	{
		insert->next = prev->next;
		prev->next = insert;
	}
}

static void private_remove_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  remove)
{
	if (prev == NULL)
	{
		lisys_assert (self->sticks == remove);
		self->sticks = remove->next;
	}
	else
	{
		lisys_assert (prev->next == remove);
		prev->next = remove->next;
	}
	liext_terrain_stick_free (remove);
}

/** @} */
/** @} */
