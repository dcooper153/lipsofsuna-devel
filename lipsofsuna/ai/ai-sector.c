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
 * \addtogroup liai Ai
 * @{
 * \addtogroup liaiSector Sector
 * @{
 */

#include "ai-sector.h"

/**
 * \brief Creates a new AI data sector.
 *
 * \return New sector or NULL.
 */
liaiSector*
liai_sector_new ()
{
	liaiSector* self;

	self = calloc (1, sizeof (liaiSector));
	if (self == NULL)
		return NULL;

	return self;
}

/**
 * \brief Loads an AI data sector from data.
 *
 * \param reader Reader.
 * \return New sector or NULL.
 */
liaiSector*
liai_sector_new_from_data (liReader* reader)
{
	uint32_t i;
	uint32_t j;
	uint32_t count;
	uint32_t link;
	uint16_t links;
	limatVector position;
	liaiSector* self;
	liaiWaypoint** points;

	/* Read header. */
	if (!li_reader_check_text (reader, "lips/pth", ""))
	{
		lisys_error_set (EINVAL, "incorrect file type");
		return NULL;
	}
	if (!li_reader_get_uint32 (reader, &count))
		return NULL;

	/* Allocate points. */
	points = calloc (count, sizeof (liaiWaypoint*));
	if (points == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	position = limat_vector_init (0.0f, 0.0f, 0.0f);
	for (i = 0 ; i < count ; i++)
	{
		points[i] = liai_waypoint_new (&position);
		if (points[i] == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
	}

	/* Read points. */
	for (i = 0 ; i < count ; i++)
	{
		if (!li_reader_get_float (reader, &(points[i]->position.x)) ||
		    !li_reader_get_float (reader, &(points[i]->position.y)) ||
		    !li_reader_get_float (reader, &(points[i]->position.z)) ||
		    !li_reader_get_uint16 (reader, &links))
			goto error;
		for (j = 0 ; j < links ; j++)
		{
			if (!li_reader_get_uint32 (reader, &link))
				goto error;
			if (link >= count)
			{
				lisys_error_set (EINVAL, "link index out of bounds");
				goto error;
			}
			if (!liai_waypoint_insert_link (points[i], points[link]))
			{
				lisys_error_set (ENOMEM, NULL);
				goto error;
			}
		}
	}

	/* Create sector. */
	self = liai_sector_new ();
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}
	for (i = 0 ; i < count ; i++)
	{
		if (!liai_sector_insert_waypoint (self, points[i]))
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		points[i] = NULL;
	}
	free (points);

	return self;

error:
	for (i = 0 ; i < count ; i++)
	{
		if (points[i] != NULL)
			liai_waypoint_free (points[i]);
	}
	free (points);
	return NULL;
}

/**
 * \brief Loads an AI data sector from a file.
 *
 * \param path Path to the file to load.
 * \return New sector or NULL.
 */
liaiSector*
liai_sector_new_from_file (const char* path)
{
	liReader* reader;
	liaiSector* self;

	reader = li_reader_new_from_file (path);
	if (reader == NULL)
		return NULL;
	self = liai_sector_new_from_data (reader);
	li_reader_free (reader);

	return self;
}

/**
 * \brief Frees the sector.
 *
 * \param self Sector.
 */
void
liai_sector_free (liaiSector* self)
{
	lialgList* ptr;

	for (ptr = self->waypoints ; ptr != NULL ; ptr = ptr->next)
		liai_waypoint_free (ptr->data);
	lialg_list_free (self->waypoints);
	free (self);
}

/**
 * \brief Finds a waypoint by position.
 *
 * \param self Sector.
 * \param point Position vector.
 * \return Waypoint or NULL.
 */
liaiWaypoint*
liai_sector_find_waypoint (liaiSector*     self,
                           const limatVector* point)
{
	float d;
	float dist;
	lialgList* ptr;
	liaiWaypoint* best;
	liaiWaypoint* tmp;

	dist = LI_MATH_INFINITE;
	best = NULL;

	/* FIXME: Slow! */
	for (ptr = self->waypoints ; ptr != NULL ; ptr = ptr->next)
	{
		tmp = ptr->data;
		d = limat_vector_get_length (limat_vector_subtract (*point, tmp->position));
		if (d < dist)
		{
			best = tmp;
			dist = d;
		}
	}

	return best;
}

/**
 * \brief Inserts a waypoint to the sector.
 *
 * The ownership of the waypoint is given to the sector.
 *
 * \param self Sector.
 * \param waypoint Waypoint to insert.
 */
int
liai_sector_insert_waypoint (liaiSector*   self,
                             liaiWaypoint* waypoint)
{
	return lialg_list_prepend (&self->waypoints, waypoint);
}

/**
 * \brief Removes the waypoint from the sector.
 *
 * The ownership of the waypoint is given to the user.
 *
 * \param self Sector.
 * \param waypoint Waypoint to remove.
 */
void
liai_sector_remove_waypoint (liaiSector*   self,
                             liaiWaypoint* waypoint)
{
	lialgList* ptr;

	for (ptr = self->waypoints ; ptr != NULL ; ptr = ptr->next)
	{
		if (ptr->data == waypoint)
			lialg_list_remove (&self->waypoints, ptr);
	}
}

/**
 * \brief Writes the sector to a file.
 *
 * \param self Sector.
 * \param path Path to the file.
 * \return Nonzero on success.
 */
int
liai_sector_save (const liaiSector* self,
                  const char*       path)
{
	int count;
	void* tmp;
	liaiWaypoint* point;
	lialgList* ptr;
	lialgPtrdic* lookup;
	liarcWriter* writer;

	/* Create link lookup. */
	count = 0;
	lookup = lialg_ptrdic_new ();
	if (lookup == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	for (ptr = self->waypoints ; ptr != NULL ; ptr = ptr->next)
	{
		point = ptr->data;
		if (!lialg_ptrdic_insert (lookup, point, (void*)(intptr_t)(++count)))
		{
			lisys_error_set (ENOMEM, NULL);
			lialg_ptrdic_free (lookup);
			return 0;
		}
	}

	/* Create writer. */
	writer = liarc_writer_new_file (path);
	if (writer == NULL)
	{
		lialg_ptrdic_free (lookup);
		return 0;
	}

	/* Write header. */
	liarc_writer_append_string (writer, "lips/pth");
	liarc_writer_append_nul (writer);
	liarc_writer_append_uint32 (writer, count);

	/* Write waypoints. */
	for (ptr = self->waypoints ; ptr != NULL ; ptr = ptr->next)
	{
		point = ptr->data;
		liarc_writer_append_float (writer, point->position.x);
		liarc_writer_append_float (writer, point->position.y);
		liarc_writer_append_float (writer, point->position.z);
		liarc_writer_append_uint16 (writer, point->links.count);
		for (count = 0 ; count < point->links.count ; count++)
		{
			tmp = lialg_ptrdic_find (lookup, point->links.links[count].target);
			liarc_writer_append_uint32 (writer, ((int)(intptr_t) tmp) - 1);
		}
	}

	lialg_ptrdic_free (lookup);
	liarc_writer_free (writer);

	return 1;
}

/** @} */
/** @} */
