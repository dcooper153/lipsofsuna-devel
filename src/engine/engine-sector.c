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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengSector Sector
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string/lips-string.h>
#include <system/lips-system.h>
#include "engine-sector.h"

liengSector*
lieng_sector_default_new (liengEngine* engine,
                          uint32_t     id)
{
	liengSector* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liengSector));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->x = id % LIENG_SECTORS_PER_LINE;
	self->y = id / LIENG_SECTORS_PER_LINE % LIENG_SECTORS_PER_LINE;
	self->z = id / LIENG_SECTORS_PER_LINE / LIENG_SECTORS_PER_LINE;
	self->engine = engine;
	self->origin.x = LIENG_SECTOR_WIDTH * self->x;
	self->origin.y = LIENG_SECTOR_WIDTH * self->y;
	self->origin.z = LIENG_SECTOR_WIDTH * self->z;

	/* Insert to engine. */
	if (!lialg_u32dic_insert (engine->sectors, id, self))
	{
		free (self);
		return NULL;
	}

	/* Allocate tree. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		lieng_sector_free (self);
		return NULL;
	}

	/* Invoke callbacks. */
	lieng_engine_call (engine, LIENG_CALLBACK_SECTOR_LOAD, self);

	return self;
}

/**
 * \brief Creates a new sector.
 *
 * \param engine Engine.
 * \param id Sector number.
 * \return New sector or NULL.
 */
liengSector*
lieng_sector_new (liengEngine* engine,
                  uint32_t     id)
{
	return engine->calls.lieng_sector_new (engine, id);
}

/**
 * \brief Frees the sector.
 *
 * \param self Sector.
 */
void
lieng_sector_free (liengSector* self)
{
	/* Free objects. */
	if (self->objects != NULL)
		lialg_u32dic_free (self->objects);

	free (self);
}

/**
 * \brief Inserts an object to the sector.
 *
 * \param self Sector.
 * \param object Object.
 */
int
lieng_sector_insert_object (liengSector* self,
                            liengObject* object)
{
	assert (!lialg_u32dic_find (self->objects, object->id));

	if (lialg_u32dic_insert (self->objects, object->id, object) == NULL)
		return 0;
	return 1;
}

/**
 * \brief Removes an object from the sector.
 *
 * \param self Sector.
 * \param object Object.
 */
void
lieng_sector_remove_object (liengSector* self,
                            liengObject* object)
{
	assert (lialg_u32dic_find (self->objects, object->id));

	lialg_u32dic_remove (self->objects, object->id);
}

/**
 * \brief Called once per tick to update the status of the sector.
 *
 * \param self Sector.
 * \param secs Number of seconds since the last update.
 */
void
lieng_sector_update (liengSector* self,
                     float        secs)
{
}

/**
 * \brief Gets the bounding box of the sector.
 *
 * \param self Sector.
 * \param result Return location for the bounding box.
 */
void
lieng_sector_get_bounds (const liengSector* self,
                         limatAabb*         result)
{
	limatVector min;
	limatVector max;

	min = self->origin;
	max = limat_vector_init (LIENG_SECTOR_WIDTH, LIENG_SECTOR_WIDTH, LIENG_SECTOR_WIDTH);
	max = limat_vector_add (min, max);
	limat_aabb_init_from_points (result, &min, &max);
}

/** @} */
/** @} */
