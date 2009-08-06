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
#include <engine/lips-engine.h>
#include <network/lips-network.h>
#include "generator.h"

static int
private_init_brushes (ligenGenerator* self);

static int
private_init_sql (ligenGenerator* self);

static int
private_init_tables (ligenGenerator* self);

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
 * \param path Package root directory.
 * \param name Module name.
 * \return New generator or NULL.
 */
ligenGenerator*
ligen_generator_new (const char* path,
                     const char* name)
{
	return ligen_generator_new_full (path, name, NULL, NULL);
}

/**
 * \brief Creates a new generator module.
 *
 * \param path Package root directory.
 * \param name Module name.
 * \param scene Render scene or NULL.
 * \param rndapi Render API or NULL.
 * \return New generator or NULL.
 */
ligenGenerator*
ligen_generator_new_full (const char* path,
                          const char* name,
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
	ligen_generator_set_fill (self, 1);

	/* Allocate paths. */
	self->paths = lipth_paths_new (path, name);
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
	    !private_init_tables (self) ||
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
	lialgU32dicIter iter;

	if (self->brushes != NULL)
	{
		LI_FOREACH_U32DIC (iter, self->brushes)
			ligen_brush_free (iter.value);
		lialg_u32dic_free (self->brushes);
	}
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
	free (self->strokes.array);
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
	free (self->strokes.array);
	self->strokes.array = NULL;
	self->strokes.count = 0;

	/* Clear scene. */
	livox_manager_clear (self->voxels);
	livox_manager_update (self->voxels, 1.0f);
}

/**
 * \brief Finds a brush by id.
 *
 * \param self Generator.
 * \return Brush or NULL.
 */
ligenBrush*
ligen_generator_find_brush (ligenGenerator* self,
                            int             id)
{
	return lialg_u32dic_find (self->brushes, id);
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
	int i;

	if (brush->id >= 0)
	{
		i = brush->id;
		if (lialg_u32dic_find (self->brushes, i) != NULL)
		{
			assert (0);
			return 0;
		}
	}
	else
	{
		for (i = 0 ; lialg_u32dic_find (self->brushes, i) != NULL ; i++)
		{
		}
	}
	if (!lialg_u32dic_insert (self->brushes, i, brush))
		return 0;
	brush->id = i;

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

	brush_ = lialg_u32dic_find (self->brushes, brush);
	assert (brush_ != NULL);
	stroke.pos[0] = x;
	stroke.pos[1] = y;
	stroke.pos[2] = z;
	stroke.size[0] = brush_->size[0];
	stroke.size[1] = brush_->size[1];
	stroke.size[2] = brush_->size[2];
	stroke.brush = brush;
	if (!lialg_array_append (&self->strokes, &stroke))
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
	ligenBrush* brush;
	ligenStroke stroke;

	/* FIXME: This should be configurable. */
	brush = lialg_u32dic_find (self->brushes, 0);
	if (brush != NULL);
	{
		stroke.pos[0] = 8160 - brush->size[0] / 2;
		stroke.pos[1] = 8160 - brush->size[1] / 2;
		stroke.pos[2] = 8160 - brush->size[2] / 2;
		stroke.size[0] = brush->size[0];
		stroke.size[1] = brush->size[1];
		stroke.size[2] = brush->size[2];
		stroke.brush = brush->id;
		if (!lialg_array_append (&self->strokes, &stroke))
			return 0;
	}

	/* Generate areas. */
	/* FIXME */
	for (i = 0 ; i < 20 ; i++)
	{
		if (!ligen_generator_step (self))
			break;
	}
	printf ("Strokes: %d\n", i);

	/* Generate geometry. */
	if (!ligen_generator_rebuild_scene (self) ||
	    !ligen_generator_write (self))
		return 0;

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
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		stroke = self->strokes.array + i;
		private_stroke_paint (self, stroke);
	}

	/* Rebuild geometry. */
	if (self->voxels->scene != NULL)
		livox_manager_update (self->voxels, 1.0f);

	return 1;
}

/**
 * \brief Removes a brush from the generator.
 *
 * The brush and all the strokes referencing it are removed.
 *
 * \param self Generator.
 * \param id Brush number.
 */
void
ligen_generator_remove_brush (ligenGenerator* self,
                              int             id)
{
	lialgU32dicIter iter;
	ligenBrush* brush;

	/* Find brush. */
	brush = lialg_u32dic_find (self->brushes, id);
	if (brush == NULL)
		return;

	/* Clear references. */
	LI_FOREACH_U32DIC (iter, self->brushes)
		ligen_brush_remove_strokes (iter.value, id);

	/* Free brush. */
	lialg_u32dic_remove (self->brushes, id);
	ligen_brush_free (brush);
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
	ligenRule* rule;
	ligenStroke stroke;

	for (i = 0 ; i < self->strokes.count ; i++)
	{
		/* The stroke array may be reallocated in private_rule_apply
		 * so we need to create a copy of the stroke here. */
		stroke = self->strokes.array[i];
		brush = lialg_u32dic_find (self->brushes, stroke.brush);
		assert (brush != NULL);
		for (j = 0 ; j < brush->rules.count ; j++)
		{
			rule = brush->rules.array[j];
			if (private_rule_test (self, &stroke, rule))
			{
				if (!private_rule_apply (self, &stroke, rule))
					return 0;
				return 1;
			}
		}
	}

	return 0;
}

/**
 * \brief Saves the generated map to the server database.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
ligen_generator_write (ligenGenerator* self)
{
	int i;
	int j;
	int col;
	int ret;
	int flags;
	double rnd;
	uint32_t id;
	uint32_t sector;
	const char* query;
	ligenBrush* brush;
	ligenBrushobject* object;
	ligenStroke* stroke;
	limatTransform transform;
	sqlite3_stmt* statement;

	/* Save geometry. */
	livox_manager_write (self->voxels, self->srvsql);

	/* Remove old objects. */
	query = "DELETE FROM objects;";
	if (sqlite3_prepare_v2 (self->srvsql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self->srvsql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self->srvsql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Save new objects. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		stroke = self->strokes.array + i;
		brush = lialg_u32dic_find (self->brushes, stroke->brush);
		assert (brush != NULL);
		for (j = 0 ; j < brush->objects.count ; j++)
		{
			object = brush->objects.array[j];

			/* Creation probability. */
			rnd = rand () / (double) RAND_MAX;
			if (rnd <= 1.0 - object->probability)
				continue;

			/* Choose unique object number. */
			for (id = 0 ; !id ; )
			{
				/* Choose random number. */
				rnd = rand () / (double) RAND_MAX;
				id = LINET_RANGE_SERVER_START + (uint32_t)((LINET_RANGE_SERVER_END - LINET_RANGE_SERVER_START) * rnd);
				if (!id)
					continue;

				/* Reject numbers of database objects. */
				query = "SELECT id FROM objects WHERE id=?;";
				if (sqlite3_prepare_v2 (self->srvsql, query, -1, &statement, NULL) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self->srvsql));
					return 0;
				}
				if (sqlite3_bind_int (statement, 1, id) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self->srvsql));
					sqlite3_finalize (statement);
					return 0;
				}
				ret = sqlite3_step (statement);
				if (ret != SQLITE_DONE)
				{
					if (ret != SQLITE_ROW)
					{
						lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self->srvsql));
						sqlite3_finalize (statement);
						return 0;
					}
					id = 0;
				}
				sqlite3_finalize (statement);
			}

			/* Collect values. */
			flags = object->flags;
			transform = object->transform;
			transform.position.x += LIVOX_TILE_WIDTH * stroke->pos[0];
			transform.position.y += LIVOX_TILE_WIDTH * stroke->pos[1];
			transform.position.z += LIVOX_TILE_WIDTH * stroke->pos[2];
			sector = LIENG_SECTOR_INDEX (
				(int)(transform.position.x / LIENG_SECTOR_WIDTH),
				(int)(transform.position.y / LIENG_SECTOR_WIDTH),
				(int)(transform.position.z / LIENG_SECTOR_WIDTH));

			/* Prepare statement. */
			query = "INSERT OR REPLACE INTO objects "
				"(id,sector,flags,angx,angy,angz,posx,posy,posz,rotx,roty,rotz,"
				"rotw,mass,move,speed,step,colgrp,colmsk,control,shape,model,type,extra) VALUES "
				"(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
			if (sqlite3_prepare_v2 (self->srvsql, query, -1, &statement, NULL) != SQLITE_OK)
			{
				lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self->srvsql));
				return 0;
			}

			/* Bind values. */
			col = 1;
			ret = (sqlite3_bind_int (statement, col++, id) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, sector) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, flags) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, 0.0f) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, 0.0f) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, 0.0f) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.position.x) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.position.y) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.position.z) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.rotation.x) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.rotation.y) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.rotation.z) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, transform.rotation.w) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, 0.0f) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, 0.0f) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, 5.0f) != SQLITE_OK ||
				sqlite3_bind_double (statement, col++, 0.0f) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, LIPHY_DEFAULT_COLLISION_GROUP) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, LIPHY_DEFAULT_COLLISION_MASK) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, LIPHY_CONTROL_MODE_NONE) != SQLITE_OK ||
				sqlite3_bind_int (statement, col++, LIPHY_SHAPE_MODE_CONCAVE) != SQLITE_OK ||
				sqlite3_bind_text (statement, col++, object->model, -1, SQLITE_TRANSIENT) != SQLITE_OK ||
				sqlite3_bind_text (statement, col++, object->type, -1, SQLITE_TRANSIENT) != SQLITE_OK ||
				sqlite3_bind_text (statement, col++, object->extra, -1, SQLITE_TRANSIENT) != SQLITE_OK);
			if (ret)
			{
				lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self->srvsql));
				sqlite3_finalize (statement);
				return 0;
			}

			/* Write values. */
			if (sqlite3_step (statement) != SQLITE_DONE)
			{
				lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self->srvsql));
				sqlite3_finalize (statement);
				return 0;
			}
			sqlite3_finalize (statement);
		}
	}

	return 1;
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
	const char* query;
	lialgU32dicIter iter;
	sqlite3_stmt* statement;

	/* Remove old brushes. */
	query = "DELETE FROM generator_brushes;";
	if (sqlite3_prepare_v2 (self->gensql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self->gensql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self->gensql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Remove old rules. */
	query = "DELETE FROM generator_rules;";
	if (sqlite3_prepare_v2 (self->gensql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self->gensql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self->gensql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Remove old strokes. */
	query = "DELETE FROM generator_strokes;";
	if (sqlite3_prepare_v2 (self->gensql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self->gensql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self->gensql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Remove old objects. */
	query = "DELETE FROM generator_objects;";
	if (sqlite3_prepare_v2 (self->gensql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self->gensql));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self->gensql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	/* Save brushes. */
	LI_FOREACH_U32DIC (iter, self->brushes)
		ligen_brush_write (iter.value, self->gensql);

	return 1;
}

void
ligen_generator_set_fill (ligenGenerator* self,
                          int             fill)
{
	if (fill < 0)
		self->fill = livox_voxel_init (0x00, 0);
	else
		self->fill = livox_voxel_init (0xFF, fill);
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
	lialgU32dicIter iter;
	liarcSql* sql;
	ligenBrush* brush;
	sqlite3_stmt* statement;

	sql = self->gensql;

	/* Allocate dictionary. */
	self->brushes = lialg_u32dic_new ();
	if (self->brushes == NULL)
		return 0;

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
		brush = ligen_brush_new (self, size[0], size[1], size[2]);
		if (brush == NULL)
		{
			sqlite3_finalize (statement);
			return 0;
		}
		brush->id = id;
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
	}
	sqlite3_finalize (statement);

	/* Read rules. */
	LI_FOREACH_U32DIC (iter, self->brushes)
	{
		if (!ligen_brush_read_rules (iter.value, sql))
		{
			sqlite3_finalize (statement);
			return 0;
		}
	}

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
private_init_tables (ligenGenerator* self)
{
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

	/* Create object table. */
	query = "CREATE TABLE IF NOT EXISTS generator_objects "
		"(id INTEGER,"
		"brush INTEGER REFERENCES generator_brushes(id),"
		"flags UNSIGNED INTEGER,prob REAL,posx REAL,posy REAL,posz REAL,"
		"rotx REAL,roty REAL,rotz REAL,rotw REAL,type TEXT,model TEXT,extra TEXT);";
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

	return 1;
}

static int
private_brush_exists (ligenGenerator*  self,
                      ligenStroke*     stroke,
                      ligenRulestroke* rstroke)
{
	int i;
	int pos[3];
	ligenStroke* stroke1;

	/* Calculate world position. */
	pos[0] = stroke->pos[0] + rstroke->pos[0];
	pos[1] = stroke->pos[1] + rstroke->pos[1];
	pos[2] = stroke->pos[2] + rstroke->pos[2];

	/* Test against all strokes. */
	/* FIXME: Use space partitioning. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		stroke1 = self->strokes.array + i;
		if (stroke1->brush == rstroke->brush &&
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
	brush = lialg_u32dic_find (self->brushes, rstroke->brush);
	assert (brush != NULL);
	min0[0] = stroke->pos[0] + rstroke->pos[0];
	min0[1] = stroke->pos[1] + rstroke->pos[1];
	min0[2] = stroke->pos[2] + rstroke->pos[2];
	max0[0] = min0[0] + brush->size[0];
	max0[1] = min0[1] + brush->size[1];
	max0[2] = min0[2] + brush->size[2];

	/* Test against all strokes. */
	/* FIXME: Use space partitioning. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		stroke1 = self->strokes.array + i;
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

	orig = self->strokes.count;
	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		rstroke = rule->strokes.array + i;
		brush = lialg_u32dic_find (self->brushes, rstroke->brush);
		assert (brush != NULL);
		stroke1.pos[0] = stroke->pos[0] + rstroke->pos[0];
		stroke1.pos[1] = stroke->pos[1] + rstroke->pos[1];
		stroke1.pos[2] = stroke->pos[2] + rstroke->pos[2];
		stroke1.size[0] = brush->size[0];
		stroke1.size[1] = brush->size[1];
		stroke1.size[2] = brush->size[2];
		stroke1.brush = brush->id;
		if (!lialg_array_append (&self->strokes, &stroke1))
		{
			self->strokes.count = orig;
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
	brush = lialg_u32dic_find (self->brushes, stroke->brush);
	assert (brush != NULL);
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
			if (self->fill != 0)
				livox_sector_fill (sector, self->fill);
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
