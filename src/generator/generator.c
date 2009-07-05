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
 * \addtogroup ligen Generator
 * @{
 * \addtogroup ligenGenerator Generator
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include "generator.h"

static int
private_init_brushes (ligenGenerator* self);

static int
private_init_sql (ligenGenerator* self);

static int
private_brush_exists (ligenGenerator*  self,
                      ligenStroke*     stroke,
                      ligenRulestroke* rstroke);

static int
private_brush_intersects (ligenGenerator*  self,
                          ligenStroke*     stroke,
                          ligenRulestroke* rstroke);

static int
private_rule_apply (ligenGenerator* self,
                    ligenStroke*    stroke,
                    ligenRule*      rule);

static int
private_rule_test (ligenGenerator* self,
                   ligenStroke*    stroke,
                   ligenRule*      rule);

static int
private_stroke_paint (ligenGenerator* self,
                      ligenStroke*    stroke);

/*****************************************************************************/

/**
 * \brief Creates a new generator module.
 *
 * \param name Module name.
 * \return New generator or NULL.
 */
ligenGenerator*
ligen_generator_new (const char* name)
{
	return ligen_generator_new_full (name, NULL, NULL);
}

/**
 * \brief Creates a new generator module.
 *
 * \param name Module name.
 * \param scene Render scene or NULL.
 * \param rndapi Render API or NULL.
 * \return New generator or NULL.
 */
ligenGenerator*
ligen_generator_new_full (const char* name,
                          lirndScene* scene,
                          lirndApi*   rndapi)
{
	ligenGenerator* self;

	/* Allocate self. */
	self = calloc (1, sizeof (ligenGenerator));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		lisys_error_report ();
		return NULL;
	}

	/* Allocate paths. */
	self->paths = lipth_paths_new (name);
	if (self->paths == NULL)
		goto error;

	/* Allocate terrain structures. */
	self->physics = liphy_physics_new ();
	if (self->physics == NULL)
		goto error;
	self->voxels = livox_manager_new (self->physics, scene, rndapi);
	if (self->voxels == NULL)
		goto error;

	/* Load databases. */
	if (!private_init_sql (self) ||
	    !private_init_brushes (self))
		goto error;

	return self;

error:
	ligen_generator_free (self);
	return NULL;
}

/**
 * \brief Frees the generator.
 *
 * \param self Generator.
 */
void
ligen_generator_free (ligenGenerator* self)
{
	int i;

	for (i = 0 ; i < self->brushes.count ; i++)
		ligen_brush_free (self->brushes.array[i]);
	if (self->voxels != NULL)
		livox_manager_free (self->voxels);
	if (self->physics != NULL)
		liphy_physics_free (self->physics);
	if (self->gensql != NULL)
		sqlite3_close (self->gensql);
	if (self->gensql != NULL)
		sqlite3_close (self->srvsql);
	if (self->paths != NULL)
		lipth_paths_free (self->paths);
	free (self->brushes.array);
	free (self->world.array);
	free (self);
}

/**
 * \brief Removes all the created map paint operations.
 *
 * \param self Generator.
 */
void
ligen_generator_clear_scene (ligenGenerator* self)
{
	/* Free strokes. */
	free (self->world.array);
	self->world.array = NULL;
	self->world.count = 0;

	/* Clear scene. */
	livox_manager_clear (self->voxels);
	livox_manager_update (self->voxels, 1.0f);
}

/**
 * \brief Inserts a brush to the generator.
 *
 * The ownership of the brush is transferred to the generator if successful.
 *
 * \param self Generator.
 * \param brush Brush.
 * \return Nonzero on success.
 */
int
ligen_generator_insert_brush (ligenGenerator* self,
                              ligenBrush*     brush)
{
	if (!lialg_array_append (&self->brushes, &brush))
		return 0;
	brush->id = self->brushes.count - 1;

	return 1;
}

/**
 * \brief Inserts a stroke to the generator.
 *
 * \param self Generator.
 * \param brush Brush number.
 * \param x Insertion position.
 * \param y Insertion position.
 * \param z Insertion position.
 * \return Nonzero on success.
 */
int
ligen_generator_insert_stroke (ligenGenerator* self,
                               int             brush,
                               int             x,
                               int             y,
                               int             z)
{
	ligenBrush* brush_;
	ligenStroke stroke;

	assert (brush >= 0);
	assert (brush < self->brushes.count);

	brush_ = self->brushes.array[brush];
	stroke.pos[0] = x;
	stroke.pos[1] = y;
	stroke.pos[2] = z;
	stroke.size[0] = brush_->size[0];
	stroke.size[1] = brush_->size[1];
	stroke.size[2] = brush_->size[2];
	stroke.brush = brush;
	if (!lialg_array_append (&self->world, &stroke))
		return 0;

	return 1;
}

/**
 * \brief Enters the main loop of the generator.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
ligen_generator_main (ligenGenerator* self)
{
	int i;

	/* Generate areas. */
	/* FIXME */
	for (i = 0 ; i < 20 ; i++)
	{
		if (!ligen_generator_step (self))
			break;
	}

	/* Generate geometry. */
	ligen_generator_rebuild_scene (self);

	/* Save geometry. */
	livox_manager_write (self->voxels, self->srvsql);

	return 1;
}

/**
 * \brief Rebuilds all the terrain.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
ligen_generator_rebuild_scene (ligenGenerator* self)
{
	int i;
	ligenStroke* stroke;

	/* Clear sectors. */
	livox_manager_clear (self->voxels);

	/* Paint strokes. */
	for (i = 0 ; i < self->world.count ; i++)
	{
		stroke = self->world.array + i;
		private_stroke_paint (self, stroke);
	}

	/* Rebuild geometry. */
	if (self->voxels->scene != NULL)
		livox_manager_update (self->voxels, 1.0f);

	return 1;
}

/**
 * \brief Extends the map by one rule.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
ligen_generator_step (ligenGenerator* self)
{
	int i;
	int j;
	ligenBrush* brush;
	ligenStroke* stroke;
	ligenRule* rule;

	for (i = 0 ; i < self->world.count ; i++)
	{
		stroke = self->world.array + i;
		brush = self->brushes.array[stroke->brush];
		for (j = 0 ; j < brush->rules.count ; j++)
		{
			rule = brush->rules.array[j];
			if (private_rule_test (self, stroke, rule))
			{
				if (!private_rule_apply (self, stroke, rule))
					return 0;
				return 1;
			}
		}
	}

	return 0;
}

/**
 * \brief Saves brushes to the generator database.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
ligen_generator_write_brushes (ligenGenerator* self)
{
	int i;
	const char* query;
	sqlite3_stmt* statement;

	/* Create brush table. */
	query = "CREATE TABLE IF NOT EXISTS generator_brushes "
		"(id INTEGER PRIMARY KEY,name TEXT,sizx UNSIGNED INTEGER,"
		"sizy UNSIGNED INTEGER,sizz UNSIGNED INTEGER,voxels BLOB);";
	if (sqlite3_prepare_v2 (self->gensql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self->gensql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self->gensql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Create rule table. */
	query = "CREATE TABLE IF NOT EXISTS generator_rules "
		"(id INTEGER,"
		"brush INTEGER REFERENCES generator_brushes(id),"
		"name TEXT,flags UNSIGNED INTEGER);";
	if (sqlite3_prepare_v2 (self->gensql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self->gensql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self->gensql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Create stroke table. */
	query = "CREATE TABLE IF NOT EXISTS generator_strokes "
		"(id INTEGER,"
		"brush INTEGER REFERENCES generator_brushes(id),"
		"rule INTEGER,"
		"paint INTEGER REFERENCES generator_brushes(id),"
		"x UNSIGNED INTEGER,y UNSIGNED INTEGER,z UNSIGNED INTEGER,"
		"flags UNSIGNED INTEGER);";
	if (sqlite3_prepare_v2 (self->gensql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self->gensql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self->gensql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Save brushes. */
	for (i = 0 ; i < self->brushes.count ; i++)
		ligen_brush_write (self->brushes.array[i], self->gensql);

	return 1;
}

/*****************************************************************************/

static int
private_init_brushes (ligenGenerator* self)
{
	int id;
	int col;
	int ret;
	int size0;
	int size1;
	int size[3];
	char* name;
	const char* query;
	const void* bytes;
	liarcSql* sql;
	ligenBrush* brush;
	sqlite3_stmt* statement;

	sql = self->gensql;

	/* Prepare statement. */
	query = "SELECT id,sizx,sizy,sizz,name,voxels FROM generator_brushes;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}

	/* Read brushes. */
	for (ret = sqlite3_step (statement) ; ret != SQLITE_DONE ; ret = sqlite3_step (statement))
	{
		/* Check for errors. */
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Read id and size. */
		col = 0;
		id = sqlite3_column_int (statement, col++);
		size[0] = sqlite3_column_int (statement, col++);
		size[1] = sqlite3_column_int (statement, col++);
		size[2] = sqlite3_column_int (statement, col++);
		size[0] = LI_MAX (1, size[0]);
		size[1] = LI_MAX (1, size[1]);
		size[2] = LI_MAX (1, size[2]);

		/* Create new brush. */
		brush = ligen_brush_new (size[0], size[1], size[2]);
		if (brush == NULL)
		{
			sqlite3_finalize (statement);
			return 0;
		}
		if (!ligen_generator_insert_brush (self, brush))
		{
			ligen_brush_free (brush);
			sqlite3_finalize (statement);
			return 0;
		}

		/* Read name column. */
		name = (char*) sqlite3_column_text (statement, col);
		size0 = sqlite3_column_bytes (statement, col++);
		if (size0 > 0 && name != NULL)
		{
			free (brush->name);
			brush->name = strdup (name);
			if (brush->name == NULL)
			{
				lisys_error_set (ENOMEM, NULL);
				sqlite3_finalize (statement);
				return 0;
			}
		}

		/* Read voxels column. */
		bytes = sqlite3_column_blob (statement, col);
		size0 = sqlite3_column_bytes (statement, col);
		size1 = size[0] * size[1] * size[2] * sizeof (livoxVoxel);
		if (size0 == size1)
			memcpy (brush->voxels.array, bytes, size1);

		/* Read rules. */
		if (!ligen_brush_read_rules (brush, sql, id))
		{
			sqlite3_finalize (statement);
			return 0;
		}
	}
	sqlite3_finalize (statement);

	return 1;
}

static int
private_init_sql (ligenGenerator* self)
{
	int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	char* path;

	/* Format path. */
	path = lipth_paths_get_data (self->paths, "server.db");
	if (path == NULL)
		return 0;

	/* Open database. */
	if (sqlite3_open_v2 (path, &self->srvsql, flags, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "sqlite: %s", sqlite3_errmsg (self->srvsql));
		sqlite3_close (self->srvsql);
		free (path);
		return 0;
	}
	free (path);

	/* Format path. */
	path = lipth_paths_get_data (self->paths, "generator.db");
	if (path == NULL)
		return 0;

	/* Open database. */
	if (sqlite3_open_v2 (path, &self->gensql, flags, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "sqlite: %s", sqlite3_errmsg (self->gensql));
		sqlite3_close (self->gensql);
		free (path);
		return 0;
	}
	free (path);

	return 1;
}

static int
private_brush_exists (ligenGenerator*  self,
                      ligenStroke*     stroke,
                      ligenRulestroke* rstroke)
{
	int i;
	int pos[3];
	ligenBrush* brush;
	ligenStroke* stroke1;

	/* Calculate world position. */
	brush = self->brushes.array[rstroke->brush];
	pos[0] = stroke->pos[0] + rstroke->pos[0];
	pos[1] = stroke->pos[1] + rstroke->pos[1];
	pos[2] = stroke->pos[2] + rstroke->pos[2];

	/* Test against all strokes. */
	/* FIXME: Use space partitioning. */
	for (i = 0 ; i < self->world.count ; i++)
	{
		stroke1 = self->world.array + i;
		if (stroke1->brush == brush->id &&
		    stroke1->pos[0] == pos[0] &&
		    stroke1->pos[1] == pos[1] &&
		    stroke1->pos[2] == pos[2])
			return 1;
	}

	return 0;
}

static int
private_brush_intersects (ligenGenerator*  self,
                          ligenStroke*     stroke,
                          ligenRulestroke* rstroke)
{
	int i;
	int min0[3];
	int min1[3];
	int max0[3];
	int max1[3];
	ligenBrush* brush;
	ligenStroke* stroke1;

	/* Calculate world position. */
	brush = self->brushes.array[rstroke->brush];
	min0[0] = stroke->pos[0] + rstroke->pos[0];
	min0[1] = stroke->pos[1] + rstroke->pos[1];
	min0[2] = stroke->pos[2] + rstroke->pos[2];
	max0[0] = min0[0] + brush->size[0];
	max0[1] = min0[1] + brush->size[1];
	max0[2] = min0[2] + brush->size[2];

	/* Test against all strokes. */
	/* FIXME: Use space partitioning. */
	for (i = 0 ; i < self->world.count ; i++)
	{
		stroke1 = self->world.array + i;
		min1[0] = stroke1->pos[0];
		min1[1] = stroke1->pos[1];
		min1[2] = stroke1->pos[2];
		max1[0] = min1[0] + stroke1->size[0];
		max1[1] = min1[1] + stroke1->size[1];
		max1[2] = min1[2] + stroke1->size[2];
		if (max0[0] <= min1[0] || max1[0] <= min0[0] ||
		    max0[1] <= min1[1] || max1[1] <= min0[1] ||
		    max0[2] <= min1[2] || max1[2] <= min0[2])
			continue;
		return 1;
	}

	return 0;
}

static int
private_rule_apply (ligenGenerator* self,
                    ligenStroke*    stroke,
                    ligenRule*      rule)
{
	int i;
	int orig;
	ligenBrush* brush;
	ligenStroke stroke1;
	ligenRulestroke* rstroke;

	orig = self->world.count;
	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		rstroke = rule->strokes.array + i;
		brush = self->brushes.array[rstroke->brush];
		stroke1.pos[0] = stroke->pos[0] + rstroke->pos[0];
		stroke1.pos[1] = stroke->pos[1] + rstroke->pos[1];
		stroke1.pos[2] = stroke->pos[2] + rstroke->pos[2];
		stroke1.size[0] = brush->size[0];
		stroke1.size[1] = brush->size[1];
		stroke1.size[2] = brush->size[2];
		stroke1.brush = brush->id;
		if (!lialg_array_append (&self->world, &stroke1))
		{
			self->world.count = orig;
			return 0;
		}
	}

	return 1;
}

static int
private_rule_test (ligenGenerator* self,
                   ligenStroke*    stroke,
                   ligenRule*      rule)
{
	int i;
	ligenRulestroke* rstroke;

	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		rstroke = rule->strokes.array + i;
		if (rstroke->flags & LIGEN_RULE_REQUIRE)
		{
			if (!private_brush_exists (self, stroke, rstroke))
				return 0;
		}
		else
		{
			if (private_brush_intersects (self, stroke, rstroke))
				return 0;
		}
	}

	return 1;
}

static int
private_stroke_paint (ligenGenerator* self,
                      ligenStroke*    stroke)
{
	int i;
	int index;
	int min[3];
	int max[3];
	int off[3];
	int sec[3];
	int src[3];
	int dst[3];
	ligenBrush* brush;
	livoxSector* sector;

	/* Determine affected sectors. */
	brush = self->brushes.array[stroke->brush];
	min[0] = (stroke->pos[0]) / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	min[1] = (stroke->pos[1]) / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	min[2] = (stroke->pos[2]) / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	max[0] = (stroke->pos[0] + stroke->size[0]) / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	max[1] = (stroke->pos[1] + stroke->size[1]) / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
	max[2] = (stroke->pos[2] + stroke->size[2]) / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);

	/* Loop through affected sectors. */
	for (sec[2] = min[2] ; sec[2] <= max[2] ; sec[2]++)
	for (sec[1] = min[1] ; sec[1] <= max[1] ; sec[1]++)
	for (sec[0] = min[0] ; sec[0] <= max[0] ; sec[0]++)
	{
		/* Find or create sector. */
		index = LIVOX_SECTOR_INDEX (sec[0], sec[1], sec[2]);
		sector = livox_manager_find_sector (self->voxels, index);
		if (sector == NULL)
		{
			sector = livox_manager_create_sector (self->voxels, index);
			livox_sector_fill (sector, livox_voxel_init (0xFF, 1));
		}

		/* Calculate paint offset. */
		off[0] = stroke->pos[0] - sec[0] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
		off[1] = stroke->pos[1] - sec[1] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);
		off[2] = stroke->pos[2] - sec[2] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE);

		/* Copy brush voxels to sector. */
		for (src[2] = 0, dst[2] = off[2], i = 0 ; src[2] < brush->size[2] ; src[2]++, dst[2]++)
		for (src[1] = 0, dst[1] = off[1] ; src[1] < brush->size[1] ; src[1]++, dst[1]++)
		for (src[0] = 0, dst[0] = off[0] ; src[0] < brush->size[0] ; src[0]++, dst[0]++, i++)
		{
			if (0 <= dst[0] && dst[0] < LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE &&
				0 <= dst[1] && dst[1] < LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE &&
				0 <= dst[2] && dst[2] < LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE)
				livox_sector_set_voxel (sector, dst[0], dst[1], dst[2], brush->voxels.array[i]);
		}
	}

	return 1;
}

/** @} */
/** @} */
