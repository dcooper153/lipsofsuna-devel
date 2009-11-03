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
 * \addtogroup ligenBrush Brush
 * @{
 */

#include "generator.h"
#include "generator-brush.h"

static int
private_read_objects (ligenBrush* self,
                      liarcSql*   sql);

static int
private_read_rules (ligenBrush* self,
                    liarcSql*   sql);

static int
private_write_rule (ligenBrush* self,
                    ligenRule*  rule,
                    liarcSql*   sql);

static int
private_write_object (ligenBrush*       self,
                      ligenBrushobject* object,
                      liarcSql*         sql);

static int
private_write_stroke (ligenBrush*      self,
                      ligenRule*       rule,
                      ligenRulestroke* stroke,
                      int              id,
                      liarcSql*        sql);

/*****************************************************************************/

/**
 * \brief Creates a new brush.
 *
 * \param generator Generator.
 * \param width Width in voxels.
 * \param height Height in voxels.
 * \param depth Depth in voxels.
 * \return New brush or NULL.
 */
ligenBrush*
ligen_brush_new (ligenGenerator* generator,
                 int             width,
                 int             height,
                 int             depth)
{
	ligenBrush* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (ligenBrush));
	if (self == NULL)
		return NULL;
	self->generator = generator;
	self->id = -1;
	self->size[0] = width;
	self->size[1] = height;
	self->size[2] = depth;

	/* Allocate name. */
	self->name = lisys_calloc (1, sizeof (char));
	if (self->name == NULL)
		return NULL;

	/* Allocate voxels. */
	self->voxels.count = width * height * depth;
	self->voxels.array = lisys_calloc (self->voxels.count, sizeof (livoxVoxel));
	if (self->voxels.array == NULL)
	{
		lisys_free (self->name);
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the brush.
 *
 * \param self Brush.
 */
void
ligen_brush_free (ligenBrush* self)
{
	int i;

	for (i = 0 ; i < self->rules.count ; i++)
		ligen_rule_free (self->rules.array[i]);
	lisys_free (self->voxels.array);
	lisys_free (self->objects.array);
	lisys_free (self->rules.array);
	lisys_free (self->name);
	lisys_free (self);
}

/**
 * \brief Adds a new brush object.
 *
 * \param self Brush.
 * \param flags Object flags.
 * \param prob Generation probability.
 * \param type Type string.
 * \param model Model string.
 * \param extra Extra string.
 * \param transform Transformation relative to the brush.
 * \return Nonzero on success.
 */
int
ligen_brush_insert_object (ligenBrush*           self,
                           int                   flags,
                           float                 prob,
                           const char*           type,
                           const char*           model,
                           const char*           extra,
                           const limatTransform* transform)
{
	ligenBrushobject* object;

	/* Allocate object. */
	object = lisys_calloc (1, sizeof (ligenBrushobject));
	if (object == NULL)
		return 0;
	object->id = self->objects.count;
	object->flags = flags;
	object->probability = prob;
	object->type = listr_dup (type);
	object->model = listr_dup (model);
	object->extra = listr_dup (extra);
	object->transform = *transform;
	if (object->type == NULL || object->model == NULL || object->extra == NULL)
		goto error;

	/* Add to list. */
	if (!lialg_array_append (&self->objects, &object))
		goto error;

	return 1;

error:
	lisys_free (object->type);
	lisys_free (object->model);
	lisys_free (object->extra);
	lisys_free (object);
	return 0;
}

/**
 * \brief Inserts a rule to the brush.
 *
 * The ownership of the rule is transferred to the brush if succeeded.
 *
 * \param self Brush.
 * \param rule Rule.
 * \return Nonzero on success.
 */
int
ligen_brush_insert_rule (ligenBrush* self,
                         ligenRule*  rule)
{
	if (!lialg_array_append (&self->rules, &rule))
		return 0;
	rule->id = self->rules.count - 1;
	return 1;
}

/**
 * \brief Loads brush data from the database.
 *
 * \param self Brush.
 * \param sql Database.
 * \return Nonzero on success.
 */
int
ligen_brush_read_rules (ligenBrush* self,
                        liarcSql*   sql)
{
	return private_read_rules (self, sql) &&
	       private_read_objects (self, sql);
}

/**
 * \brief Removes an object from the brush.
 *
 * The id numbers of the other object may be altered.
 *
 * \param self Brush.
 * \param index Object number.
 */
void
ligen_brush_remove_object (ligenBrush* self,
                           int         index)
{
	int i;
	ligenBrushobject* object;

	assert (index >= 0);
	assert (index < self->objects.count);

	object = self->objects.array[index];
	lisys_free (object->model);
	lisys_free (object->type);
	lisys_free (object->extra);
	lisys_free (object);
	lialg_array_remove (&self->objects, index);

	for (i = 0 ; i < self->objects.count ; i++)
	{
		object = self->objects.array[i];
		object->id = i;
	}
}

/**
 * \brief Removes a rule from the brush.
 *
 * The numbers of the other rules may be altered.
 *
 * \param self Brush.
 * \param index Rule number.
 */
void
ligen_brush_remove_rule (ligenBrush* self,
                         int         index)
{
	int i;
	ligenRule* rule;

	assert (index >= 0);
	assert (index < self->rules.count);

	ligen_rule_free (self->rules.array[index]);
	lialg_array_remove (&self->rules, index);

	for (i = 0 ; i < self->rules.count ; i++)
	{
		rule = self->rules.array[i];
		rule->id = i;
	}
}

/**
 * \brief Removes all strokes that use the brush with the given id.
 *
 * Loops through all rules their strokes. If a stroke is found that references
 * the given brush id, the stroke is removed.
 *
 * \param self Brush.
 * \param brush Brush number.
 */
void
ligen_brush_remove_strokes (ligenBrush* self,
                            int         brush)
{
	int i;
	int j;
	ligenRule* rule;

	for (i = 0 ; i < self->rules.count ; i++)
	{
		rule = self->rules.array[i];
		for (j = 0 ; j < rule->strokes.count ; j++)
		{
			if (rule->strokes.array[j].brush == brush)
			{
				lialg_array_remove (&rule->strokes, j);
				j--;
			}
		}
	}
}

/**
 * \brief Saves the brush to the database.
 *
 * \param self Brush.
 * \param sql Database.
 * \return Nonzero on success.
 */
int
ligen_brush_write (ligenBrush* self,
                   liarcSql*   sql)
{
	int i;
	int ret;
	liarcWriter* writer;

	/* Serialize voxels. */
	writer = liarc_writer_new ();
	if (writer == NULL)
		return 0;
	for (i = 0 ; i < self->voxels.count ; i++)
	{
		if (!liarc_writer_append_uint16 (writer, self->voxels.array[i].type) ||
		    !liarc_writer_append_uint8 (writer, self->voxels.array[i].damage))
		{
			liarc_writer_free (writer);
			return 0;
		}
	}

	/* Prepare statement. */
	ret = liarc_sql_replace (sql, "generator_brushes",
		"id", LIARC_SQL_INT, self->id,
		"name", LIARC_SQL_TEXT, self->name,
		"sizx", LIARC_SQL_INT, self->size[0],
		"sizy", LIARC_SQL_INT, self->size[1],
		"sizz", LIARC_SQL_INT, self->size[2],
		"voxels", LIARC_SQL_BLOB, writer->memory.buffer, writer->memory.length, NULL);
	liarc_writer_free (writer);
	if (!ret)
		return 0;

	/* Write objects. */
	for (i = 0 ; i < self->objects.count ; i++)
	{
		if (!private_write_object (self, self->objects.array[i], sql))
			return 0;
	}

	/* Write rules. */
	for (i = 0 ; i < self->rules.count ; i++)
	{
		if (!private_write_rule (self, self->rules.array[i], sql))
			return 0;
	}

	return 1;
}

int
ligen_brush_set_name (ligenBrush* self,
                      const char* value)
{
	char* tmp;

	tmp = listr_dup (value);
	if (tmp == NULL)
		return 0;
	lisys_free (self->name);
	self->name = tmp;

	return 1;
}

int
ligen_brush_set_size (ligenBrush* self,
                      int         x,
                      int         y,
                      int         z)
{
	int i;
	int j;
	int k;
	livoxVoxel* tmp;

	assert (x > 0);
	assert (y > 0);
	assert (z > 0);

	tmp = lisys_calloc (x * y * z, sizeof (livoxVoxel));
	if (tmp == NULL)
		return 0;
	for (k = 0 ; k < self->size[2] && k < z ; k++)
	for (j = 0 ; j < self->size[1] && j < y ; j++)
	for (i = 0 ; i < self->size[0] && i < x ; i++)
	{
		tmp[i + j * x + k * x * y] = self->voxels.array[
			i + j * self->size[0] + k * self->size[0] * self->size[1]];
	}
	lisys_free (self->voxels.array);
	self->voxels.array = tmp;
	self->voxels.count = x * y * z;
	self->size[0] = x;
	self->size[1] = y;
	self->size[2] = z;

	return 1;
}

void
ligen_brush_set_voxel (ligenBrush* self,
                       int         x,
                       int         y,
                       int         z,
                       livoxVoxel  voxel)
{
	int i;

	assert (x >= 0);
	assert (y >= 0);
	assert (z >= 0);
	assert (x < self->size[0]);
	assert (y < self->size[1]);
	assert (z < self->size[2]);

	i = x + y * self->size[0] + z * self->size[1] * self->size[2];
	self->voxels.array[i] = voxel;
}

/*****************************************************************************/

static int
private_read_objects (ligenBrush* self,
                      liarcSql*   sql)
{
	int i;
	int col;
	int ret;
	int size;
	const char* query;
	ligenBrushobject object;
	ligenBrushobject** tmp;
	sqlite3_stmt* statement;
	struct
	{
		int count;
		ligenBrushobject** array;
	} objects = { 0, NULL };

	/* Prepare statement. */
	query = "SELECT id,flags,prob,posx,posy,posz,rotx,roty,rotz,rotw,type,model,extra "
		"FROM generator_objects WHERE brush=?;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}

	/* Read objects. */
	for (ret = sqlite3_step (statement) ; ret != SQLITE_DONE ; ret = sqlite3_step (statement))
	{
		/* Check for errors. */
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Read numeric fields. */
		col = 0;
		object.id = sqlite3_column_int (statement, col++);
		object.flags = sqlite3_column_int (statement, col++);
		object.probability = sqlite3_column_double (statement, col++);
		object.transform.position.x = sqlite3_column_double (statement, col++);
		object.transform.position.y = sqlite3_column_double (statement, col++);
		object.transform.position.z = sqlite3_column_double (statement, col++);
		object.transform.rotation.x = sqlite3_column_double (statement, col++);
		object.transform.rotation.y = sqlite3_column_double (statement, col++);
		object.transform.rotation.z = sqlite3_column_double (statement, col++);
		object.transform.rotation.w = sqlite3_column_double (statement, col++);
		if (object.probability <= 0.0f || object.probability > 1.0f)
			object.probability = 1.0f;
		object.transform.rotation = limat_quaternion_normalize (object.transform.rotation);

		/* Read type column. */
		object.type = (char*) sqlite3_column_text (statement, col);
		size = sqlite3_column_bytes (statement, col++);
		if (size > 0 && object.type != NULL)
			object.type = listr_dup (object.type);
		else
			object.type = listr_dup ("");
		if (object.type == NULL)
			goto error;

		/* Read model column. */
		object.model = (char*) sqlite3_column_text (statement, col);
		size = sqlite3_column_bytes (statement, col++);
		if (size > 0 && object.model != NULL)
			object.model = listr_dup (object.model);
		else
			object.model = listr_dup ("");
		if (object.model == NULL)
		{
			lisys_free (object.type);
			goto error;
		}

		/* Read extra column. */
		object.extra = (char*) sqlite3_column_text (statement, col);
		size = sqlite3_column_bytes (statement, col);
		if (size > 0 && object.extra != NULL)
			object.extra = listr_dup (object.extra);
		else
			object.extra = listr_dup ("");
		if (object.extra == NULL)
		{
			lisys_free (object.type);
			lisys_free (object.model);
			goto error;
		}

		/* Add to list. */
		if (object.id >= objects.count)
		{
			tmp = realloc (objects.array, (object.id + 1) * sizeof (ligenBrushobject*));
			if (tmp == NULL)
			{
				lisys_free (object.type);
				lisys_free (object.model);
				lisys_free (object.extra);
				goto error;
			}
			for (i = objects.count ; i < object.id ; i++)
				tmp[i] = NULL;
			objects.array = tmp;
			objects.count = object.id + 1;
		}
		objects.array[object.id] = lisys_malloc (sizeof (ligenBrushobject));
		if (objects.array[object.id] == NULL)
		{
			lisys_free (object.type);
			lisys_free (object.model);
			lisys_free (object.extra);
			goto error;
		}
		*(objects.array[object.id]) = object;
	}

	/* TODO: Remove any NULL objects. */

	/* Use new objects. */
	for (i = 0 ; i < self->objects.count ; i++)
	{
		lisys_free (self->objects.array[i]->type);
		lisys_free (self->objects.array[i]->model);
		lisys_free (self->objects.array[i]->extra);
		lisys_free (self->objects.array[i]);
	}
	lisys_free (self->objects.array);
	self->objects.count = objects.count;
	self->objects.array = objects.array;

	return 1;

error:
	for (i = 0 ; i < objects.count ; i++)
	{
		if (objects.array[i] != NULL)
		{
			lisys_free (objects.array[i]->type);
			lisys_free (objects.array[i]->extra);
			lisys_free (objects.array[i]);
		}
	}
	lisys_free (objects.array);
	sqlite3_finalize (statement);
	return 0;
}

static int
private_read_rules (ligenBrush* self,
                    liarcSql*   sql)
{
	int i;
	int col;
	int ret;
	int size;
	int flags;
	int ruleid;
	int strokeid;
	int paintid;
	int pos[3];
	char* name;
	const char* query;
	ligenRule* rule;
	ligenRule** tmp;
	sqlite3_stmt* statement;
	struct
	{
		int count;
		ligenRule** array;
	} rules = { 0, NULL };

	/* Prepare statement. */
	query = "SELECT id,flags,name FROM generator_rules WHERE brush=?;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}

	/* Read rules. */
	for (ret = sqlite3_step (statement) ; ret != SQLITE_DONE ; ret = sqlite3_step (statement))
	{
		/* Check for errors. */
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Read id and flags. */
		col = 0;
		ruleid = sqlite3_column_int (statement, col++);
		flags = sqlite3_column_int (statement, col++);
		if (ruleid < 0)
			continue;

		/* Read name column. */
		name = (char*) sqlite3_column_text (statement, col);
		size = sqlite3_column_bytes (statement, col);
		if (size > 0 && name != NULL)
		{
			name = listr_dup (name);
			if (name == NULL)
				goto error;
		}
		else
			name = NULL;

		/* Create new rule. */
		rule = ligen_rule_new ();
		if (rule == NULL)
			goto error;
		rule->id = ruleid;
		if (name != NULL)
		{
			lisys_free (rule->name);
			rule->name = name;
		}

		/* Add to list. */
		if (ruleid >= rules.count)
		{
			tmp = realloc (rules.array, (ruleid + 1) * sizeof (ligenRule*));
			if (tmp == NULL)
				goto error;
			for (i = rules.count ; i < ruleid ; i++)
				tmp[i] = NULL;
			rules.array = tmp;
			rules.count = ruleid + 1;
		}
		rules.array[ruleid] = rule;
	}

	/* Prepare statement. */
	query = "SELECT id,rule,paint,x,y,z,flags FROM generator_strokes WHERE "
		"brush=? ORDER BY id;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}

	/* Read strokes. */
	for (ret = sqlite3_step (statement) ; ret != SQLITE_DONE ; ret = sqlite3_step (statement))
	{
		/* Check for errors. */
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Read columns. */
		col = 0;
		strokeid = sqlite3_column_int (statement, col++);
		ruleid = sqlite3_column_int (statement, col++);
		paintid = sqlite3_column_int (statement, col++);
		pos[0] = sqlite3_column_int (statement, col++);
		pos[1] = sqlite3_column_int (statement, col++);
		pos[2] = sqlite3_column_int (statement, col++);
		flags = sqlite3_column_int (statement, col++);
		if (ruleid < 0 || strokeid < 0 || paintid < 0)
			continue;
		if (ruleid >= rules.count)
			continue;
		if (ligen_generator_find_brush (self->generator, paintid) == NULL)
			continue;

		/* Insert stroke. */
		ligen_rule_insert_stroke (rules.array[ruleid], pos[0], pos[1], pos[2], flags, paintid);
	}

	/* TODO: Remove any NULL rules. */

	/* Use new rules. */
	for (i = 0 ; i < self->rules.count ; i++)
		lisys_free (self->rules.array[i]);
	lisys_free (self->rules.array);
	self->rules.count = rules.count;
	self->rules.array = rules.array;

	return 1;

error:
	for (i = 0 ; i < rules.count ; i++)
	{
		if (rules.array[i] != NULL)
			ligen_rule_free (rules.array[i]);
	}
	lisys_free (rules.array);
	sqlite3_finalize (statement);
	return 0;
}

static int
private_write_object (ligenBrush*       self,
                      ligenBrushobject* object,
                      liarcSql*         sql)
{
	return liarc_sql_replace (sql, "generator_objects",
		"id", LIARC_SQL_INT, object->id,
		"brush", LIARC_SQL_INT, self->id,
		"flags", LIARC_SQL_INT, object->flags,
		"prob", LIARC_SQL_FLOAT, object->probability,
		"posx", LIARC_SQL_FLOAT, object->transform.position.x,
		"posy", LIARC_SQL_FLOAT, object->transform.position.y,
		"posz", LIARC_SQL_FLOAT, object->transform.position.z,
		"rotx", LIARC_SQL_FLOAT, object->transform.rotation.x,
		"roty", LIARC_SQL_FLOAT, object->transform.rotation.y,
		"rotz", LIARC_SQL_FLOAT, object->transform.rotation.z,
		"rotw", LIARC_SQL_FLOAT, object->transform.rotation.w,
		"type", LIARC_SQL_TEXT, object->type,
		"model", LIARC_SQL_TEXT, object->model,
		"extra", LIARC_SQL_TEXT, object->extra, NULL);
}

static int
private_write_rule (ligenBrush* self,
                    ligenRule*  rule,
                    liarcSql*   sql)
{
	int i;

	/* Write rule. */
	if (!liarc_sql_replace (sql, "generator_rules",
		"id", LIARC_SQL_INT, rule->id,
		"brush", LIARC_SQL_INT, self->id,
		"name", LIARC_SQL_TEXT, rule->name,
		"flags", LIARC_SQL_INT, rule->flags, NULL))
		return 0;

	/* Write strokes. */
	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		if (!private_write_stroke (self, rule, rule->strokes.array + i, i, sql))
			return 0;
	}

	return 1;
}

static int
private_write_stroke (ligenBrush*      self,
                      ligenRule*       rule,
                      ligenRulestroke* stroke,
                      int              id,
                      liarcSql*        sql)
{
	return liarc_sql_replace (sql, "generator_strokes",
		"id", LIARC_SQL_INT, id,
		"brush", LIARC_SQL_INT, self->id,
		"rule", LIARC_SQL_INT, rule->id,
		"paint", LIARC_SQL_INT, stroke->brush,
		"x", LIARC_SQL_INT, stroke->pos[0],
		"y", LIARC_SQL_INT, stroke->pos[1],
		"z", LIARC_SQL_INT, stroke->pos[2],
		"flags", LIARC_SQL_INT, stroke->flags, NULL);
}

/** @} */
/** @} */
