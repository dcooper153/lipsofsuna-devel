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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup LIVoxManager Manager
 * @{
 */

#include <lipsofsuna/algorithm.h>
#include "voxel-iterator.h"
#include "voxel-manager.h"
#include "voxel-material.h"
#include "voxel-private.h"

#define VOXEL_BORDER_TOLERANCE 0.05f

static void
private_clear_materials (LIVoxManager* self);

static void
private_mark_block (LIVoxManager* self,
                    LIVoxSector*  sector,
                    int           x,
                    int           y,
                    int           z);

/*****************************************************************************/

LIVoxManager*
livox_manager_new (LICalCallbacks* callbacks,
                   LIAlgSectors*   sectors)
{
	LIVoxManager* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIVoxManager));
	if (self == NULL)
		return NULL;
	self->callbacks = callbacks;
	self->sectors = sectors;
	livox_manager_configure (self, 4, 16);

	/* Allocate materials. */
	self->materials = lialg_u32dic_new ();
	if (self->materials == NULL)
	{
		livox_manager_free (self);
		return NULL;
	}

	/* Allocate sector data. */
	if (!lialg_sectors_insert_content (self->sectors, "voxel", self,
	     	(LIAlgSectorFreeFunc) livox_sector_free,
	     	(LIAlgSectorLoadFunc) livox_sector_new))
	{
		livox_manager_free (self);
		return NULL;
	}

	return self;
}

void
livox_manager_free (LIVoxManager* self)
{
	/* Free materials. */
	if (self->materials != NULL)
	{
		private_clear_materials (self);
		lialg_u32dic_free (self->materials);
	}

	/* Free sector data. */
	if (self->sectors != NULL)
		lialg_sectors_remove_content (self->sectors, "voxel");

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
livox_manager_check_occluder (const LIVoxManager* self,
                              const LIVoxVoxel*   voxel)
{
	LIVoxMaterial* material;

	if (!voxel->type)
		return 0;
	material = livox_manager_find_material ((LIVoxManager*) self, voxel->type);
	if (material == NULL)
		return 0;

	if (!(material->flags & LIVOX_MATERIAL_FLAG_OCCLUDER))
		return 0;
	if (material->type == LIVOX_MATERIAL_TYPE_HEIGHT)
	{
#warning Occlusion probably does not work correctly for heightmap tiles.
		if (voxel->damage)
			return 0;
	}

	return 1;
}

/**
 * \brief Removes all the materials.
 *
 * \param self Voxel manager.
 */
void
livox_manager_clear_materials (LIVoxManager* self)
{
	private_clear_materials (self);
}

/**
 * \brief Reconfigures the block and tile counts of the map.
 *
 * \param self Voxel manager.
 * \param blocks_per_line Number of blocks per sector edge.
 * \param tiles_per_line Number of tiles per sector edge.
 * \return Nonzero on success.
 */
int livox_manager_configure (
	LIVoxManager* self,
	int           blocks_per_line,
	int           tiles_per_line)
{
	if (self->sectors->sectors->size)
	{
		lisys_error_set (EINVAL, "cannot change grid settings when the map is populated");
		return 0;
	}
	self->blocks_per_line = blocks_per_line;
	self->blocks_per_sector = self->blocks_per_line * self->blocks_per_line * self->blocks_per_line;
	self->tiles_per_line = tiles_per_line;
	self->tiles_per_sector = self->tiles_per_line * self->tiles_per_line * self->tiles_per_line;
	self->tile_width = self->sectors->width / self->tiles_per_line;

	return 1;
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
livox_manager_copy_voxels (LIVoxManager* self,
                           int           xstart,
                           int           ystart,
                           int           zstart,
                           int           xsize,
                           int           ysize,
                           int           zsize,
                           LIVoxVoxel*   result)
{
	int i;
	int x;
	int y;
	int z;
	int sx;
	int sy;
	int sz;
	int tx;
	int ty;
	int tz;
	LIVoxSector* sec;

	/* FIXME: Avoid excessive sector lookups. */
	for (i = 0, z = zstart ; z < zstart + zsize ; z++)
	for (y = ystart ; y < ystart + ysize ; y++)
	for (x = xstart ; x < xstart + xsize ; x++, i++)
	{
		sx = x / self->tiles_per_line;
		sy = y / self->tiles_per_line;
		sz = z / self->tiles_per_line;
		tx = x - sx * self->tiles_per_line;
		ty = y - sy * self->tiles_per_line;
		tz = z - sz * self->tiles_per_line;
		if (sx < 0 || sx >= self->sectors->count || tx < 0 ||
		    sy < 0 || sy >= self->sectors->count || ty < 0 ||
		    sz < 0 || sz >= self->sectors->count || tz < 0)
		{
			livox_voxel_init (result + i, 1);
			continue;
		}
		sec = lialg_sectors_data_offset (self->sectors, "voxel", sx, sy, sz, 0);
		if (sec != NULL)
			result[i] = *livox_sector_get_voxel (sec, tx, ty, tz);
		else
			livox_voxel_init (result + i, 1);
	}
}

LIVoxMaterial*
livox_manager_find_material (LIVoxManager* self,
                             uint32_t      id)
{
	return lialg_u32dic_find (self->materials, id);
}

/**
 * \brief Finds the nearest voxel to the given point.
 * \param self Voxel manager.
 * \param flags Search flags.
 * \param point Point in world space.
 * \param index Return location for voxel position or NULL.
 * \return Voxel or NULL.
 */
LIVoxVoxel* livox_manager_find_voxel (
	LIVoxManager*      self,
	int                flags,
	const LIMatVector* point,
	int*               index)
{
	float d;
	LIAlgRange range;
	LIMatVector tmp;
	LIMatVector diff;
	LIMatVector origin;
	LIVoxVoxel voxel;
	LIVoxVoxelIter iter;
	struct
	{
		int x;
		int y;
		int z;
		float dist;
		LIVoxSector* sector;
	}
	best = { 0, 0, 0, 10.0E10f, NULL };

	/* Loop through affected sectors. */
	range = lialg_range_new_from_sphere (point, self->tile_width, self->tile_width);
	LIVOX_VOXEL_FOREACH (iter, self, range, 1)
	{
		livox_sector_get_origin (iter.sector, &origin);
		voxel = *livox_sector_get_voxel (iter.sector, iter.voxel[0], iter.voxel[1], iter.voxel[2]);
		if ((!voxel.type && (flags & LIVOX_FIND_EMPTY)) ||
			( voxel.type && (flags & LIVOX_FIND_FULL)))
		{
			tmp = limat_vector_init (
				origin.x + self->tile_width * (iter.voxel[0] + 0.5f),
				origin.y + self->tile_width * (iter.voxel[1] + 0.5f),
				origin.z + self->tile_width * (iter.voxel[2] + 0.5f));
			diff = limat_vector_subtract (*point, tmp);
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

	if (best.sector == NULL)
		return NULL;
	if (index != NULL)
	{
		index[0] = best.x + best.sector->sector->x * self->tiles_per_line;
		index[1] = best.y + best.sector->sector->y * self->tiles_per_line;
		index[2] = best.z + best.sector->sector->z * self->tiles_per_line;
	}

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
livox_manager_insert_material (LIVoxManager*  self,
                               LIVoxMaterial* material)
{
	LIVoxMaterial* tmp;

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

void
livox_manager_mark_updates (LIVoxManager* self)
{
	int i;
	int j;
	int x;
	int y;
	int z;
	LIAlgSectorsIter iter;
	LIVoxSector* sector;
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
	LIALG_SECTORS_FOREACH (iter, self->sectors)
	{
		sector = lialg_strdic_find (iter.sector->content, "voxel");
		if (sector == NULL || !sector->dirty)
			continue;
		for (i = z = 0 ; z < self->blocks_per_line ; z++)
		for (y = 0 ; y < self->blocks_per_line ; y++)
		for (x = 0 ; x < self->blocks_per_line ; x++, i++)
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
 * \return Nonzero on success.
 */
int livox_manager_paste_voxels (
	LIVoxManager* self,
	int           xstart,
	int           ystart,
	int           zstart,
	int           xsize,
	int           ysize,
	int           zsize,
	LIVoxVoxel*   voxels)
{
	int i;
	int min[3];
	int max[3];
	int off[3];
	int sec[3];
	int src[3];
	int dst[3];
	LIVoxSector* sector;

	/* Determine affected sectors. */
	min[0] = xstart / self->tiles_per_line;
	min[1] = ystart / self->tiles_per_line;
	min[2] = zstart / self->tiles_per_line;
	max[0] = (xstart + xsize - 1) / self->tiles_per_line;
	max[1] = (ystart + ysize - 1) / self->tiles_per_line;
	max[2] = (zstart + zsize - 1) / self->tiles_per_line;

	/* Loop through affected sectors. */
	for (sec[2] = min[2] ; sec[2] <= max[2] ; sec[2]++)
	for (sec[1] = min[1] ; sec[1] <= max[1] ; sec[1]++)
	for (sec[0] = min[0] ; sec[0] <= max[0] ; sec[0]++)
	{
		/* Find or create sector. */
		sector = lialg_sectors_data_offset (self->sectors, "voxel", sec[0], sec[1], sec[2], 1);
		if (sector == NULL)
			return 0;

		/* Calculate paint offset. */
		off[0] = xstart - sec[0] * self->tiles_per_line;
		off[1] = ystart - sec[1] * self->tiles_per_line;
		off[2] = zstart - sec[2] * self->tiles_per_line;

		/* Copy brush voxels to sector. */
		for (src[2] = 0, dst[2] = off[2], i = 0 ; src[2] < zsize ; src[2]++, dst[2]++)
		for (src[1] = 0, dst[1] = off[1] ; src[1] < ysize ; src[1]++, dst[1]++)
		for (src[0] = 0, dst[0] = off[0] ; src[0] < xsize ; src[0]++, dst[0]++, i++)
		{
			if (0 <= dst[0] && dst[0] < self->tiles_per_line &&
				0 <= dst[1] && dst[1] < self->tiles_per_line &&
				0 <= dst[2] && dst[2] < self->tiles_per_line)
				livox_sector_set_voxel (sector, dst[0], dst[1], dst[2], voxels[i]);
		}
	}

	return 1;
}

void
livox_manager_remove_material (LIVoxManager* self,
                               int           id)
{
	LIVoxMaterial* material;

	material = lialg_u32dic_find (self->materials, id);
	if (material != NULL)
	{
		lialg_u32dic_remove (self->materials, id);
		livox_material_free (material);
	}
}

void
livox_manager_update (LIVoxManager* self,
                      float         secs)
{
	livox_manager_mark_updates (self);
	livox_manager_update_marked (self);
}

void
livox_manager_update_marked (LIVoxManager* self)
{
	int i;
	int x;
	int y;
	int z;
	LIAlgSectorsIter iter;
	LIVoxSector* sector;

	/* Rebuild modified terrain. */
	LIALG_SECTORS_FOREACH (iter, self->sectors)
	{
		sector = lialg_strdic_find (iter.sector->content, "voxel");
		if (sector == NULL || !sector->dirty)
			continue;
		for (i = z = 0 ; z < self->blocks_per_line ; z++)
		for (y = 0 ; y < self->blocks_per_line ; y++)
		for (x = 0 ; x < self->blocks_per_line ; x++, i++)
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
 * \brief Sets the default fill tile type for empty sectors.
 *
 * \param self Voxel manager.
 * \param type Terrain type, zero for empty.
 */
void
livox_manager_set_fill (LIVoxManager* self,
                        int           type)
{
	self->fill = type;
}

void
livox_manager_get_voxel (LIVoxManager* self,
                         int           x,
                         int           y,
                         int           z,
                         LIVoxVoxel*   value)
{
	int sx;
	int sy;
	int sz;
	LIVoxSector* sector;

	sx = x / self->tiles_per_line;
	sy = y / self->tiles_per_line;
	sz = z / self->tiles_per_line;
	sector = lialg_sectors_data_offset (self->sectors, "voxel", sx, sy, sz, 1);
	if (sector == NULL)
	{
		livox_voxel_init (value, 0);
		return;
	}
	sx = x % self->tiles_per_line;
	sy = y % self->tiles_per_line;
	sz = z % self->tiles_per_line;
	*value = *livox_sector_get_voxel (sector, sx, sy, sz);
}

int
livox_manager_set_voxel (LIVoxManager*     self,
                         int               x,
                         int               y,
                         int               z,
                         const LIVoxVoxel* value)
{
	int sx;
	int sy;
	int sz;
	LIVoxSector* sector;

	sx = x / self->tiles_per_line;
	sy = y / self->tiles_per_line;
	sz = z / self->tiles_per_line;
	sector = lialg_sectors_data_offset (self->sectors, "voxel", sx, sy, sz, 1);
	if (sector == NULL)
		return 0;
	sx = x % self->tiles_per_line;
	sy = y % self->tiles_per_line;
	sz = z % self->tiles_per_line;

	return livox_sector_set_voxel (sector, sx, sy, sz, *value);
}

/*****************************************************************************/

static void
private_clear_materials (LIVoxManager* self)
{
	LIAlgU32dicIter iter;
	LIVoxMaterial* material;

	LIALG_U32DIC_FOREACH (iter, self->materials)
	{
		material = iter.value;
		livox_material_free (material);
	}
	lialg_u32dic_clear (self->materials);
}

static void
private_mark_block (LIVoxManager* self,
                    LIVoxSector*  sector,
                    int           x,
                    int           y,
                    int           z)
{
	int sx;
	int sy;
	int sz;
	LIVoxBlock* block;
	LIVoxSector* sector1;

	/* Find affected sector. */
	sx = sector->sector->x;
	sy = sector->sector->y;
	sz = sector->sector->z;
	if (x < 0)
	{
		x = self->blocks_per_line - 1;
		sx--;
	}
	else if (x >= self->blocks_per_line)
	{
		x = 0;
		sx++;
	}
	if (y < 0)
	{
		y = self->blocks_per_line - 1;
		sy--;
	}
	else if (y >= self->blocks_per_line)
	{
		y = 0;
		sy++;
	}
	if (z < 0)
	{
		z = self->blocks_per_line - 1;
		sz--;
	}
	else if (z >= self->blocks_per_line)
	{
		z = 0;
		sz++;
	}

	/* Mark block as dirty. */
	sector1 = lialg_sectors_data_offset (self->sectors, "voxel", sx, sy, sz, 0);
	if (sector1 == NULL)
		return;
	block = livox_sector_get_block (sector1, x, y, z);
	block->dirty |= 0x80;
	sector1->dirty = 1;
}

/** @} */
/** @} */
