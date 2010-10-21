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
 * \addtogroup liser Server
 * @{
 * \addtogroup liserObserver Observer
 * @{
 */

#ifndef __SERVER_OBSERVER_H__
#define __SERVER_OBSERVER_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/engine.h>
#include "server.h"

typedef struct _LISerObserverIter LISerObserverIter;
struct _LISerObserverIter
{
	LIEngObjectIter objects;
	LIEngObject* subject;
	LIEngObject* object;
	LISerServer* server;
};

/**
 * \brief Iterates through all objects near the subject object that can see it.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Observer iterator.
 * \param object_ Object whose observers to find.
 * \param radius Maximum search radius in world units.
 */
#define LISER_FOREACH_OBSERVER(iter, object_, radius) \
	for (liser_observer_iter_first (&iter, object_, radius) ; iter.object != NULL ; \
	     liser_observer_iter_next (&iter))

static inline int
liser_observer_iter_first (LISerObserverIter* self,
                           LIEngObject*       object,
                           float              radius)
{
	LIMaiProgram* program;
	LIMatTransform transform;

	/* Initialize self. */
	memset (self, 0, sizeof (LISerObserverIter));
	if (object->sector == NULL)
		return 0;
	program = lieng_engine_get_userdata (object->engine);
	self->server = limai_program_find_component (program, "server");
	if (self->server == NULL)
		return 0;
	lieng_object_get_transform (object, &transform);
	self->subject = object;
	if (!lieng_object_iter_first (&self->objects, object->engine, &transform.position, radius))
		return 0;

	/* Find first observer. */
	while (self->objects.object != NULL)
	{
#if 0
		if (liser_network_sees (self->server->network, self->objects.object, self->subject))
		{
			self->object = self->objects.object;
			return 1;
		}
#endif
		if (!lieng_object_iter_next (&self->objects))
			return 0;
	}

	return 0;
}

static inline int
liser_observer_iter_next (LISerObserverIter* self)
{
	/* Find next observer. */
	while (self->objects.object != NULL)
	{
		if (!lieng_object_iter_next (&self->objects))
			break;
#if 0
		if (liser_network_sees (self->server->network, self->objects.object, self->subject))
		{
			self->object = self->objects.object;
			return 1;
		}
#endif
	}
	self->object = NULL;

	return 0;
}

#endif

/** @} */
/** @} */
