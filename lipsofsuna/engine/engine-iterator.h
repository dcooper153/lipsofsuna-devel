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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengIterator Iterator
 * @{
 */

#ifndef __ENGINE_ITERATOR_H__
#define __ENGINE_ITERATOR_H__

#include <lipsofsuna/algorithm.h>
#include "engine-sector.h"
#include "engine-selection.h"

typedef struct _LIEngObjectIter LIEngObjectIter;
struct _LIEngObjectIter
{
	LIAlgSectorsIter sectors;
	LIAlgU32dicIter objects;
	LIEngObject* object;
};

/**
 * \brief Iterates through objects in a range of sectors.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Object iterator.
 * \param engine Engine.
 * \param point Search center in world coordinates.
 * \param radius Search radius hint in world units.
 */
#define LIENG_FOREACH_OBJECT(iter, engine, point, radius) \
	for (lieng_object_iter_first (&iter, engine, point, radius) ; iter.object != NULL ; \
	     lieng_object_iter_next (&iter))

static inline int
lieng_object_iter_first (LIEngObjectIter*   self,
                         LIEngEngine*       engine,
                         const LIMatVector* point,
                         int                radius)
{
	LIEngSector* sector;

	/* Initialize self. */
	memset (self, 0, sizeof (LIEngObjectIter));
	if (!lialg_sectors_iter_first_point (&self->sectors, engine->sectors, point, radius))
		return 0;

	/* Find first object. */
	while (self->sectors.sector != NULL)
	{
		sector = lialg_strdic_find (self->sectors.sector->content, "engine");
		if (sector != NULL)
		{
			lialg_u32dic_iter_start (&self->objects, sector->objects);
			if (self->objects.value != NULL)
			{
				self->object = self->objects.value;
				return 1;
			}
		}
		if (!lialg_sectors_iter_next (&self->sectors))
			break;
	}

	return 0;
}

static inline int
lieng_object_iter_next (LIEngObjectIter* self)
{
	LIEngSector* sector;

	self->object = NULL;

	/* Find next object in sector. */
	while (self->objects.value != NULL)
	{
		if (lialg_u32dic_iter_next (&self->objects))
		{
			self->object = self->objects.value;
			return 1;
		}
	}

	/* Find next object in another sector. */
	while (lialg_sectors_iter_next (&self->sectors))
	{
		sector = lialg_strdic_find (self->sectors.sector->content, "engine");
		if (sector != NULL)
		{
			lialg_u32dic_iter_start (&self->objects, sector->objects);
			if (self->objects.value != NULL)
			{
				self->object = self->objects.value;
				return 1;
			}
		}
	}

	return 0;
}

/*****************************************************************************/

typedef struct _LIEngSelectionIter LIEngSelectionIter;
struct _LIEngSelectionIter
{
	LIAlgPtrdicIter objects;
	LIEngObject* object;
};

/**
 * \brief Iterates through all currently selected objects.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Selection iterator.
 * \param engine Engine.
 */
#define LIENG_FOREACH_SELECTION(iter, engine) \
	for (lieng_selection_iter_first (&iter, engine) ; iter.object != NULL ; \
	     lieng_selection_iter_next (&iter))

static inline int
lieng_selection_iter_first (LIEngSelectionIter* self,
                            LIEngEngine*        engine)
{
	LIEngSelection* selection;

	/* Find first object. */
	lialg_ptrdic_iter_start (&self->objects, engine->selection);
	if (self->objects.value != NULL)
	{
		selection = self->objects.value;
		self->object = selection->object;
		return 1;
	}
	else
	{
		self->object = NULL;
		return 1;
	}
}

static inline int
lieng_selection_iter_next (LIEngSelectionIter* self)
{
	LIEngSelection* selection;

	if (lialg_ptrdic_iter_next (&self->objects))
	{
		selection = self->objects.value;
		self->object = selection->object;
		return 1;
	}
	else
	{
		self->object = NULL;
		return 0;
	}
}

#endif

/** @} */
/** @} */
