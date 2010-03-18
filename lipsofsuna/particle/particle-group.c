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
 * \addtogroup LIParGroup Group
 * @{
 */

#include <lipsofsuna/string.h>
#include "particle-group.h"

#define LIPAR_GROUP_MIN_POINTS 128
#define LIPAR_GROUP_MAX_POINTS (1<<16) /* ~5MB */

static void
private_rebuild_lists (LIParGroup* self,
                       int         used);

/*****************************************************************************/

LIParGroup*
lipar_group_new (const char* texture)
{
	int i;
	LIParGroup* self;

	/* Allocate self. */
	self = calloc (1, sizeof (LIParGroup));
	if (self == NULL)
		return 0;
	self->texture = listr_dup (texture);
	if (self->texture == NULL)
	{
		lipar_group_free (self);
		return NULL;
	}

	/* Allocate particles. */
	self->points.count = LIPAR_GROUP_MIN_POINTS;
	self->points.all = lisys_calloc (self->points.count, sizeof (LIParPoint));
	if (self->points.all == NULL)
	{
		lipar_group_free (self);
		return NULL;
	}

	/* Add points to free list. */
	self->points.count_free = self->points.count;
	self->points.free = self->points.all;
	for (i = 0 ; i < self->points.count ; i++)
	{
		self->points.all[i].prev = self->points.all + i - 1;
		self->points.all[i].next = self->points.all + i + 1;
	}
	self->points.all[0].prev = NULL;
	self->points.all[self->points.count - 1].next = NULL;

	return self;
}

void
lipar_group_free (LIParGroup* self)
{
	lisys_free (self->points.all);
	lisys_free (self->texture);
	lisys_free (self);
}

LIParPoint*
lipar_group_insert_point (LIParGroup*        self,
                          const LIMatVector* position,
                          const LIMatVector* velocity)
{
	int cap;
	int old;
	LIParPoint* point;

	/* Get free point. */
	point = self->points.free;
	if (point == NULL)
	{
		old = self->points.count;
		cap = self->points.count << 1;
		if (cap > LIPAR_GROUP_MAX_POINTS)
			return NULL;

		/* Reallocate particles. */
		point = lisys_realloc (self->points.all, cap * sizeof (LIParPoint));
		if (point == NULL)
			return NULL;
		self->points.all = point;
		self->points.count = cap;
		point += old;
		memset (point, 0, (cap - old) * sizeof (LIParPoint));

		/* Rebuild if pointers became invalid. */
		if (point != self->points.all)
			private_rebuild_lists (self, old);
	}
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

	/* Update counts. */
	self->points.count_free--;
	self->points.count_used++;

	return point;
}

void
lipar_group_remove_point (LIParGroup* self,
                          LIParPoint* point)
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

	/* Update counts. */
	self->points.count_free++;
	self->points.count_used--;
}

void
lipar_group_update (LIParGroup* self,
                    float       secs)
{
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
			lipar_group_remove_point (self, point);
	}
}

/*****************************************************************************/

static void
private_rebuild_lists (LIParGroup* self,
                       int         used)
{
	int i;
	LIParPoint* point;

	/* Clear lists. */
	self->points.free = NULL;
	self->points.used = NULL;

	/* Insert points to used list. */
	for (i = 0 ; i < used ; i++)
	{
		point = self->points.all + i;
		point->prev = NULL;
		point->next = self->points.used;
		if (self->points.used != NULL)
			self->points.used->prev = point;
		self->points.used = point;
	}

	/* Insert points to free list. */
	for (i = used ; i < self->points.count ; i++)
	{
		point = self->points.all + i;
		point->prev = NULL;
		point->next = self->points.free;
		if (self->points.free != NULL)
			self->points.free->prev = point;
		self->points.free = point;
	}
}

/** @} */
/** @} */
