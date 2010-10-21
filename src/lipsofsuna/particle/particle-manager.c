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
lipar_manager_new (int lines)
{
	int i;
	LIParManager* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIParManager));
	if (self == NULL)
		return NULL;

	/* Allocate particles. */
	self->groups = lialg_strdic_new ();
	if (self->groups == NULL)
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
	LIAlgStrdicIter iter;

	if (self->groups != NULL)
	{
		LIALG_STRDIC_FOREACH (iter, self->groups)
			lipar_group_free (iter.value);
		lialg_strdic_free (self->groups);
	}
	lisys_free (self->lines.all);
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
                            const char*        texture,
                            const LIMatVector* position,
                            const LIMatVector* velocity)
{
	LIParGroup* group;

	/* Find or create group. */
	group = lialg_strdic_find (self->groups, texture);
	if (group == NULL)
	{
		group = lipar_group_new (texture);
		if (group == NULL)
			return NULL;
		if (!lialg_strdic_insert (self->groups, texture, group))
		{
			lipar_group_free (group);
			return NULL;
		}
	}

	/* Insert point to group. */
	return lipar_group_insert_point (group, position, velocity);
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
lipar_manager_update (LIParManager* self,
                      float         secs)
{
	int i;
	LIAlgStrdicIter iter;
	LIParGroup* group;
	LIParLine* line;
	LIParLine* line_next;

	/* Update groups. */
	LIALG_STRDIC_FOREACH (iter, self->groups)
	{
		group = iter.value;
		lipar_group_update (group, secs);
		if (!group->points.count_used)
		{
			lialg_strdic_remove (self->groups, iter.key);
			lipar_group_free (group);
		}
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
