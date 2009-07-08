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

#include "voxel-manager.h"
#include "voxel-material.h"
#include "voxel-private.h"

static void
private_clear_materials (livoxManager* self);

static void
private_clear_sectors (livoxManager* self);

static void
private_mark_block (livoxManager* self,
                    livoxSector*  sector,
                    int           x,
                    int           y,
                    int           z);

/*****************************************************************************/

#ifndef LIVOX_DISABLE_GRAPHICS
livoxManager*
livox_manager_new (liphyPhysics* physics,
                   lirndScene*   scene,
                   lirndApi*     rndapi)
{
	livoxManager* self;

	self = calloc (1, sizeof (livoxManager));
	if (self == NULL)
		return NULL;
	self->physics = physics;
	self->scene = scene;
	if (scene != NULL)
		self->render = scene->render;
	self->renderapi = rndapi;
	self->materials = lialg_u32dic_new ();
	if (self->materials == NULL)
	{
		free (self);
		return NULL;
	}
	self->sectors = lialg_u32dic_new ();
	if (self->sectors == NULL)
	{
		lialg_u32dic_free (self->materials);
		free (self);
		return NULL;
	}

	return self;
}
#else
livoxManager*
livox_manager_new (liphyPhysics* physics)
{
	livoxManager* self;

	self = calloc (1, sizeof (livoxManager));
	if (self == NULL)
		return NULL;
	self->physics = physics;
	self->materials = lialg_u32dic_new ();
	if (self->materials == NULL)
	{
		free (self);
		return NULL;
	}
	self->sectors = lialg_u32dic_new ();
	if (self->sectors == NULL)
	{
		lialg_u32dic_free (self->materials);
		free (self);
		return NULL;
	}

	return self;
}
#endif

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
	free (self);
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
 * \brief Creates an empty sector.
 *
 * If a sector with the same number already exists, it is returned instead of
 * creating a new one.
 *
 * \brief Voxel manager.
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

/**
 * \brief Loads materials from an SQL database.
 *
 * \brief Voxel manager.
 * \param sql Database.
 * \return Nonzero on success.
 */
int
livox_manager_load_materials (livoxManager* self,
                              liarcSql*     sql)
{
	int ret;
	const char* query;
	livoxMaterial* material;
	sqlite3_stmt* statement;

	/* Prepare statement. */
	query = "SELECT id,flags,fric,scal,shi,dif0,dif1,dif2,dif3,spe0,spe1,"
		"spe2,spe3,name,shdr FROM voxel_materials;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}

	/* Read materials. */
	for (ret = sqlite3_step (statement) ; ret != SQLITE_DONE ; ret = sqlite3_step (statement))
	{
		/* Check for errors. */
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Read material. */
		material = livox_material_new (sql, statement);
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
 * \brief Loads a sector from an SQL database.
 *
 * If a sector with the same number already exists, no loading takes place.
 * If a sector with the requested ID cannot be found from the database,
 * an empty sector is created.
 *
 * \brief Voxel manager.
 * \param id Sector index.
 * \param sql Database.
 * \return Sector owned by the manager or NULL.
 */
livoxSector*
livox_manager_load_sector (livoxManager* self,
                           uint32_t      id,
                           liarcSql*     sql)
{
	livoxSector* sector;

	sector = livox_manager_find_sector (self, id);
	if (sector != NULL)
		return sector;
	sector = livox_sector_new (self, id);
	if (sector == NULL)
		return NULL;
	livox_sector_read (sector, sql);

	return sector;
}

void
livox_manager_update (livoxManager* self,
                      float         secs)
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
 * \param sql Database.
 * \return Nonzero on success.
 */
int
livox_manager_write (livoxManager* self,
                     liarcSql*     sql)
{
	const char* query;
	sqlite3_stmt* statement;
	lialgU32dicIter iter;

	/* Create material table. */
	query = "CREATE TABLE IF NOT EXISTS voxel_materials "
		"(id INTEGER PRIMARY KEY,flags UNSIGNED INTEGER,"
		"fric REAL,scal REAL,shi REAL,"
		"dif0 REAL,dif1 REAL,dif2 REAL,dif3 REAL,"
		"spe0 REAL,spe1 REAL,spe2 REAL,spe3 REAL,name TEXT,shdr TEXT);";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Create texture table. */
	query = "CREATE TABLE IF NOT EXISTS voxel_textures "
		"(mat INTEGER REFERENCES voxel_materials(id),"
		"unit UNSIGNED INTEGER,flags UNSIGNED INTEGER,name TEXT);";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Create sector table. */
	query = "CREATE TABLE IF NOT EXISTS voxel_sectors "
		"(id INTEGER PRIMARY KEY,data BLOB);";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Save materials. */
/*	LI_FOREACH_ASSOCID (iter, self->materials)
	{
		if (!liext_material_write (iter.value, sql))
			return 0;
	}*/

	/* Save terrain. */
	LI_FOREACH_U32DIC (iter, self->sectors)
	{
		if (!livox_sector_write (iter.value, sql))
			return 0;
	}

	return 1;
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
