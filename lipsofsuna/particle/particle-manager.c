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

/**
 * \addtogroup lipar Particle
 * @{
 * \addtogroup LIParManager Manager
 * @{
 */

#include <lipsofsuna/system.h>
#include "particle-manager.h"

LIParManager*
lipar_manager_new (int points,
                   int lines)
{
	int i;
	LIParManager* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIParManager));
	if (self == NULL)
		return NULL;

	/* Allocate particles. */
	self->points.all = lisys_calloc (points, sizeof (LIParPoint));
	if (self->points.all == NULL)
	{
		lipar_manager_free (self);
		return NULL;
	}
	self->lines.all = lisys_calloc (lines, sizeof (LIParLine));
	if (self->lines.all == NULL)
	{
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
lipar_manager_free (LIParManager* self)
{
	lisys_free (self->lines.all);
	lisys_free (self->points.all);
	lisys_free (self);
}

LIParLine*
lipar_manager_insert_line (LIParManager*      self,
                           const LIMatVector* position0,
                           const LIMatVector* position1,
                           const LIMatVector* velocity0,
                           const LIMatVector* velocity1)
{
	LIParLine* line;

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

LIParPoint*
lipar_manager_insert_point (LIParManager*      self,
                            const LIMatVector* position,
                            const LIMatVector* velocity)
{
	LIParPoint* point;

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
lipar_manager_remove_line (LIParManager* self,
                           LIParLine*    line)
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
lipar_manager_remove_point (LIParManager* self,
                            LIParPoint*   point)
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
lipar_manager_update (LIParManager* self,
                      float         secs)
{
	int i;
	LIParLine* line;
	LIParLine* line_next;
	LIParPoint* point;
	LIParPoint* point_next;

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
