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
#include "engine-block-builder.h"
#include "engine-sector.h"

#define LIENG_SECTOR_VERSION 0xFF

static int
private_build_block (liengSector* self,
                     int          x,
                     int          y,
                     int          z);

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

	/* Free objects. */
	if (self->objects != NULL)
		lialg_u32dic_free (self->objects);

	/* Free blocks. */
	for (i = 0 ; i < LIENG_BLOCKS_PER_SECTOR ; i++)
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

	for (z = 0 ; z < LIENG_BLOCKS_PER_LINE ; z++)
	{
		for (y = 0 ; y < LIENG_BLOCKS_PER_LINE ; y++)
		{
			for (x = 0 ; x < LIENG_BLOCKS_PER_LINE ; x++)
			{
				lieng_block_fill (self->blocks + i, terrain);
				i++;
			}
		}
	}
	self->dirty = 1;
}

/**
 * \brief Fills a sphere with the given terrain type.
 *
 * \param self Sector.
 * \param box Bounding box relative to the origin of the sector.
 * \param terrain Terrain type.
 */
void
lieng_sector_fill_aabb (liengSector*     self,
                        const limatAabb* box,
                        liengTile        terrain)
{
	int i;
	int x;
	int y;
	int z;
	limatAabb block;
	limatAabb child;

	for (i = z = 0 ; z < LIENG_BLOCKS_PER_LINE ; z++)
	for (y = 0 ; y < LIENG_BLOCKS_PER_LINE ; y++)
	for (x = 0 ; x < LIENG_BLOCKS_PER_LINE ; x++, i++)
	{
		block.min = limat_vector_init (
			x * LIENG_BLOCK_WIDTH,
			y * LIENG_BLOCK_WIDTH,
			z * LIENG_BLOCK_WIDTH);
		block.max = limat_vector_init (
			(x + 1) * LIENG_BLOCK_WIDTH,
			(y + 1) * LIENG_BLOCK_WIDTH,
			(z + 1) * LIENG_BLOCK_WIDTH);
		if (limat_aabb_intersects_aabb (box, &block))
		{
			child.min = limat_vector_subtract (box->min, block.min);
			child.max = limat_vector_subtract (box->max, block.min);
			if (lieng_block_fill_aabb (self->blocks + i, &child, terrain))
				self->dirty = 1;
		}
	}
}

/**
 * \brief Fills a sphere with the given terrain type.
 *
 * \param self Sector.
 * \param center Center of the sphere relative to the origin of the sector.
 * \param radius Radius of the sphere.
 * \param terrain Terrain type.
 */
void
lieng_sector_fill_sphere (liengSector*       self,
                          const limatVector* center,
                          float              radius,
                          liengTile          terrain)
{
	int i;
	int x;
	int y;
	int z;
	float r;
	limatVector block;
	limatVector dist;

	r = radius + LIENG_BLOCK_WIDTH;
	for (i = z = 0 ; z < LIENG_BLOCKS_PER_LINE ; z++)
	for (y = 0 ; y < LIENG_BLOCKS_PER_LINE ; y++)
	for (x = 0 ; x < LIENG_BLOCKS_PER_LINE ; x++, i++)
	{
		block = limat_vector_init (
			(x + 0.5f) * LIENG_BLOCK_WIDTH,
			(y + 0.5f) * LIENG_BLOCK_WIDTH,
			(z + 0.5f) * LIENG_BLOCK_WIDTH);
		dist = limat_vector_subtract (*center, block);
		if (limat_vector_dot (dist, dist) < r * r)
		{
			block = limat_vector_init (
				x * LIENG_BLOCK_WIDTH,
				y * LIENG_BLOCK_WIDTH,
				z * LIENG_BLOCK_WIDTH);
			block = limat_vector_subtract (*center, block);
			if (lieng_block_fill_sphere (self->blocks + i, &block, radius, terrain))
				self->dirty = 1;
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
	snprintf (name, 16, "%06X", LIENG_SECTOR_INDEX (self->x, self->y, self->z));
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
 * \brief Called once per tick to update the status of the sector.
 *
 * \param self Sector.
 * \param secs Number of seconds since the last update.
 */
void
lieng_sector_update (liengSector* self,
                     float        secs)
{
	int i;
	int x;
	int y;
	int z;

	/* Rebuild changed blocks. */
	if (self->dirty)
	{
		for (i = z = 0 ; z < LIENG_BLOCKS_PER_LINE ; z++)
		for (y = 0 ; y < LIENG_BLOCKS_PER_LINE ; y++)
		for (x = 0 ; x < LIENG_BLOCKS_PER_LINE ; x++, i++)
		{
			if (!self->blocks[i].dirty)
				continue;
			private_build_block (self, x, y, z);
			self->blocks[i].dirty = 0;
		}
		self->dirty = 0;
	}
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

/**
 * \brief Sets the terrain type of a voxel.
 *
 * \param self Block.
 * \param x Offset of the voxel within the sector.
 * \param y Offset of the voxel within the sector.
 * \param z Offset of the voxel within the sector.
 * \return Terrain type or zero.
 */
liengTile
lieng_sector_get_voxel (liengSector* sector,
                        int          x,
                        int          y,
                        int          z)
{
	liengBlock* block;
	int bx = x / LIENG_TILES_PER_LINE;
	int by = y / LIENG_TILES_PER_LINE;
	int bz = z / LIENG_TILES_PER_LINE;
	int tx = x % LIENG_TILES_PER_LINE;
	int ty = y % LIENG_TILES_PER_LINE;
	int tz = z % LIENG_TILES_PER_LINE;

	if (x < 0 || y < 0 || z < 0 ||
	    bx < 0 || bx >= LIENG_BLOCKS_PER_LINE ||
	    by < 0 || by >= LIENG_BLOCKS_PER_LINE ||
	    bz < 0 || bz >= LIENG_BLOCKS_PER_LINE)
		return 0;
	block = sector->blocks + LIENG_BLOCK_INDEX (bx, by, bz);

	return lieng_block_get_voxel (block, tx, ty, tz);
}

/**
 * \brief Sets the terrain type of a voxel.
 *
 * \param self Block.
 * \param x Offset of the voxel within the sector.
 * \param y Offset of the voxel within the sector.
 * \param z Offset of the voxel within the sector.
 * \param terrain Terrain type.
 * \return Nonzero if a voxel was modified.
 */
int
lieng_sector_set_voxel (liengSector* self,
                        int          x,
                        int          y,
                        int          z,
                        liengTile    terrain)
{
	int ret;
	int bx = x / LIENG_BLOCKS_PER_LINE;
	int by = y / LIENG_BLOCKS_PER_LINE;
	int bz = z / LIENG_BLOCKS_PER_LINE;
	int tx = x % LIENG_BLOCKS_PER_LINE;
	int ty = y % LIENG_BLOCKS_PER_LINE;
	int tz = z % LIENG_BLOCKS_PER_LINE;
	liengBlock* block;
	limatVector offset;

	if (x < 0 || y < 0 || z < 0 ||
	    bx < 0 || bx >= LIENG_BLOCKS_PER_LINE || 
	    by < 0 || by >= LIENG_BLOCKS_PER_LINE || 
	    bz < 0 || bz >= LIENG_BLOCKS_PER_LINE)
	{
		assert (0);
		return 0;
	}
	block = self->blocks + LIENG_BLOCK_INDEX (bx, by, bz);
	ret = lieng_block_set_voxel (block, tx, ty, tz, terrain);
	if (ret)
	{
		offset = limat_vector_init (
			self->origin.x + LIENG_BLOCK_WIDTH * bx,
			self->origin.y + LIENG_BLOCK_WIDTH * by,
			self->origin.z + LIENG_BLOCK_WIDTH * bz);
	}

	return ret;
}

/*****************************************************************************/

static int
private_build_block (liengSector* self,
                     int          x,
                     int          y,
                     int          z)
{
	liengBlock* block;
	liengBlockBuilder* builder;
	limdlModel* model;
	lirndModel* rndmdl;

	/* Optimize block. */
	block = self->blocks + LIENG_BLOCK_INDEX (x, y, z);
	lieng_block_optimize (block);

	/* Generate mesh. */
	builder = lieng_block_builder_new (self);
	if (builder == NULL)
		return 0;
	if (!lieng_block_builder_build (builder, x, y, z))
	{
		lieng_block_builder_free (builder);
		return 0;
	}

	/* Replace physics data. */
	if (block->physics != NULL)
		liphy_object_free (block->physics);
	if (block->shape != NULL)
		liphy_shape_free (block->shape);
	block->physics = NULL;
	block->shape = lieng_block_builder_get_shape (builder);
	if (block->shape != NULL)
	{
		block->physics = liphy_object_new (self->engine->physics, block->shape,
			LIPHY_SHAPE_MODE_CONCAVE, LIPHY_CONTROL_MODE_STATIC);
		if (block->physics != NULL)
		{
			liphy_object_set_collision_group (block->physics, LIENG_PHYSICS_GROUP_TILES);
			liphy_object_set_collision_mask (block->physics,
				~(LIENG_PHYSICS_GROUP_TILES | LIENG_PHYSICS_GROUP_STATICS));
			liphy_object_set_realized (block->physics, 1);
		}
	}

	/* Replace render data. */
#ifndef LIENG_DISABLE_GRAPHICS
	if (self->engine->render != NULL)
	{
		if (block->render != NULL)
		{
			model = block->render->model->model;
			rndmdl = block->render->model;
			self->engine->renderapi->lirnd_object_free (block->render);
			self->engine->renderapi->lirnd_model_free (rndmdl);
			limdl_model_free (model);
		}
		model = lieng_block_builder_get_model (builder);
		rndmdl = self->engine->renderapi->lirnd_model_new (self->engine->render, model);
		block->render = self->engine->renderapi->lirnd_object_new (self->engine->render, 0);
		if (block->render == NULL)
		{
			self->engine->renderapi->lirnd_model_free (rndmdl);
			limdl_model_free (model);
			lieng_block_builder_free (builder);
			return 0;
		}
		if (!self->engine->renderapi->lirnd_object_set_model (block->render, rndmdl))
		{
			self->engine->renderapi->lirnd_object_free (block->render);
			self->engine->renderapi->lirnd_model_free (rndmdl);
			limdl_model_free (model);
			block->render = NULL;
			lieng_block_builder_free (builder);
			return 0;
		}
		self->engine->renderapi->lirnd_object_set_realized (block->render, 1);
	}
#endif

	lieng_block_builder_free (builder);

	return 1;
}

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
