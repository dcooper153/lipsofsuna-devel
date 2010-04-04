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
 * \addtogroup ligen Generator
 * @{
 * \addtogroup LIGenGenerator Generator
 * @{
 */

#include <lipsofsuna/engine.h>
#include <lipsofsuna/network.h>
#include <lipsofsuna/system.h>
#include "generator.h"

static int
private_init_brushes (LIGenGenerator* self);

static int
private_init_sql (LIGenGenerator* self);

static int
private_init_tables (LIGenGenerator* self);

static int
private_brush_disabled (LIGenGenerator*  self,
                        LIGenStroke*     stroke,
                        LIGenRulestroke* rstroke);

static int
private_brush_exists (LIGenGenerator*  self,
                      LIGenStroke*     stroke,
                      LIGenRulestroke* rstroke);

static int
private_brush_intersects (LIGenGenerator*  self,
                          LIGenStroke*     stroke,
                          LIGenRulestroke* rstroke);

static int
private_rule_apply (LIGenGenerator* self,
                    LIGenStroke*    stroke,
                    LIGenRule*      rule,
                    LIGenBrush*     brush);

static int
private_rule_test (LIGenGenerator* self,
                   LIGenStroke*    stroke,
                   LIGenRule*      rule);

static int
private_stroke_paint (LIGenGenerator* self,
                      LIGenStroke*    stroke);

/*****************************************************************************/

/**
 * \brief Creates a new generator module.
 *
 * \param paths Path information.
 * \param callbacks Callbacks.
 * \param sectors Sector manager.
 * \return New generator or NULL.
 */
LIGenGenerator*
ligen_generator_new (LIPthPaths*     paths,
                     LICalCallbacks* callbacks,
                     LIAlgSectors*   sectors)
{
	LIGenGenerator* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIGenGenerator));
	if (self == NULL)
	{
		lisys_error_report ();
		return NULL;
	}
	self->paths = paths;
	self->callbacks = callbacks;
	self->sectors = sectors;

	/* Allocate terrain structures. */
	self->voxels = livox_manager_new (callbacks, sectors);
	if (self->voxels == NULL)
		goto error;
	ligen_generator_set_fill (self, 1);

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
ligen_generator_free (LIGenGenerator* self)
{
	LIAlgU32dicIter iter;

	if (self->brushes != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->brushes)
			ligen_brush_free (iter.value);
		lialg_u32dic_free (self->brushes);
	}
	if (self->voxels != NULL)
		livox_manager_free (self->voxels);
	if (self->sql != NULL)
		sqlite3_close (self->sql);
	lisys_free (self->strokes.array);
	lisys_free (self);
}

/**
 * \brief Removes all the created map paint operations.
 *
 * \param self Generator.
 */
void
ligen_generator_clear_scene (LIGenGenerator* self)
{
	/* Free strokes. */
	lisys_free (self->strokes.array);
	self->strokes.array = NULL;
	self->strokes.count = 0;

	/* Clear scene. */
	lialg_sectors_clear (self->sectors);
	livox_manager_update (self->voxels, 1.0f);
}

/**
 * \brief Finds a brush by id.
 *
 * \param self Generator.
 * \param id Brush number.
 * \return Brush or NULL.
 */
LIGenBrush*
ligen_generator_find_brush (LIGenGenerator* self,
                            int             id)
{
	return lialg_u32dic_find (self->brushes, id);
}

/**
 * \brief Finds a brush by name.
 *
 * \param self Generator.
 * \param name Brush name.
 * \return Brush or NULL.
 */
LIGenBrush*
ligen_generator_find_brush_by_name (LIGenGenerator* self,
                                    const char*     name)
{
	LIAlgU32dicIter iter;
	LIGenBrush* brush;

	/* Find root brush. */
	/* TODO: Dictionary would be faster. */
	LIALG_U32DIC_FOREACH (iter, self->brushes)
	{
		brush = iter.value;
		if (!strcmp (brush->name, name))
			return brush;
	}

	return NULL;
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
ligen_generator_insert_brush (LIGenGenerator* self,
                              LIGenBrush*     brush)
{
	int i;

	if (brush->id >= 0)
	{
		i = brush->id;
		if (lialg_u32dic_find (self->brushes, i) != NULL)
		{
			lisys_assert (0);
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
ligen_generator_insert_stroke (LIGenGenerator* self,
                               int             brush,
                               int             x,
                               int             y,
                               int             z)
{
	LIGenBrush* brush_;
	LIGenStroke stroke;

	brush_ = lialg_u32dic_find (self->brushes, brush);
	lisys_assert (brush_ != NULL);
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

int
ligen_generator_load_materials (LIGenGenerator* self)
{
	return livox_manager_load_materials (self->voxels);
}

/**
 * \brief Enters the main loop of the generator.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
ligen_generator_main (LIGenGenerator* self)
{
	int i;
	LIGenBrush* brush;
	LIGenStroke stroke;

	/* Find root brush. */
	brush = ligen_generator_find_brush_by_name (self, "root");
	if (brush == NULL)
	{
		lisys_error_set (EINVAL, "cannot find `root' brush");
		return 0;
	}

	/* Create root stroke. */
	i = LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE * self->sectors->count / 2;
	stroke.pos[0] = i - brush->size[0] / 2;
	stroke.pos[1] = i - brush->size[1] / 2;
	stroke.pos[2] = i - brush->size[2] / 2;
	stroke.size[0] = brush->size[0];
	stroke.size[1] = brush->size[1];
	stroke.size[2] = brush->size[2];
	stroke.brush = brush->id;
	if (!lialg_array_append (&self->strokes, &stroke))
		return 0;

	/* Generate areas. */
	/* FIXME */
	for (i = 0 ; i < 100 ; i++)
	{
		if (!ligen_generator_step (self))
			break;
	}

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
ligen_generator_rebuild_scene (LIGenGenerator* self)
{
	int i;
	LIGenStroke* stroke;

	/* Clear sectors. */
	lialg_sectors_clear (self->sectors);

	/* Paint strokes. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		stroke = self->strokes.array + i;
		private_stroke_paint (self, stroke);
	}

	/* Rebuild geometry. */
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
ligen_generator_remove_brush (LIGenGenerator* self,
                              int             id)
{
	LIAlgU32dicIter iter;
	LIGenBrush* brush;

	/* Find brush. */
	brush = lialg_u32dic_find (self->brushes, id);
	if (brush == NULL)
		return;

	/* Clear references. */
	LIALG_U32DIC_FOREACH (iter, self->brushes)
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
ligen_generator_step (LIGenGenerator* self)
{
	int i;
	int j;
	int k;
	int tmp;
	int* rnd;
	int* rnd1;
	LIGenBrush* brush;
	LIGenRule* rule;
	LIGenStroke stroke;

	/* Randomize stroke order. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		k = rand () % self->strokes.count;
		stroke = self->strokes.array[i];
		self->strokes.array[i] = self->strokes.array[k];
		self->strokes.array[k] = stroke;
	}
	rnd = NULL;

	/* Try to expand each stroke. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		/* The stroke array may be reallocated in private_rule_apply
		 * so we need to create a copy of the stroke here. */
		stroke = self->strokes.array[i];
		brush = lialg_u32dic_find (self->brushes, stroke.brush);
		lisys_assert (brush != NULL);
		if (!brush->rules.count)
			continue;

		/* Randomize rule order. */
		rnd1 = realloc (rnd, brush->rules.count * sizeof (int));
		if (rnd1 == NULL)
		{
			lisys_free (rnd);
			return 0;
		}
		rnd = rnd1;
		for (j = 0 ; j < brush->rules.count ; j++)
			rnd[j] = j;
		for (j = 0 ; j < brush->rules.count ; j++)
		{
			k = rand () % brush->rules.count;
			tmp = rnd[j];
			rnd[j] = rnd[k];
			rnd[k] = tmp;
		}

		/* Try each rule. */
		for (j = 0 ; j < brush->rules.count ; j++)
		{
			rule = brush->rules.array[rnd[j]];
			if (private_rule_test (self, &stroke, rule))
			{
				lisys_free (rnd);
				if (!private_rule_apply (self, &stroke, rule, brush))
					return 0;
				return 1;
			}
		}
	}
	lisys_free (rnd);

	return 0;
}

/**
 * \brief Saves the generated map to the server database.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
ligen_generator_write (LIGenGenerator* self)
{
	int i;
	int j;
	int len;
	int ret;
	int flags;
	double rnd;
	char* path;
	const void* buf;
	uint32_t id;
	uint32_t sector;
	const char* query;
	LIArcSql* server;
	LIArcWriter* writer;
	LIGenBrush* brush;
	LIGenBrushobject* object;
	LIGenStroke* stroke;
	LIMatTransform transform;
	LIMatVector position;
	LIMatVector size;
	sqlite3_stmt* statement;

	/* Format path. */
	path = lipth_paths_get_sql (self->paths, "server.db");
	if (path == NULL)
		return 0;

	/* Open server database. */
	flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	if (sqlite3_open_v2 (path, &server, flags, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "sqlite: %s", sqlite3_errmsg (server));
		sqlite3_close (server);
		lisys_free (path);
		return 0;
	}
	lisys_free (path);

	/* Drop old tables. */
	liarc_sql_drop (server, "objects");
	liarc_sql_drop (server, "regions");
	liarc_sql_drop (server, "voxel_sectors");
	liarc_sql_drop (server, "voxel_materials");

	/* Save terrain. */
	livox_manager_set_sql (self->voxels, server);
	if (!livox_manager_write (self->voxels))
		lisys_error_report ();
	livox_manager_set_sql (self->voxels, self->sql);

	/* Create region table. */
	query = "CREATE TABLE IF NOT EXISTS regions "
		"(id INTEGER PRIMARY KEY,sector UNSIGNED INTEGER,data BLOB);";
	if (!liarc_sql_query (server, query))
	{
		sqlite3_close (server);
		return 0;
	}

	/* Write strokes as regions. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		stroke = self->strokes.array + i;
		writer = liarc_writer_new ();
		if (writer == NULL)
			return 0;
		position.x = LIVOX_TILE_WIDTH * stroke->pos[0];
		position.y = LIVOX_TILE_WIDTH * stroke->pos[1];
		position.z = LIVOX_TILE_WIDTH * stroke->pos[2];
		size.x = LIVOX_TILE_WIDTH * stroke->size[0];
		size.y = LIVOX_TILE_WIDTH * stroke->size[1];
		size.z = LIVOX_TILE_WIDTH * stroke->size[2];
		sector = lialg_sectors_point_to_index (self->sectors, &position);
		if (!liarc_writer_append_uint32 (writer, i) || /* id */
		    !liarc_writer_append_uint32 (writer, 0) || /* type */
		    !liarc_writer_append_uint32 (writer, 0) || /* flags */
		    !liarc_writer_append_uint32 (writer, stroke->brush) ||
		    !liarc_writer_append_float (writer, position.x) ||
		    !liarc_writer_append_float (writer, position.y) ||
		    !liarc_writer_append_float (writer, position.z) ||
		    !liarc_writer_append_float (writer, size.x) ||
		    !liarc_writer_append_float (writer, size.y) ||
		    !liarc_writer_append_float (writer, size.z))
		{
			liarc_writer_free (writer);
			return 0;
		}
		len = liarc_writer_get_length (writer);
		buf = liarc_writer_get_buffer (writer);
		if (!liarc_sql_replace (server, "regions",
			"id", LIARC_SQL_INT, i,
			"sector", LIARC_SQL_INT, sector,
			"data", LIARC_SQL_BLOB, buf, len, NULL))
		{
			liarc_writer_free (writer);
			sqlite3_close (server);
			return 0;
		}
		liarc_writer_free (writer);
	}

	/* Create object table. */
	query = "CREATE TABLE IF NOT EXISTS objects "
		"(id INTEGER PRIMARY KEY,sector UNSIGNED INTEGER,model TEXT,"
		"flags UNSIGNED INTEGER,angx REAL,angy REAL,angz REAL,posx REAL,"
		"posy REAL,posz REAL,rotx REAL,roty REAL,rotz REAL,rotw REAL,"
		"mass REAL,move REAL,speed REAL,step REAL,colgrp UNSIGNED INTEGER,"
		"colmsk UNSIGNED INTEGER,control UNSIGNED INTEGER,type TEXT,extra TEXT);";
	if (!liarc_sql_query (server, query))
	{
		sqlite3_close (server);
		return 0;
	}

	/* Save new objects. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		stroke = self->strokes.array + i;
		brush = lialg_u32dic_find (self->brushes, stroke->brush);
		lisys_assert (brush != NULL);
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
				if (sqlite3_prepare_v2 (server, query, -1, &statement, NULL) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (server));
					sqlite3_close (server);
					return 0;
				}
				if (sqlite3_bind_int (statement, 1, id) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (server));
					sqlite3_finalize (statement);
					sqlite3_close (server);
					return 0;
				}
				ret = sqlite3_step (statement);
				if (ret != SQLITE_DONE)
				{
					if (ret != SQLITE_ROW)
					{
						lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (server));
						sqlite3_finalize (statement);
						sqlite3_close (server);
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
			sector = lialg_sectors_point_to_index (self->sectors, &transform.position);

			/* Write values. */
			if (!liarc_sql_replace (server, "objects",
				"id", LIARC_SQL_INT, id,
				"sector", LIARC_SQL_INT, sector,
				"flags", LIARC_SQL_INT, flags,
				"angx", LIARC_SQL_FLOAT, 0.0f,
				"angy", LIARC_SQL_FLOAT, 0.0f,
				"angz", LIARC_SQL_FLOAT, 0.0f,
				"posx", LIARC_SQL_FLOAT, transform.position.x,
				"posy", LIARC_SQL_FLOAT, transform.position.y,
				"posz", LIARC_SQL_FLOAT, transform.position.z,
				"rotx", LIARC_SQL_FLOAT, transform.rotation.x,
				"roty", LIARC_SQL_FLOAT, transform.rotation.y,
				"rotz", LIARC_SQL_FLOAT, transform.rotation.z,
				"rotw", LIARC_SQL_FLOAT, transform.rotation.w,
				"mass", LIARC_SQL_FLOAT, 0.0f,
				"move", LIARC_SQL_FLOAT, 0.0f,
				"speed", LIARC_SQL_FLOAT, 5.0f,
				"step", LIARC_SQL_FLOAT, 0.0f,
				"colgrp", LIARC_SQL_INT, LIPHY_DEFAULT_COLLISION_GROUP,
				"colmsk", LIARC_SQL_INT, LIPHY_DEFAULT_COLLISION_MASK,
				"control", LIARC_SQL_INT, LIPHY_CONTROL_MODE_NONE,
				"model", LIARC_SQL_TEXT, object->model,
				"type", LIARC_SQL_TEXT, object->type,
				"extra", LIARC_SQL_TEXT, object->extra, NULL))
			{
				sqlite3_close (server);
				return 0;
			}
		}
	}

	sqlite3_close (server);

	return 1;
}

/**
 * \brief Saves brushes to the generator database.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
ligen_generator_write_brushes (LIGenGenerator* self)
{
	LIAlgU32dicIter iter;

	/* Remove old brushes. */
	if (!liarc_sql_delete (self->sql, "generator_brushes") ||
	    !liarc_sql_delete (self->sql, "generator_rules") ||
	    !liarc_sql_delete (self->sql, "generator_strokes") ||
	    !liarc_sql_delete (self->sql, "generator_objects"))
		return 0;

	/* Save brushes. */
	LIALG_U32DIC_FOREACH (iter, self->brushes)
	{
		if (!ligen_brush_write (iter.value, self->sql))
			return 0;
	}

	return 1;
}

/**
 * \brief Saves materials to the generator database.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
ligen_generator_write_materials (LIGenGenerator* self)
{
	return livox_manager_write_materials (self->voxels);
}

/**
 * \brief Sets the material used for filling empty sectors.
 *
 * \param self Generator.
 * \param fill Material number.
 */
void
ligen_generator_set_fill (LIGenGenerator* self,
                          int             fill)
{
	if (fill < 0)
		self->fill = 0;
	else
		self->fill = fill;
	livox_manager_set_fill (self->voxels, self->fill);
}

/*****************************************************************************/

static int
private_init_brushes (LIGenGenerator* self)
{
	int i;
	int id;
	int col;
	int ret;
	int size0;
	int size1;
	int size[3];
	char* name;
	const char* query;
	const void* bytes;
	LIAlgU32dicIter iter;
	LIArcReader* reader;
	LIArcSql* sql;
	LIGenBrush* brush;
	sqlite3_stmt* statement;

	sql = self->sql;

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
		size[0] = LIMAT_MAX (1, size[0]);
		size[1] = LIMAT_MAX (1, size[1]);
		size[2] = LIMAT_MAX (1, size[2]);

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
			lisys_free (brush->name);
			brush->name = listr_dup (name);
			if (brush->name == NULL)
			{
				sqlite3_finalize (statement);
				return 0;
			}
		}

		/* Read voxels column. */
		bytes = sqlite3_column_blob (statement, col);
		size0 = sqlite3_column_bytes (statement, col);
		size1 = size[0] * size[1] * size[2] * 4;
		if (size0 == size1)
		{
			reader = liarc_reader_new (bytes, size0);
			if (reader != NULL)
			{
				for (i = 0 ; i < size[0] * size[1] * size[2] ; i++)
				{
					liarc_reader_get_uint16 (reader, &brush->voxels.array[i].type);
					liarc_reader_get_uint8 (reader, &brush->voxels.array[i].damage);
					liarc_reader_get_uint8 (reader, &brush->voxels.array[i].rotation);
				}
				liarc_reader_free (reader);
			}
		}
	}
	sqlite3_finalize (statement);

	/* Read rules. */
	LIALG_U32DIC_FOREACH (iter, self->brushes)
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
private_init_sql (LIGenGenerator* self)
{
	int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	char* path;

	/* Format path. */
	path = lipth_paths_get_sql (self->paths, "generator.db");
	if (path == NULL)
		return 0;

	/* Open database. */
	if (sqlite3_open_v2 (path, &self->sql, flags, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "sqlite: %s", sqlite3_errmsg (self->sql));
		sqlite3_close (self->sql);
		lisys_free (path);
		return 0;
	}
	lisys_free (path);

	/* Load materials from it. */
	livox_manager_set_sql (self->voxels, self->sql);
	livox_manager_set_load (self->voxels, 0);
	livox_manager_load_materials (self->voxels);

	return 1;
}

static int
private_init_tables (LIGenGenerator* self)
{
	const char* query;

	/* Create brush table. */
	query = "CREATE TABLE IF NOT EXISTS generator_brushes "
		"(id INTEGER PRIMARY KEY,name TEXT,sizx UNSIGNED INTEGER,"
		"sizy UNSIGNED INTEGER,sizz UNSIGNED INTEGER,voxels BLOB);";
	if (!liarc_sql_query (self->sql, query))
		return 0;

	/* Create object table. */
	query = "CREATE TABLE IF NOT EXISTS generator_objects "
		"(id INTEGER,"
		"brush INTEGER REFERENCES generator_brushes(id),"
		"flags UNSIGNED INTEGER,prob REAL,posx REAL,posy REAL,posz REAL,"
		"rotx REAL,roty REAL,rotz REAL,rotw REAL,type TEXT,model TEXT,extra TEXT);";
	if (!liarc_sql_query (self->sql, query))
		return 0;

	/* Create rule table. */
	query = "CREATE TABLE IF NOT EXISTS generator_rules "
		"(id INTEGER,"
		"brush INTEGER REFERENCES generator_brushes(id),"
		"name TEXT,flags UNSIGNED INTEGER);";
	if (!liarc_sql_query (self->sql, query))
		return 0;

	/* Create stroke table. */
	query = "CREATE TABLE IF NOT EXISTS generator_strokes "
		"(id INTEGER,"
		"brush INTEGER REFERENCES generator_brushes(id),"
		"rule INTEGER,"
		"paint INTEGER REFERENCES generator_brushes(id),"
		"x UNSIGNED INTEGER,y UNSIGNED INTEGER,z UNSIGNED INTEGER,"
		"flags UNSIGNED INTEGER);";
	if (!liarc_sql_query (self->sql, query))
		return 0;

	return 1;
}

static int
private_brush_disabled (LIGenGenerator*  self,
                        LIGenStroke*     stroke,
                        LIGenRulestroke* rstroke)
{
	LIGenBrush* brush;

	brush = lialg_u32dic_find (self->brushes, rstroke->brush);
	lisys_assert (brush != NULL);

	return brush->disabled;
}

static int
private_brush_exists (LIGenGenerator*  self,
                      LIGenStroke*     stroke,
                      LIGenRulestroke* rstroke)
{
	int i;
	int pos[3];
	LIGenStroke* stroke1;

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
private_brush_intersects (LIGenGenerator*  self,
                          LIGenStroke*     stroke,
                          LIGenRulestroke* rstroke)
{
	int i;
	int min0[3];
	int min1[3];
	int max0[3];
	int max1[3];
	LIGenBrush* brush;
	LIGenStroke* stroke1;

	/* Calculate world position. */
	brush = lialg_u32dic_find (self->brushes, rstroke->brush);
	lisys_assert (brush != NULL);
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
private_rule_apply (LIGenGenerator* self,
                    LIGenStroke*    stroke,
                    LIGenRule*      rule,
                    LIGenBrush*     brush)
{
	int i;
	int orig;
	LIGenBrush* brush1;
	LIGenStroke stroke1;
	LIGenRulestroke* rstroke;

	printf ("BRUSH %s RULE %s\n", brush->name, rule->name);

	orig = self->strokes.count;
	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		rstroke = rule->strokes.array + i;
		brush1 = lialg_u32dic_find (self->brushes, rstroke->brush);
		lisys_assert (brush1 != NULL);
		printf (" * CREATE %s\n", brush1->name);
		stroke1.pos[0] = stroke->pos[0] + rstroke->pos[0];
		stroke1.pos[1] = stroke->pos[1] + rstroke->pos[1];
		stroke1.pos[2] = stroke->pos[2] + rstroke->pos[2];
		stroke1.size[0] = brush1->size[0];
		stroke1.size[1] = brush1->size[1];
		stroke1.size[2] = brush1->size[2];
		stroke1.brush = brush1->id;
		if (!lialg_array_append (&self->strokes, &stroke1))
		{
			self->strokes.count = orig;
			return 0;
		}
	}

	return 1;
}

static int
private_rule_test (LIGenGenerator* self,
                   LIGenStroke*    stroke,
                   LIGenRule*      rule)
{
	int i;
	LIGenRulestroke* rstroke;

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
			if (private_brush_disabled (self, stroke, rstroke) ||
			    private_brush_intersects (self, stroke, rstroke))
				return 0;
		}
	}

	return 1;
}

static int
private_stroke_paint (LIGenGenerator* self,
                      LIGenStroke*    stroke)
{
	int i;
	int min[3];
	int max[3];
	int off[3];
	int sec[3];
	int src[3];
	int dst[3];
	LIGenBrush* brush;
	LIVoxSector* sector;

	/* Determine affected sectors. */
	brush = lialg_u32dic_find (self->brushes, stroke->brush);
	lisys_assert (brush != NULL);
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
		sector = lialg_sectors_data_offset (self->sectors, "voxel", sec[0], sec[1], sec[2], 1);
		if (sector == NULL)
			return 0;

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
