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
 * \addtogroup LIEngSector Sector
 * @{
 */

#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "engine-sector.h"

/**
 * \brief Creates a new sector.
 *
 * \param sector Sector manager sector.
 * \return New sector or NULL.
 */
LIEngSector*
lieng_sector_new (LIAlgSector* sector)
{
	LIEngSector* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIEngSector));
	if (self == NULL)
		return NULL;
	self->engine = lialg_sectors_get_userdata (sector->manager, "engine");
	self->sector = sector;

	/* Allocate tree. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		lieng_sector_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the sector.
 *
 * \param self Sector.
 */
void
lieng_sector_free (LIEngSector* self)
{
	LIAlgU32dicIter iter;
	LIEngObject* object;

	/* Remove objects. */
	if (self->objects != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->objects)
		{
			object = iter.value;
			lieng_object_set_realized (object, 0);
		}
		lialg_u32dic_free (self->objects);
	}

	lisys_free (self);
}

/**
 * \brief Inserts an object to the sector.
 *
 * \param self Sector.
 * \param object Object.
 */
int
lieng_sector_insert_object (LIEngSector* self,
                            LIEngObject* object)
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
lieng_sector_remove_object (LIEngSector* self,
                            LIEngObject* object)
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
lieng_sector_update (LIEngSector* self,
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
lieng_sector_get_bounds (const LIEngSector* self,
                         LIMatAabb*         result)
{
	LIMatVector min;
	LIMatVector max;

	min = self->sector->position;
	max = limat_vector_init (LIENG_SECTOR_WIDTH, LIENG_SECTOR_WIDTH, LIENG_SECTOR_WIDTH);
	max = limat_vector_add (min, max);
	limat_aabb_init_from_points (result, &min, &max);
}

/** @} */
/** @} */
