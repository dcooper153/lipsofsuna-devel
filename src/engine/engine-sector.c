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
#include <sector/lips-sector.h>
#include <string/lips-string.h>
#include <system/lips-system.h>
#include "engine-sector.h"

#define LIENG_SECTOR_VERSION 0xFF

static int
private_init_objects (liengSector* self,
                      const char*  dir,
                      const char*  name);

/*****************************************************************************/

/**
 * \brief Creates a new sector.
 *
 * If NULL is passed as the data directory to the function, an empty sector
 * is always created without any attempts to load data from disk.
 *
 * \param engine Engine.
 * \param id Sector number.
 * \param dir Path to data directory or NULL.
 */
liengSector*
lieng_sector_new (liengEngine* engine,
                  uint32_t     id,
                  const char*  dir)
{
	char name[16];
	liengSector* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liengSector));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->id = id;
	self->engine = engine;
	lisec_pointer_get_origin (id, &self->origin);

	/* Insert to engine. */
	if (!lialg_u32dic_insert (engine->sectors, self->id, self))
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

	/* Load sector data. */
	if (dir != NULL)
	{
		snprintf (name, 16, "%06X", id);
		if (!private_init_objects (self, dir, name))
		{
			if (lisys_error_peek () != EIO)
			{
				lisys_error_append ("cannot load sector `%s'", name);
				lieng_sector_free (self);
				return NULL;
			}
			lisys_error_get (NULL);
		}
	}

	return self;
}

/**
 * \brief Frees the sector.
 *
 * \param self Sector.
 */
void
lieng_sector_free (liengSector* self)
{
	int i;

	/* Remove from engine. */
	lialg_u32dic_remove (self->engine->sectors, self->id);

	/* Free objects. */
	if (self->objects != NULL)
		lialg_u32dic_free (self->objects);

	/* Free blocks. */
	for (i = 0 ; i < LIENG_SECTOR_BLOCK_TOTAL ; i++)
		lieng_block_free (self->blocks + i);

	free (self);
}

/**
 * \brief Fills the sector with the given terrain type.
 *
 * \param self Sector.
 * \param terrain Terrain type.
 */
void
lieng_sector_fill (liengSector* self,
                   liengTile    terrain)
{
	int x;
	int y;
	int z;
	int i = 0;
	limatVector offset;

	for (z = 0 ; z < LIENG_SECTOR_BLOCK_ROWS ; z++)
	{
		for (y = 0 ; y < LIENG_SECTOR_BLOCK_ROWS ; y++)
		{
			for (x = 0 ; x < LIENG_SECTOR_BLOCK_ROWS ; x++)
			{
				offset = limat_vector_init (
					self->origin.x + 8 * x,
					self->origin.y + 8 * y,
					self->origin.z + 8 * z);
				lieng_block_fill (self->blocks + i, terrain);
				lieng_block_rebuild (self->blocks + i, self->engine, &offset);
				i++;
			}
		}
	}
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
 * \brief Saves the sector to disk.
 *
 * \param self Sector.
 * \return Nonzero on success.
 */
int
lieng_sector_save (liengSector* self)
{
	int num;
	char name[16];
	char* path;
	lialgU32dicIter iter;
	liarcSerialize* serialize;
	liengObject* object;

	/* Count objects. */
	num = 0;
	LI_FOREACH_U32DIC (iter, self->objects)
	{
		object = iter.value;
		if (object->flags & LIENG_OBJECT_FLAG_SAVE)
			num++;
	}
	if (!num)
		return 1;

	/* Create serializer. */
	snprintf (name, 16, "%06X", self->id);
	path = lisys_path_format (self->engine->config.dir,
		LISYS_PATH_SEPARATOR, "world",
		LISYS_PATH_SEPARATOR, name, ".lsec", NULL);
	if (path == NULL)
		return 0;
	serialize = liarc_serialize_new_write (path);
	free (path);
	if (serialize == NULL)
		return 0;

	/* Write header. */
	if (!liarc_writer_append_string (serialize->writer, "lips/sec") ||
	    !liarc_writer_append_nul (serialize->writer) ||
	    !liarc_writer_append_uint32 (serialize->writer, LIENG_SECTOR_VERSION) ||
	    !liarc_writer_append_uint32 (serialize->writer, num))
	{
		liarc_serialize_free (serialize);
		return 0;
	}

	/* Write objects. */
	LI_FOREACH_U32DIC (iter, self->objects)
	{
		object = iter.value;
		if (object->flags & LIENG_OBJECT_FLAG_SAVE)
		{
			if (!lieng_engine_write_object (self->engine, serialize, object))
			{
				liarc_serialize_free (serialize);
				return 0;
			}
		}
	}

	liarc_serialize_free (serialize);

	return 1;
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
	max = limat_vector_init (LISEC_SECTOR_SIZE, LISEC_SECTOR_SIZE, LISEC_SECTOR_SIZE);
	max = limat_vector_add (min, max);
	limat_aabb_init_from_points (result, &min, &max);
}

/**
 * \brief Sets the terrain type of a tile.
 *
 * \param self Block.
 * \param x Offset of the tile within the sector.
 * \param y Offset of the tile within the sector.
 * \param z Offset of the tile within the sector.
 * \param terrain Terrain type.
 * \return Nonzero if a tile was modified.
 */
int
lieng_sector_set_tile (liengSector* self,
                       int          x,
                       int          y,
                       int          z,
                       liengTile    terrain)
{
	int i;
	int ret;
	int bx = x / 8;
	int by = y / 8;
	int bz = z / 8;
	int tx = x % 8;
	int ty = y % 8;
	int tz = z % 8;
	limatVector offset;

	if (bx <= 0 || bx >= LIENG_SECTOR_BLOCK_ROWS || 
	    by <= 0 || by >= LIENG_SECTOR_BLOCK_ROWS || 
	    bz <= 0 || bz >= LIENG_SECTOR_BLOCK_ROWS)
	{
		assert (0);
		return 0;
	}
	i = bx + (by + bz * LIENG_SECTOR_BLOCK_ROWS) * LIENG_SECTOR_BLOCK_ROWS;
	offset = limat_vector_init (
		self->origin.x + 8 * bx,
		self->origin.y + 8 * by,
		self->origin.z + 8 * bz);
	ret = lieng_block_set_tile (self->blocks + i, tx, ty, tz, terrain);
	if (ret)
		lieng_block_rebuild (self->blocks + i, self->engine, &offset);

	return ret;
}

/*****************************************************************************/

static int
private_init_objects (liengSector* self,
                      const char*  dir,
                      const char*  name)
{
	int ret;
	char* path;
	uint32_t i;
	uint32_t size;
	uint32_t version;
	liarcSerialize* serialize;
	liengObject* object;

	/* Construct path. */
	path = lisys_path_format (dir,
		LISYS_PATH_SEPARATOR, "world",
		LISYS_PATH_SEPARATOR, name, ".lsec", NULL);
	if (path == NULL)
		return 0;

	/* Open file. */
	serialize = liarc_serialize_new_read (path);
	free (path);
	if (serialize == NULL)
		return 0;

	/* Read header. */
	if (!li_reader_check_text (serialize->reader, "lips/sec", ""))
	{
		lisys_error_set (EINVAL, "incorrect file type");
		goto error;
	}
	if (!li_reader_get_uint32 (serialize->reader, &version) ||
	    !li_reader_get_uint32 (serialize->reader, &size))
		goto error;
	if (version != LIENG_SECTOR_VERSION)
	{
		lisys_error_set (LI_ERROR_VERSION, "incompatible sector version");
		goto error;
	}

	/* Read objects. */
	for (i = 0 ; i < size ; i++)
	{
		ret = lieng_engine_read_object (self->engine, serialize, &object);
		if (!ret)
			lisys_error_report ();
	}

	/* Check for end .*/
	if (!li_reader_check_end (serialize->reader))
	{
		lisys_error_set (EINVAL, "end of stream expected");
		goto error;
	}
	liarc_serialize_free (serialize);

	return 1;

error:
	liarc_serialize_free (serialize);
	return 0;
}

/** @} */
/** @} */
