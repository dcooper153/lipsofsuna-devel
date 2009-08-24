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
 * \addtogroup lipar Particle
 * @{
 * \addtogroup liparManager Manager
 * @{
 */

#include <system/lips-system.h>
#include "particle-manager.h"

liparManager*
lipar_manager_new (int points,
                   int lines)
{
	int i;
	liparManager* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liparManager));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Allocate particles. */
	self->points.all = calloc (points, sizeof (liparPoint));
	if (self->points.all == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		lipar_manager_free (self);
		return NULL;
	}
	self->lines.all = calloc (lines, sizeof (liparLine));
	if (self->lines.all == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		lipar_manager_free (self);
		return NULL;
	}

	/* Add points to free list. */
	self->points.count_free = points;
	self->points.free = self->points.all;
	for (i = 0 ; i < points ; i++)
	{
		self->points.all[i].prev = self->points.all + i - 1;
		self->points.all[i].next = self->points.all + i + 1;
	}
	self->points.all[0].prev = NULL;
	self->points.all[points - 1].next = NULL;

	/* Add lines to free list. */
	self->lines.count_free = lines;
	self->lines.free = self->lines.all;
	for (i = 0 ; i < lines ; i++)
	{
		self->lines.all[i].prev = self->lines.all + i - 1;
		self->lines.all[i].next = self->lines.all + i + 1;
	}
	self->lines.all[0].prev = NULL;
	self->lines.all[lines - 1].next = NULL;

	return self;
}

void
lipar_manager_free (liparManager* self)
{
	free (self->lines.all);
	free (self->points.all);
	free (self);
}

liparLine*
lipar_manager_insert_line (liparManager*      self,
                           const limatVector* position0,
                           const limatVector* position1,
                           const limatVector* velocity0,
                           const limatVector* velocity1)
{
	liparLine* line;

	/* Get free line. */
	line = self->lines.free;
	if (line == NULL)
		return NULL;
	lipar_line_init (line, position0, position1, velocity0, velocity1);

	/* Remove from free list. */
	self->lines.free = line->next;
	if (line->next != NULL)
		line->next->prev = NULL;

	/* Add to used list. */
	if (self->lines.used != NULL)
		self->lines.used->prev = line;
	line->next = self->lines.used;
	line->prev = NULL;
	self->lines.used = line;

	return line;
}

liparPoint*
lipar_manager_insert_point (liparManager*      self,
                            const limatVector* position,
                            const limatVector* velocity)
{
	liparPoint* point;

	/* Get free point. */
	point = self->points.free;
	if (point == NULL)
		return NULL;
	lipar_point_init (point, position, velocity);

	/* Remove from free list. */
	self->points.free = point->next;
	if (point->next != NULL)
		point->next->prev = NULL;

	/* Add to used list. */
	if (self->points.used != NULL)
		self->points.used->prev = point;
	point->next = self->points.used;
	point->prev = NULL;
	self->points.used = point;

	return point;
}

void
lipar_manager_remove_line (liparManager* self,
                           liparLine*    line)
{
	/* Remove from used list. */
	if (line->prev != NULL)
		line->prev->next = line->next;
	else
		self->lines.used = line->next;
	if (line->next != NULL)
		line->next->prev = line->prev;

	/* Add to free list. */
	if (self->lines.free != NULL)
		self->lines.free->prev = line;
	line->next = self->lines.free;
	line->prev = NULL;
	self->lines.free = line;
}

void
lipar_manager_remove_point (liparManager* self,
                            liparPoint*   point)
{
	/* Remove from used list. */
	if (point->prev != NULL)
		point->prev->next = point->next;
	else
		self->points.used = point->next;
	if (point->next != NULL)
		point->next->prev = point->prev;

	/* Add to free list. */
	if (self->points.free != NULL)
		self->points.free->prev = point;
	point->next = self->points.free;
	point->prev = NULL;
	self->points.free = point;
}

void
lipar_manager_update (liparManager* self,
                      float         secs)
{
	int i;
	liparLine* line;
	liparLine* line_next;
	liparPoint* point;
	liparPoint* point_next;

	/* Update points. */
	for (point = self->points.used ; point != NULL ; point = point_next)
	{
		/* FIXME: Inaccurate. */
		point->velocity = limat_vector_add (point->velocity,
			limat_vector_multiply (point->acceleration, secs * secs));
		point->position = limat_vector_add (point->position,
			limat_vector_multiply (point->velocity, secs));
		point->time += secs;
		point_next = point->next;
		if (point->time > point->time_life)
			lipar_manager_remove_point (self, point);
	}

	/* Update lines. */
	for (line = self->lines.used ; line != NULL ; line = line_next)
	{
		/* FIXME: Inaccurate. */
		for (i = 0 ; i < 2 ; i++)
		{
			line->velocity[i] = limat_vector_add (line->velocity[i],
				limat_vector_multiply (line->acceleration[i], secs * secs));
			line->position[i] = limat_vector_add (line->position[i],
				limat_vector_multiply (line->velocity[i], secs));
			line->time[i] += secs;
		}
		line_next = line->next;
		if (line->time[0] > line->time_life[0] &&
		    line->time[1] > line->time_life[1])
			lipar_manager_remove_line (self, line);
	}
}

/** @} */
/** @} */
