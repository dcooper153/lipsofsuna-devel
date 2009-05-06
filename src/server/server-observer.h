/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvObserver Observer
 * @{
 */

#ifndef __SERVER_OBSERVER_H__
#define __SERVER_OBSERVER_H__

#include <sector/lips-sector.h>
#include <algorithm/lips-algorithm.h>
#include "server.h"

typedef struct _lisrvObserverIter lisrvObserverIter;
struct _lisrvObserverIter
{
	lisecIter sectors;
	lialgU32dicIter objects;
	liengSector* sector;
	liengObject* subject;
	liengObject* object;
};

/**
 * \brief Iterates through all objects near the subject object that can see it.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Observer iterator.
 * \param subject Pointer to an object.
 * \param radius Search radius in sectors.
 */
#define LISRV_FOREACH_OBSERVER(iter, subject, radius) \
	for (lisrv_observer_iter_first (&iter, subject, radius) ; iter.object != NULL ; \
	     lisrv_observer_iter_next (&iter))

static inline void
lisrv_observer_iter_first (lisrvObserverIter* self,
                           liengObject*       subject,
                           int                radius)
{
	liengObject* object;

	memset (self, 0, sizeof (lisrvObserverIter));
	if (subject->sector != NULL)
	{
		self->subject = subject;
		self->object = NULL;
		self->sector = NULL;
		lisec_iter_first (&self->sectors, subject->sector->id, radius);
		while (1)
		{
			/* Find non-empty sector. */
			while (self->sectors.id != LISEC_SECTOR_INVALID)
			{
				self->sector = lieng_engine_find_sector (subject->engine, self->sectors.id);
				lisec_iter_next (&self->sectors);
				if (self->sector != NULL)
					break;
			}
			if (self->sector == NULL)
				return;

			/* Find observer in sector. */
			LI_FOREACH_U32DIC (self->objects, self->sector->objects)
			{
				object = self->objects.value;
				if (lisrv_object_sees (object, self->subject))
				{
					self->object = object;
					lialg_u32dic_iter_next (&self->objects);
					return;
				}
			}
		}
	}
	else
	{
		self->subject = subject;
		self->object = NULL;
		self->sector = NULL;
	}
}

static inline void
lisrv_observer_iter_next (lisrvObserverIter* self)
{
	liengObject* object;

	/* Find next observer in current sector. */
	while (self->objects.value != NULL)
	{
		object = self->objects.value;
		if (lisrv_object_sees (object, self->subject))
		{
			self->object = object;
			lialg_u32dic_iter_next (&self->objects);
			return;
		}
		lialg_u32dic_iter_next (&self->objects);
	}

	/* Find new sector and an observer in it. */
	while (1)
	{
		/* Find non-empty sector. */
		do
		{
			lisec_iter_next (&self->sectors);
			if (self->sectors.id == LISEC_SECTOR_INVALID)
			{
				self->sector = NULL;
				self->object = NULL;
				return;
			}
			self->sector = lieng_engine_find_sector (self->subject->engine, self->sectors.id);
		}
		while (self->sector == NULL);

		/* Find observer in sector. */
		LI_FOREACH_U32DIC (self->objects, self->sector->objects)
		{
			object = self->objects.value;
			if (lisrv_object_sees (object, self->subject))
			{
				self->object = object;
				lialg_u32dic_iter_next (&self->objects);
				return;
			}
		}
	}
}

#endif

/** @} */
/** @} */
