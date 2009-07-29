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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#include "ext-generator.h"

static int
private_object_count (liextGenerator* self,
                      liengObject*    root);

static int
private_object_expand (liextGenerator* self,
                       liengObject*    root);

static int
private_pattern_complete (liextGenerator* self,
                          liextMatch*     match);

static int
private_pattern_intersects (liextGenerator* self,
                            liextMatch*     match);

/*****************************************************************************/

liextGenerator*
liext_generator_new (licliModule* module)
{
	liextGenerator* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liextGenerator));
	if (self == NULL)
		return self;
	self->module = module;

	/* Allocate preview scene. */
	self->scene = lirnd_scene_new (module->engine->render);
	if (self->scene == NULL)
	{
		free (self);
		return NULL;
	}

	/* Allocate generator. */
	self->generator = ligen_generator_new_full (module->paths->root,
		module->name, self->scene, module->engine->renderapi);
	if (self->generator == NULL)
	{
		lirnd_scene_free (self->scene);
		free (self);
		return NULL;
	}
	ligen_generator_set_fill (self->generator, 0);

	return self;
}

void
liext_generator_free (liextGenerator* self)
{
	int i;

	if (self->generator != NULL)
		ligen_generator_free (self->generator);
	if (self->scene != NULL)
		lirnd_scene_free (self->scene);

	/* Free rules. */
	for (i = 0 ; i < self->rules.count ; i++)
		liext_rule_free (self->rules.array[i]);
	free (self->rules.array);
	self->rules.count = 0;
	self->rules.array = NULL;

	free (self);
}

int
liext_generator_create_object (liextGenerator*       self,
                               liengModel*           model,
                               const limatTransform* transform)
{
	liarcWriter* writer;

	writer = liarc_writer_new_packet (LINET_EXT_CLIENT_PACKET_GENERATOR);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint8 (writer, LINET_EXT_GENERATOR_PACKET_CREATE);
	liarc_writer_append_uint32 (writer, model->id);
	liarc_writer_append_float (writer, transform->position.x);
	liarc_writer_append_float (writer, transform->position.y);
	liarc_writer_append_float (writer, transform->position.z);
	liarc_writer_append_float (writer, transform->rotation.x);
	liarc_writer_append_float (writer, transform->rotation.y);
	liarc_writer_append_float (writer, transform->rotation.z);
	liarc_writer_append_float (writer, transform->rotation.w);
	licli_module_send (self->module, writer, GRAPPLE_RELIABLE);
	liarc_writer_free (writer);

	return 1;
}

/**
 * \brief Finds a rule that matches the selection exactly.
 *
 * \param self Generator.
 * \return Rule number or -1.
 */
int
liext_generator_find_rule (liextGenerator* self)
{
	int i;
	int j;
	int found;
	lialgPtrdicIter iter;
	liengSelection* selection;
	liengObject* object;
	liengObject* root;
	liextRule* rule;

	/* Choose any object as root. */
	if (self->module->engine->selection->size < 1)
		return -1;
	selection = self->module->engine->selection->list->value;
	root = selection->object;

	/* Check against each rule. */
	for (i = 0 ; i < self->rules.count ; i++)
	{
		rule = self->rules.array[i];
		if (self->module->engine->selection->size != rule->lines.count)
			continue;

		/* Check against each possible permutation. */
		for (j = 0 ; j < rule->lines.count ; j++)
		{
			/* Check for valid root type. */
			if (rule->lines.array[j].model != root->model)
				continue;

			/* Compare rule to all objects. */
			found = 1;
			LI_FOREACH_PTRDIC (iter, self->module->engine->selection)
			{
				selection = iter.value;
				object = selection->object;
				if (liext_rule_matches (rule, j, root, object) == -1)
				{
					found = 0;
					break;
				}
			}
			if (found)
				return i;
		}
	}

	return -1;
}

/**
 * \brief Finds a partial match for the rule.
 *
 * Takes a rule number, a rule line number, and an object matching the
 * rule and the line and finds all other objects matching the rule.
 *
 * \param self Generator.
 * \param rule Rule number.
 * \param line Rule line the root object matches.
 * \param root Root object.
 * \return Match or NULL.
 */
liextMatch*
liext_generator_find_match (liextGenerator* self,
                            int             rule,
                            int             line,
                            liengObject*    root)
{
	int line_;
	lialgU32dicIter iter;
	liengObject* object;
	liextMatch* match;
	liextRule* rule_;

	/* Create match data. */
	rule_ = self->rules.array[rule];
	match = liext_match_new (root, rule_, line);
	if (match == NULL)
		return NULL;

	/* Match each object. */
	LI_FOREACH_U32DIC (iter, self->module->engine->objects)
	{
		object = iter.value;
		line_ = liext_rule_matches (rule_, line, root, object);
		if (line_ != -1)
		{
			if (match->objects.array[line_] == NULL)
				match->matches.count++;
			match->objects.array[line_] = object;
		}
	}

	/* Calculate match completeness. */
	match->matches.fraction = (float) match->matches.count / rule_->lines.count;

	return match;
}

/**
 * \brief Creates a new rule from the selection.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
liext_generator_insert_rule (liextGenerator* self)
{
	lialgPtrdicIter iter;
	liengObject* object;
	liengSelection* selection;
	liextRule* rule;
	limatTransform transform;

	if (self->module->engine->selection->size <= 1)
		return 1;

	/* Create new rule. */
	rule = liext_rule_new ();
	if (rule == NULL)
		return 0;

	/* Add selected objects to the rule. */
	LI_FOREACH_PTRDIC (iter, self->module->engine->selection)
	{
		selection = iter.value;
		object = selection->object;
		transform = selection->transform;
		if (!liext_rule_create_line (rule, &transform, object->model))
		{
			liext_rule_free (rule);
			return 0;
		}
	}

	/* Add to the rule list. */
	if (!lialg_array_append (&self->rules, &rule))
	{
		liext_rule_free (rule);
		return 0;
	}

	return 1;
}

/**
 * \brief Loads the generator rules.
 *
 * If the `path' is NULL, the rules are loaded from the default generator file
 * at `config/generator.dat'. Otherwise, the file specified by `path' is used.
 *
 * \param self Generator.
 * \param path Path or NULL.
 * \return Nonzero on success.
 */
int
liext_generator_load (liextGenerator* self,
                      const char*     path)
{
#warning Reloading generator data not implemented.
	return 1;
}

void
liext_generator_remove_rule (liextGenerator* self,
                             int             rule)
{
	liext_rule_free (self->rules.array[rule]);
	lialg_array_remove (&self->rules, rule);
}

int
liext_generator_run (liextGenerator* self)
{
	int i;
	int j;
	int count;
	lialgU32dicIter iter;
	liengObject* object;
	liengObject** objects;

	if (!self->module->engine->objects->size)
	{
		lisys_error_set (EINVAL, "there are no objects");
		return 0;
	}

	/* Randomize object list. */
	/* FIXME: Allocating and freeing continuously is inefficient. */
	count = self->module->engine->objects->size;
	objects = calloc (count, sizeof (liengObject*));
	if (objects == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	i = 0;
	LI_FOREACH_U32DIC (iter, self->module->engine->objects)
		objects[i++] = iter.value;
	for (i = 0 ; i < count ; i++)
	{
		j = rand () % count;
		object = objects[i];
		objects[i] = objects[j];
		objects[j] = object;
	}

	/* Try to expand each object. */
	for (i = 0 ; i < count ; i++)
	{
		if (private_object_expand (self, objects[i]))
			break;
	}
	free (objects);

	return i != count;
}

/**
 * \brief Saves the generator rules.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
liext_generator_save (liextGenerator* self)
{
	return ligen_generator_write_brushes (self->generator);
}

/**
 * \brief Calculates statistics for the current scene.
 *
 * This operation shouldn't be performed too frequently because
 * it's rather expensive.
 *
 * \param self Generator.
 * \param value Return location for statistics.
 * \return Nonzero on success.
 */
void
liext_generator_get_statistics (liextGenerator*  self,
                                liextStatistics* value)
{
	lialgU32dicIter iter;

	/* Check if selection matches a rule. */
	value->selected_rule = liext_generator_find_rule (self);

	/* Count known rules. */
	value->rules_known = self->rules.count;

	/* Count expandable rules. */
	value->rules_expand = 0;
	LI_FOREACH_U32DIC (iter, self->module->engine->objects)
		value->rules_expand += private_object_count (self, iter.value);
}

/*****************************************************************************/

static int
private_object_count (liextGenerator* self,
                      liengObject*    root)
{
	int i;
	int j;
	int count = 0;
	liextRule* rule;
	liextMatch* match;

	/* Find all expandable rules. */
	for (i = 0 ; i < self->rules.count ; i++)
	{
		rule = self->rules.array[i];
		for (j = 0 ; j < rule->lines.count ; j++)
		{
			if (rule->lines.array[j].model != root->model)
				continue;
			match = liext_generator_find_match (self, i, j, root);
			if (match == NULL)
				continue;
			if (match->matches.count < 1 ||
			    match->matches.count == rule->lines.count)
			{
				liext_match_free (match);
				continue;
			}
			if (!private_pattern_intersects (self, match))
				count++;
			liext_match_free (match);
		}
	}

	return count;
}

static int
private_object_expand (liextGenerator* self,
                       liengObject*    root)
{
	int i;
	int j;
	int found = 0;
	liextRule* rule;
	liextMatch* match;
	struct { int count; liextMatch** array; } matches = { 0, NULL };

	/* Find all matching rules. */
	for (i = 0 ; i < self->rules.count ; i++)
	{
		rule = self->rules.array[i];
		for (j = 0 ; j < rule->lines.count ; j++)
		{
			if (rule->lines.array[j].model != root->model)
				continue;
			match = liext_generator_find_match (self, i, j, root);
			if (match == NULL)
				continue;
			if (match->matches.count < 1 ||
			    match->matches.count == rule->lines.count)
			{
				liext_match_free (match);
				continue;
			}
			if (!lialg_array_append (&matches, &match))
			{
				liext_match_free (match);
				continue;
			}
		}
	}

	/* Randomize the order of matches. */
	/* FIXME: Should take rule weights into account. */
	for (i = 0 ; i < matches.count ; i++)
	{
		j = rand () % matches.count;
		match = matches.array[i];
		matches.array[i] = matches.array[j];
		matches.array[j] = match;
	}

	/* Try each matching pattern until one fits. */
	for (i = 0 ; i < matches.count ; i++)
	{
		if (private_pattern_complete (self, matches.array[i]))
		{
			found = 1;
			break;
		}
	}

	/* Free matches. */
	for (i = 0 ; i < matches.count ; i++)
		liext_match_free (matches.array[i]);
	free (matches.array);

	return found;
}

static int
private_pattern_complete (liextGenerator* self,
                          liextMatch*     match)
{
	int i;
	int error = 0;
	liengModel* model;
	liengObject* object;
	limatTransform transform0;
	limatTransform transform1;

	/* Check for unwanted intersections. */
	if (private_pattern_intersects (self, match))
		return 0;

	/* Create objects. */
	for (i = 0 ; i < match->objects.count ; i++)
	{
		/* Get missing object. */
		object = match->objects.array[i];
		if (object != NULL)
			continue;

		/* Get object orientation. */
		liext_rule_get_transform (match->rule, match->line, i, &transform0);
		lieng_object_get_transform (match->root, &transform1);
		transform0 = limat_transform_multiply (transform1, transform0);
		model = match->rule->lines.array[i].model;

		/* Create object. */
		if (!liext_generator_create_object (self, model, &transform0))
		{
			error = 1;
			continue;
		}
	}

	return !error;
}

static int
private_pattern_intersects (liextGenerator* self,
                            liextMatch*     match)
{
	lialgU32dicIter iter;

	/* Test against each object. */
	/* TODO: Could be optimized with space partitioning. */
	LI_FOREACH_U32DIC (iter, self->module->engine->objects)
	{
		if (liext_match_intersects_object (match, iter.value))
			return 1;
	}

	return 0;
}

/** @} */
/** @} */
/** @} */
