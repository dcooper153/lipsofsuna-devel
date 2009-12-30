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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup livoxManager Manager
 * @{
 */

#include <algorithm/lips-algorithm.h>
#include "voxel-iterator.h"
#include "voxel-manager.h"
#include "voxel-material.h"
#include "voxel-private.h"

#define VOXEL_BORDER_TOLERANCE 0.05f

static void
private_clear_materials (livoxManager* self);

static void
private_clear_sectors (livoxManager* self);

static int
private_ensure_materials (livoxManager* self);

static int
private_ensure_sectors (livoxManager* self);

static void
private_mark_block (livoxManager* self,
                    livoxSector*  sector,
                    int           x,
                    int           y,
                    int           z);

/*****************************************************************************/

livoxManager*
livox_manager_new ()
{
	livoxManager* self;

	self = lisys_calloc (1, sizeof (livoxManager));
	if (self == NULL)
		return NULL;

	self->materials = lialg_u32dic_new ();
	if (self->materials == NULL)
	{
		livox_manager_free (self);
		return NULL;
	}

	self->sectors = lialg_u32dic_new ();
	if (self->sectors == NULL)
	{
		livox_manager_free (self);
		return NULL;
	}

	self->callbacks = lical_callbacks_new ();
	if (self->callbacks == NULL)
	{
		livox_manager_free (self);
		return NULL;
	}

	return self;
}

void
livox_manager_free (livoxManager* self)
{
	if (self->materials != NULL)
	{
		private_clear_materials (self);
		lialg_u32dic_free (self->materials);
	}
	if (self->sectors != NULL)
	{
		private_clear_sectors (self);
		lialg_u32dic_free (self->sectors);
	}
	lisys_free (self);
}

/**
 * \brief Checks if the voxel is an occluder.
 *
 * \param self Voxel manager.
 * \param voxel Voxel.
 * \return Nonzero if occluder.
 */
int
livox_manager_check_occluder (const livoxManager* self,
                              const livoxVoxel*   voxel)
{
	livoxMaterial* material;

	if (!voxel->type)
		return 0;
	material = livox_manager_find_material ((livoxManager*) self, voxel->type);
	if (material == NULL)
		return 0;

	return (material->flags & LIVOX_MATERIAL_FLAG_OCCLUDER) != 0;
}

/**
 * \brief Removes all the sectors.
 *
 * \param self Voxel manager.
 */
void
livox_manager_clear (livoxManager* self)
{
	private_clear_sectors (self);
}

/**
 * \brief Removes all the materials.
 *
 * \param self Voxel manager.
 */
void
livox_manager_clear_materials (livoxManager* self)
{
	private_clear_materials (self);
}

/**
 * \brief Copies a box of voxels from the currently loaded scene.
 *
 * \param self Voxel manager.
 * \param xstart Start voxel in voxels in world space.
 * \param ystart Start voxel in voxels in world space.
 * \param zstart Start voxel in voxels in world space.
 * \param xsize Number of voxels to copy.
 * \param ysize Number of voxels to copy.
 * \param zsize Number of voxels to copy.
 * \param result Buffer with room for xsize*ysize*zsize voxels.
 */
void
livox_manager_copy_voxels (livoxManager* self,
                           int           xstart,
                           int           ystart,
                           int           zstart,
                           int           xsize,
                           int           ysize,
                           int           zsize,
                           livoxVoxel*   result)
{
	int i;
	int x;
	int y;
	int z;
	int sx;
	int sy;
	int sz;
	int idx;
	livoxSector* sec;

	/* FIXME: Avoid excessive sector lookups. */
	for (i = 0, z = zstart ; z < zstart + zsize ; z++)
	for (y = ystart ; y < ystart + ysize ; y++)
	for (x = xstart ; x < xstart + xsize ; x++, i++)
	{
		sx = x / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
		sy = y / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
		sz = z / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
		idx = LIVOX_SECTOR_INDEX (sx, sy, sz);
		sec = livox_manager_load_sector (self, idx);
		if (sec != NULL)
		{
			result[i] = *livox_sector_get_voxel (sec,
				x - sx * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
				y - sy * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
				z - sz * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE));
		}
		else
			livox_voxel_init (result + i, 1);
	}
}

/**
 * \brief Creates an empty sector.
 *
 * If a sector with the same number already exists, it is returned instead of
 * creating a new one.
 *
 * \param self Voxel manager.
 * \param id Sector index.
 * \return Sector owned by the manager or NULL.
 */
livoxSector*
livox_manager_create_sector (livoxManager* self,
                             uint32_t      id)
{
	livoxSector* sector;

	sector = livox_manager_find_sector (self, id);
	if (sector != NULL)
		return sector;
	return livox_sector_new (self, id);
}

int
livox_manager_erase_voxel (livoxManager*      self,
                           const limatVector* point)
{
	float d;
	lialgRange range;
	limatVector diff;
	limatVector origin;
	livoxVoxel voxel;
	livoxVoxelIter iter;
	struct
	{
		int x;
		int y;
		int z;
		float dist;
		livoxSector* sector;
	}
	best = { 0, 0, 0, 10.0E10f, NULL };

	/* Loop through affected sectors. */
	range = lialg_range_new_from_sphere (point, LIVOX_TILE_WIDTH, LIVOX_TILE_WIDTH);
	LIVOX_VOXEL_FOREACH (iter, self, range, 1)
	{
		livox_sector_get_origin (iter.sector, &origin);
		voxel = *livox_sector_get_voxel (iter.sector, iter.voxel[0], iter.voxel[1], iter.voxel[2]);
		if (voxel.type)
		{
			diff = limat_vector_subtract (*point, limat_vector_init (
				origin.x + LIVOX_TILE_WIDTH * (iter.voxel[0] + 0.5f),
				origin.y + LIVOX_TILE_WIDTH * (iter.voxel[1] + 0.5f),
				origin.z + LIVOX_TILE_WIDTH * (iter.voxel[2] + 0.5f)));
			d = limat_vector_dot (diff, diff);
			if (best.sector == NULL || d < best.dist)
			{
				best.x = iter.voxel[0];
				best.y = iter.voxel[1];
				best.z = iter.voxel[2];
				best.dist = d;
				best.sector = iter.sector;
			}
		}
	}

	/* Erase the best match. */
	if (best.sector != NULL)
	{
		livox_voxel_init (&voxel, 0);
		return livox_sector_set_voxel (best.sector, best.x, best.y, best.z, voxel);
	}

	return 0;
}

livoxMaterial*
livox_manager_find_material (livoxManager* self,
                             uint32_t      id)
{
	return lialg_u32dic_find (self->materials, id);
}

livoxSector*
livox_manager_find_sector (livoxManager* self,
                           uint32_t      id)
{
	return lialg_u32dic_find (self->sectors, id);
}

/**
 * \brief Finds the nearest voxel to the given point.
 *
 * \param self Voxel manager.
 * \param flags Search flags.
 * \param point Point in world space.
 * \param center Return location for voxel center or NULL.
 *
 * \return Voxel or NULL.
 */
livoxVoxel*
livox_manager_find_voxel (livoxManager*      self,
                          int                flags,
                          const limatVector* point,
                          limatVector*       center)
{
	float d;
	lialgRange range;
	limatVector tmp;
	limatVector diff;
	limatVector origin;
	livoxVoxel voxel;
	livoxVoxelIter iter;
	struct
	{
		int x;
		int y;
		int z;
		float dist;
		limatVector center;
		livoxSector* sector;
	}
	best = { 0, 0, 0, 10.0E10f, { 0.0f, 0.0f, 0.0f }, NULL };

	/* Loop through affected sectors. */
	range = lialg_range_new_from_sphere (point, LIVOX_TILE_WIDTH, LIVOX_TILE_WIDTH);
	LIVOX_VOXEL_FOREACH (iter, self, range, 1)
	{
		livox_sector_get_origin (iter.sector, &origin);
		voxel = *livox_sector_get_voxel (iter.sector, iter.voxel[0], iter.voxel[1], iter.voxel[2]);
		if ((!voxel.type && (flags & LIVOX_FIND_EMPTY)) ||
			( voxel.type && (flags & LIVOX_FIND_FULL)))
		{
			tmp = limat_vector_init (
				origin.x + LIVOX_TILE_WIDTH * (iter.voxel[0] + 0.5f),
				origin.y + LIVOX_TILE_WIDTH * (iter.voxel[1] + 0.5f),
				origin.z + LIVOX_TILE_WIDTH * (iter.voxel[2] + 0.5f));
			diff = limat_vector_subtract (*point, tmp);
			d = limat_vector_dot (diff, diff);
			if (best.sector == NULL || d < best.dist)
			{
				best.x = iter.voxel[0];
				best.y = iter.voxel[1];
				best.z = iter.voxel[2];
				best.dist = d;
				best.center = tmp;
				best.sector = iter.sector;
			}
		}
	}

	if (best.sector == NULL)
		return NULL;
	if (center != NULL)
		*center = best.center;

	return livox_sector_get_voxel (best.sector, best.x, best.y, best.z);
}

/**
 * \brief Inserts a material to the material database.
 *
 * Any existing material with a conflicting ID is removed and freed
 * before attemtpting to insert the new material.
 *
 * The ownership of the material is transfered to the material manager
 * upon success.
 *
 * \param self Voxel manager.
 * \param material Material.
 * \return Nonzeron on success.
 */
int
livox_manager_insert_material (livoxManager*  self,
                               livoxMaterial* material)
{
	livoxMaterial* tmp;

	tmp = lialg_u32dic_find (self->materials, material->id);
	if (tmp != NULL)
	{
		lialg_u32dic_remove (self->materials, material->id);
		livox_material_free (tmp);
	}
	if (!lialg_u32dic_insert (self->materials, material->id, material))
		return 0;

	return 1;
}

int
livox_manager_insert_voxel (livoxManager*      self,
                            const limatVector* point,
                            const livoxVoxel*  terrain)
{
	float d;
	lialgRange range;
	limatVector diff;
	limatVector origin;
	livoxVoxel* voxel;
	livoxVoxelIter iter;
	struct
	{
		int x;
		int y;
		int z;
		float dist;
		livoxSector* sector;
	}
	best = { 0, 0, 0, 10.0E10f, NULL };

	/* Loop through affected sectors. */
	range = lialg_range_new_from_sphere (point, LIVOX_TILE_WIDTH, LIVOX_TILE_WIDTH);
	LIVOX_VOXEL_FOREACH (iter, self, range, 1)
	{
		livox_sector_get_origin (iter.sector, &origin);
		voxel = livox_sector_get_voxel (iter.sector, iter.voxel[0], iter.voxel[1], iter.voxel[2]);
		if (!voxel->type)
		{
			diff = limat_vector_subtract (*point, limat_vector_init (
				origin.x + LIVOX_TILE_WIDTH * (iter.voxel[0] + 0.5f),
				origin.y + LIVOX_TILE_WIDTH * (iter.voxel[1] + 0.5f),
				origin.z + LIVOX_TILE_WIDTH * (iter.voxel[2] + 0.5f)));
			d = limat_vector_dot (diff, diff);
			if (best.sector == NULL || d < best.dist)
			{
				best.x = iter.voxel[0];
				best.y = iter.voxel[1];
				best.z = iter.voxel[2];
				best.dist = d;
				best.sector = iter.sector;
			}
		}
	}

	/* Fill the best match. */
	if (best.sector != NULL)
		return livox_sector_set_voxel (best.sector, best.x, best.y, best.z, *terrain);

	return 0;
}

/**
 * \brief Loads materials from an SQL database.
 *
 * \brief Voxel manager.
 * \return Nonzero on success.
 */
int
livox_manager_load_materials (livoxManager* self)
{
	int ret;
	const char* query;
	livoxMaterial* material;
	sqlite3_stmt* statement;

	if (self->sql == NULL)
	{
		lisys_error_set (EINVAL, "no database");
		return 0;
	}

	/* Prepare statement. */
	query = "SELECT id,flags,fric,name,model FROM voxel_materials;";
	if (sqlite3_prepare_v2 (self->sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self->sql));
		return 0;
	}

	/* Read materials. */
	for (ret = sqlite3_step (statement) ; ret != SQLITE_DONE ; ret = sqlite3_step (statement))
	{
		/* Check for errors. */
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self->sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Read material. */
		material = livox_material_new_from_sql (self->sql, statement);
		if (material == NULL)
		{
			sqlite3_finalize (statement);
			return 0;
		}
		assert (lialg_u32dic_find (self->materials, material->id) == NULL);

		/* Add to material list. */
		if (!lialg_u32dic_insert (self->materials, material->id, material))
		{
			livox_material_free (material);
			sqlite3_finalize (statement);
			return 0;
		}
	}
	sqlite3_finalize (statement);

	return 1;
}

/**
 * \brief Loads a sector from the SQL database.
 *
 * If a sector with the same number already exists, no loading takes place.
 * If a sector with the requested ID cannot be found from the database,
 * an empty sector is created.
 *
 * \param self Voxel manager.
 * \param id Sector index.
 * \return Sector owned by the manager or NULL.
 */
livoxSector*
livox_manager_load_sector (livoxManager* self,
                           uint32_t      id)
{
	livoxSector* sector;

	sector = livox_manager_find_sector (self, id);
	if (sector != NULL)
		return sector;
	sector = livox_sector_new (self, id);
	if (sector == NULL)
		return NULL;
	if (self->sql != NULL)
		livox_sector_read (sector, self->sql);

	return sector;
}

void
livox_manager_mark_updates (livoxManager* self)
{
	int i;
	int j;
	int x;
	int y;
	int z;
	lialgU32dicIter iter;
	livoxSector* sector;
	struct
	{
		int x;
		int y;
		int z;
		int mask;
	}
	neighbors[26] =
	{
		{ -1, -1, -1, 0x01|0x04|0x10 },
		{  0, -1, -1, 0x00|0x04|0x10 },
		{  1, -1, -1, 0x02|0x04|0x10 },
		{ -1,  0, -1, 0x01|0x00|0x10 },
		{  0,  0, -1, 0x00|0x00|0x10 },
		{  1,  0, -1, 0x02|0x00|0x10 },
		{ -1,  1, -1, 0x01|0x08|0x10 },
		{  0,  1, -1, 0x00|0x08|0x10 },
		{  1,  1, -1, 0x02|0x08|0x10 },
		{ -1, -1,  0, 0x01|0x04|0x00 },
		{  0, -1,  0, 0x00|0x04|0x00 },
		{  1, -1,  0, 0x02|0x04|0x00 },
		{ -1,  0,  0, 0x01|0x00|0x00 },
		{  1,  0,  0, 0x02|0x00|0x00 },
		{ -1,  1,  0, 0x01|0x08|0x00 },
		{  0,  1,  0, 0x00|0x08|0x00 },
		{  1,  1,  0, 0x02|0x08|0x00 },
		{ -1, -1,  1, 0x01|0x04|0x20 },
		{  0, -1,  1, 0x00|0x04|0x20 },
		{  1, -1,  1, 0x02|0x04|0x20 },
		{ -1,  0,  1, 0x01|0x00|0x20 },
		{  0,  0,  1, 0x00|0x00|0x20 },
		{  1,  0,  1, 0x02|0x00|0x20 },
		{ -1,  1,  1, 0x01|0x08|0x20 },
		{  0,  1,  1, 0x00|0x08|0x20 },
		{  1,  1,  1, 0x02|0x08|0x20 },
	};

	/* Update block boundaries. */
	LI_FOREACH_U32DIC (iter, self->sectors)
	{
		sector = iter.value;
		if (!sector->dirty)
			continue;
		for (i = z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
		for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
		for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++, i++)
		{
			for (j = 0 ; j < 26 ; j++)
			{
				if ((sector->blocks[i].dirty & neighbors[j].mask) != neighbors[j].mask)
					continue;
				private_mark_block (self, sector,
					neighbors[j].x + x,
					neighbors[j].y + y,
					neighbors[j].z + z);
			}
		}
	}
}

/**
 * \brief Pastes a box of voxels from the scene.
 *
 * \param self Voxel manager.
 * \param xstart Start voxel in voxels in world space.
 * \param ystart Start voxel in voxels in world space.
 * \param zstart Start voxel in voxels in world space.
 * \param xsize Number of voxels to paste.
 * \param ysize Number of voxels to paste.
 * \param zsize Number of voxels to paste.
 * \param voxels Buffer containing xsize*ysize*zsize voxels.
 */
void
livox_manager_paste_voxels (livoxManager* self,
                            int           xstart,
                            int           ystart,
                            int           zstart,
                            int           xsize,
                            int           ysize,
                            int           zsize,
                            livoxVoxel*   voxels)
{
	int i;
	int x;
	int y;
	int z;
	int sx;
	int sy;
	int sz;
	int idx;
	livoxSector* sec;

	/* FIXME: Avoid excessive sector lookups. */
	for (i = 0, z = zstart ; z < zstart + zsize ; z++)
	for (y = ystart ; y < ystart + ysize ; y++)
	for (x = xstart ; x < xstart + xsize ; x++, i++)
	{
		sx = x / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
		sy = y / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
		sz = z / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
		idx = LIVOX_SECTOR_INDEX (sx, sy, sz);
		sec = livox_manager_load_sector (self, idx);
		if (sec != NULL)
		{
			livox_sector_set_voxel (sec,
				x - sx * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
				y - sy * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
				z - sz * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE), voxels[i]);
		}
	}
}

void
livox_manager_remove_material (livoxManager* self,
                               int           id)
{
	livoxMaterial* material;

	material = lialg_u32dic_find (self->materials, id);
	if (material != NULL)
	{
		lialg_u32dic_remove (self->materials, id);
		livox_material_free (material);
	}
}

int
livox_manager_replace_voxel (livoxManager*      self,
                             const limatVector* point,
                             const livoxVoxel*  terrain)
{
	float d;
	lialgRange range;
	limatVector diff;
	limatVector origin;
	livoxVoxel voxel;
	livoxVoxelIter iter;
	struct
	{
		int x;
		int y;
		int z;
		float dist;
		livoxSector* sector;
	}
	best = { 0, 0, 0, 10.0E10f, NULL };

	/* Loop through affected sectors. */
	range = lialg_range_new_from_sphere (point, LIVOX_TILE_WIDTH, LIVOX_TILE_WIDTH);
	LIVOX_VOXEL_FOREACH (iter, self, range, 1)
	{
		livox_sector_get_origin (iter.sector, &origin);
		voxel = *livox_sector_get_voxel (iter.sector, iter.voxel[0], iter.voxel[1], iter.voxel[2]);
		if (voxel.type)
		{
			diff = limat_vector_subtract (*point, limat_vector_init (
				origin.x + LIVOX_TILE_WIDTH * (iter.voxel[0] + 0.5f),
				origin.y + LIVOX_TILE_WIDTH * (iter.voxel[1] + 0.5f),
				origin.z + LIVOX_TILE_WIDTH * (iter.voxel[2] + 0.5f)));
			d = limat_vector_dot (diff, diff);
			if (best.sector == NULL || d < best.dist)
			{
				best.x = iter.voxel[0];
				best.y = iter.voxel[1];
				best.z = iter.voxel[2];
				best.dist = d;
				best.sector = iter.sector;
			}
		}
	}

	/* Replace the material of the best match. */
	if (best.sector != NULL)
		return livox_sector_set_voxel (best.sector, best.x, best.y, best.z, *terrain);

	return 0;
}

int
livox_manager_rotate_voxel (livoxManager*      self,
                            const limatVector* point,
                            int                axis,
                            int                step)
{
	float d;
	lialgRange range;
	limatVector diff;
	limatVector origin;
	livoxVoxel voxel;
	livoxVoxelIter iter;
	struct
	{
		int x;
		int y;
		int z;
		float dist;
		livoxSector* sector;
	}
	best = { 0, 0, 0, 10.0E10f, NULL };

	/* Loop through affected sectors. */
	range = lialg_range_new_from_sphere (point, LIVOX_TILE_WIDTH, LIVOX_TILE_WIDTH);
	LIVOX_VOXEL_FOREACH (iter, self, range, 1)
	{
		livox_sector_get_origin (iter.sector, &origin);
		voxel = *livox_sector_get_voxel (iter.sector, iter.voxel[0], iter.voxel[1], iter.voxel[2]);
		if (voxel.type)
		{
			diff = limat_vector_subtract (*point, limat_vector_init (
				origin.x + LIVOX_TILE_WIDTH * (iter.voxel[0] + 0.5f),
				origin.y + LIVOX_TILE_WIDTH * (iter.voxel[1] + 0.5f),
				origin.z + LIVOX_TILE_WIDTH * (iter.voxel[2] + 0.5f)));
			d = limat_vector_dot (diff, diff);
			if (best.sector == NULL || d < best.dist)
			{
				best.x = iter.voxel[0];
				best.y = iter.voxel[1];
				best.z = iter.voxel[2];
				best.dist = d;
				best.sector = iter.sector;
			}
		}
	}

	/* Replace the material of the best match. */
	if (best.sector != NULL)
	{
		voxel = *livox_sector_get_voxel (best.sector, best.x, best.y, best.z);
		livox_voxel_rotate (&voxel, axis, step);
		return livox_sector_set_voxel (best.sector, best.x, best.y, best.z, voxel);
	}

	return 0;
}

void
livox_manager_update (livoxManager* self,
                      float         secs)
{
	livox_manager_mark_updates (self);
	livox_manager_update_marked (self);
}

void
livox_manager_update_marked (livoxManager* self)
{
	int i;
	int x;
	int y;
	int z;
	lialgU32dicIter iter;
	livoxSector* sector;

	/* Rebuild modified terrain. */
	LI_FOREACH_U32DIC (iter, self->sectors)
	{
		sector = iter.value;
		if (!sector->dirty)
			continue;
		for (i = z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
		for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
		for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++, i++)
		{
			if (!sector->blocks[i].dirty)
				continue;
			livox_sector_build_block (sector, x, y, z);
			sector->blocks[i].dirty = 0;
		}
		sector->dirty = 0;
	}
}

/**
 * \brief Writes all the sectors to the database.
 *
 * \param self Voxel manager.
 * \return Nonzero on success.
 */
int
livox_manager_write (livoxManager* self)
{
	lialgU32dicIter iter;

	if (self->sql == NULL)
	{
		lisys_error_set (EINVAL, "no database");
		return 0;
	}

	/* Save terrain. */
	LI_FOREACH_U32DIC (iter, self->sectors)
	{
		if (!livox_sector_write (iter.value, self->sql))
			return 0;
	}

	return livox_manager_write_materials (self);
}

/**
 * \brief Writes the materials to the database.
 *
 * \param self Voxel manager.
 * \return Nonzero on success.
 */
int
livox_manager_write_materials (livoxManager* self)
{
	lialgU32dicIter iter;

	if (self->sql == NULL)
	{
		lisys_error_set (EINVAL, "no database");
		return 0;
	}

	/* Remove old materials. */
	if (!liarc_sql_delete (self->sql, "voxel_materials"))
		return 0;

	/* Save materials. */
	LI_FOREACH_U32DIC (iter, self->materials)
	{
		if (!livox_material_write_to_sql (iter.value, self->sql))
			return 0;
	}

	return 1;
}

void
livox_manager_set_sql (livoxManager* self,
                       liarcSql*     sql)
{
	self->sql = sql;
	if (sql != NULL)
	{
		private_ensure_materials (self);
		private_ensure_sectors (self);
	}
}

void
livox_manager_get_voxel (livoxManager* self,
                         int           x,
                         int           y,
                         int           z,
                         livoxVoxel*   value)
{
	int sx;
	int sy;
	int sz;
	livoxSector* sector;

	sx = x / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	sy = y / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	sz = z / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	sector = livox_manager_load_sector (self, LIVOX_SECTOR_INDEX (sx, sy, sz));
	if (sector == NULL)
	{
		livox_voxel_init (value, 0);
		return;
	}
	sx = x % (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	sy = y % (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	sz = z % (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	*value = *livox_sector_get_voxel (sector, sx, sy, sz);
}

int
livox_manager_set_voxel (livoxManager*     self,
                         int               x,
                         int               y,
                         int               z,
                         const livoxVoxel* value)
{
	int sx;
	int sy;
	int sz;
	livoxSector* sector;

	sx = x / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	sy = y / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	sz = z / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	sector = livox_manager_load_sector (self, LIVOX_SECTOR_INDEX (sx, sy, sz));
	if (sector == NULL)
		return 0;
	sx = x % (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	sy = y % (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	sz = z % (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);

	return livox_sector_set_voxel (sector, sx, sy, sz, *value);
}

/*****************************************************************************/

static void
private_clear_materials (livoxManager* self)
{
	lialgU32dicIter iter;
	livoxMaterial* material;

	LI_FOREACH_U32DIC (iter, self->materials)
	{
		material = iter.value;
		livox_material_free (material);
	}
	lialg_u32dic_clear (self->materials);
}

static void
private_clear_sectors (livoxManager* self)
{
	lialgU32dicIter iter;
	livoxSector* sector;

	LI_FOREACH_U32DIC (iter, self->sectors)
	{
		sector = iter.value;
		livox_sector_free (sector);
	}
	lialg_u32dic_clear (self->sectors);
}

static int
private_ensure_materials (livoxManager* self)
{
	const char* query;
	sqlite3_stmt* statement;

	/* Create material table. */
	query = "CREATE TABLE IF NOT EXISTS voxel_materials "
		"(id INTEGER PRIMARY KEY,flags UNSIGNED INTEGER,"
		"fric REAL,name TEXT,model TEXT);";
	if (sqlite3_prepare_v2 (self->sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self->sql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self->sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return 1;
}

static int
private_ensure_sectors (livoxManager* self)
{
	const char* query;
	sqlite3_stmt* statement;

	/* Create sector table. */
	query = "CREATE TABLE IF NOT EXISTS voxel_sectors "
		"(id INTEGER PRIMARY KEY,data BLOB);";
	if (sqlite3_prepare_v2 (self->sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self->sql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self->sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return 1;
}

static void
private_mark_block (livoxManager* self,
                    livoxSector*  sector,
                    int           x,
                    int           y,
                    int           z)
{
	int sx;
	int sy;
	int sz;
	uint32_t id;
	livoxSector* sector1;

	/* Find affected sector. */
	sx = sector->x;
	sy = sector->y;
	sz = sector->z;
	if (x < 0)
	{
		x = LIVOX_BLOCKS_PER_LINE - 1;
		sx--;
	}
	else if (x >= LIVOX_BLOCKS_PER_LINE)
	{
		x = 0;
		sx++;
	}
	if (y < 0)
	{
		y = LIVOX_BLOCKS_PER_LINE - 1;
		sy--;
	}
	else if (y >= LIVOX_BLOCKS_PER_LINE)
	{
		y = 0;
		sy++;
	}
	if (z < 0)
	{
		z = LIVOX_BLOCKS_PER_LINE - 1;
		sz--;
	}
	else if (z >= LIVOX_BLOCKS_PER_LINE)
	{
		z = 0;
		sz++;
	}
	id = LIVOX_SECTOR_INDEX (sx, sy, sz);

	/* Mark block as dirty. */
	sector1 = livox_manager_find_sector (self, id);
	if (sector1 == NULL)
		return;
	sector1->blocks[LIVOX_BLOCK_INDEX (x, y, z)].dirty |= 0x80;
	sector1->dirty = 1;
}

/** @} */
/** @} */
