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
 * \addtogroup liengResources Resources
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "engine-resources.h"

static int
private_compare_animations (const void* a,
                            const void* b);

static int
private_compare_models (const void* a,
                        const void* b);

static int
private_compare_samples (const void* a,
                         const void* b);

static inline int
private_filter_models (const char* dir,
                       const char* name);

static inline int
private_filter_samples (const char* dir,
                        const char* name);

static int
private_insert_model (liengResources*   self,
                      const char*       name,
                      const limdlModel* model);

static int
private_insert_sample (liengResources* self,
                       const char*     name);

/*****************************************************************************/

/**
 * \brief Creates a new resource manager.
 *
 * The resource list is left empty after creation. The appropriate function
 * for loading the resources needs to be called manually.
 *
 * \param engine Engine.
 * \return Resource manager or NULL.
 */
liengResources*
lieng_resources_new (liengEngine* engine)
{
	liengResources* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liengResources));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->engine = engine;

	return self;
}

/**
 * \brief Frees the resource manager.
 *
 * \param self Resources.
 */
void
lieng_resources_free (liengResources* self)
{
	int i;
	liengAnimation* animation;
	liengModel* model;
	liengSample* sample;

	/* Free animations. */
	if (self->animations.array != NULL)
	{
		for (i = 0 ; i < self->animations.count ; i++)
		{
			animation = self->animations.array + i;
			free (animation->name);
		}
		free (self->animations.array);
	}

	/* Free models. */
	if (self->models.array != NULL)
	{
		for (i = 0 ; i < self->models.count ; i++)
		{
			model = self->models.array[i];
			lieng_model_free (model);
		}
		free (self->models.array);
	}

	/* Free samples. */
	if (self->samples.array != NULL)
	{
		for (i = 0 ; i < self->samples.count ; i++)
		{
			sample = self->samples.array + i;
			free (sample->name);
		}
		free (self->samples.array);
	}

	free (self);
}

/**
 * \brief Finds an animation by ID.
 *
 * \param self Resources.
 * \param id ID.
 * \return Animation or NULL.
 */
liengAnimation*
lieng_resources_find_animation_by_code (liengResources* self,
                                        int             id)
{
	if (id >= self->animations.count)
		return NULL;
	return self->animations.array + id;
}

/**
 * \brief Finds an animation by name.
 *
 * \param self Resources.
 * \param name Name.
 * \return Animation or NULL.
 */
liengAnimation*
lieng_resources_find_animation_by_name (liengResources* self,
                                        const char*     name)
{
	liengAnimation tmp;

	tmp.name = (char*) name;
	return bsearch (&tmp, self->animations.array, self->animations.count,
		sizeof (liengAnimation), private_compare_animations);
}

/**
 * \brief Finds a model by ID.
 *
 * \param self Resources.
 * \param id ID.
 * \return Model or NULL.
 */
liengModel*
lieng_resources_find_model_by_code (liengResources* self,
                                    int             id)
{
	if (id >= self->models.count)
		return NULL;
	return self->models.array[id];
}

/**
 * \brief Finds a model by name.
 *
 * \param self Resources.
 * \param name Name.
 * \return Model or NULL.
 */
liengModel*
lieng_resources_find_model_by_name (liengResources* self,
                                    const char*     name)
{
	liengModel tmp;
	liengModel* ptr;
	liengModel** ret;

	tmp.name = (char*) name;
	ptr = &tmp;
	ret = bsearch (&ptr, self->models.array, self->models.count,
		sizeof (liengModel*), private_compare_models);
	if (ret == NULL)
		return NULL;

	return *ret;
}

/**
 * \brief Finds a sample by ID.
 *
 * \param self Resources.
 * \param id ID.
 * \return Sample or NULL.
 */
liengSample*
lieng_resources_find_sample_by_code (liengResources* self,
                                     int             id)
{
	if (id >= self->samples.count)
		return NULL;
	return self->samples.array + id;
}

/**
 * \brief Finds a sample by name.
 *
 * \param self Resources.
 * \param name Name.
 * \return Sample or NULL.
 */
liengSample*
lieng_resources_find_sample_by_name (liengResources* self,
                                     const char*     name)
{
	liengSample tmp;

	tmp.name = (char*) name;
	return bsearch (&tmp, self->samples.array, self->samples.count,
		sizeof (liengSample), private_compare_samples);
}

/**
 * \brief Reloads the resource list by iterating through all the data files.
 *
 * \param self Resources.
 * \param path Path to data directory root.
 * \return Nonzero on success.
 */
int
lieng_resources_load_from_dir (liengResources* self,
                               const char*     path)
{
	int i;
	int ret;
	int count;
	char* file;
	char* name;
	char* tmp;
	limdlModel* model = NULL;
	lisysDir* directory = NULL;

	/* Find all models. */
	tmp = lisys_path_concat (path, "graphics", NULL);
	if (tmp == NULL)
		return 0;
	directory = lisys_dir_open (tmp);
	free (tmp);
	if (directory == NULL)
		return 0;
	lisys_dir_set_filter (directory, private_filter_models);
	lisys_dir_set_sorter (directory, LISYS_DIR_SORTER_ALPHA);
	if (!lisys_dir_scan (directory))
	{
		lisys_dir_free (directory);
		return 0;
	}
	count = lisys_dir_get_count (directory);

	/* Create model list. */
	for (i = 0 ; i < count ; i++)
	{
		/* Open model file. */
		file = lisys_dir_get_path (directory, i);
		if (file == NULL)
			goto error;
		model = limdl_model_new_from_file (file);
		free (file);
		if (model == NULL)
			goto error;

		/* Add to list. */
		name = lisys_path_format (lisys_dir_get_name (directory, i), LISYS_PATH_STRIPEXT, NULL);
		if (name == NULL)
			goto error;
		ret = private_insert_model (self, name, model);
		free (name);
		if (!ret)
			goto error;

		/* Free the model. */
		limdl_model_free (model);
		model = NULL;
	}

	/* Find all samples. */
	tmp = lisys_path_concat (path, "sounds", NULL);
	if (tmp == NULL)
		return 0;
	directory = lisys_dir_open (tmp);
	free (tmp);
	if (directory == NULL)
		return 0;
	lisys_dir_set_filter (directory, private_filter_samples);
	lisys_dir_set_sorter (directory, LISYS_DIR_SORTER_ALPHA);
	if (!lisys_dir_scan (directory))
	{
		lisys_dir_free (directory);
		return 0;
	}
	count = lisys_dir_get_count (directory);

	/* Create sample list. */
	for (i = 0 ; i < count ; i++)
	{
		file = lisys_dir_get_path (directory, i);
		if (file == NULL)
			goto error;
		name = lisys_path_format (lisys_dir_get_name (directory, i), LISYS_PATH_STRIPEXT, NULL);
		if (name == NULL)
			goto error;
		ret = private_insert_sample (self, name);
		free (name);
		if (!ret)
			goto error;
	}

	/* Sort loaded data. */
	qsort (self->animations.array, self->animations.count,
		sizeof (liengAnimation), private_compare_animations);
	qsort (self->models.array, self->models.count,
		sizeof (liengModel*), private_compare_models);
	qsort (self->models.array, self->samples.count,
		sizeof (liengSample*), private_compare_samples);
	for (i = 0 ; i < self->animations.count ; i++)
		self->animations.array[i].id = i;
	for (i = 0 ; i < self->models.count ; i++)
		self->models.array[i]->id = i;
	for (i = 0 ; i < self->samples.count ; i++)
		self->samples.array[i].id = i;
	lisys_dir_free (directory);

	return 1;

error:
	if (model != NULL)
		limdl_model_free (model);
	lisys_dir_free (directory);
	return 0;
}

/**
 * \brief Reloads the resource list from a stream.
 *
 * \param self Resources.
 * \param reader Stream.
 * \return Nonzero on success.
 */
int
lieng_resources_load_from_stream (liengResources* self,
                                  liReader*       reader)
{
	int id;
	char* name;
	uint32_t n_animations;
	uint32_t n_models;
	liengAnimation* animation;
	liengModel* model;

	/* Read the header. */
	if (!li_reader_get_uint32 (reader, &n_animations) ||
	    !li_reader_get_uint32 (reader, &n_models))
	{
		lisys_error_set (EINVAL, "invalid resource list header");
		goto error;
	}

	/* Read animations. */
	if (n_animations)
	{
		self->animations.count = n_animations;
		self->animations.array = calloc (n_animations, sizeof (liengAnimation));
		if (self->animations.array == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		for (id = 0 ; id < n_animations ; id++)
		{
			animation = self->animations.array + id;
			animation->id = id;
			if (!li_reader_get_text (reader, "", &animation->name))
				goto error;
		}
	}

	/* Read models. */
	if (n_models)
	{
		self->models.count = n_models;
		self->models.array = calloc (n_models, sizeof (liengModel*));
		if (self->models.array == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		for (id = 0 ; id < n_models ; id++)
		{
			if (!li_reader_get_text (reader, "", &name))
				goto error;
			model = lieng_model_new (self->engine, id, self->engine->config.dir, name);
			free (name);
			if (model == NULL)
				goto error;
			self->models.array[id] = model;
		}
	}

	/* Read end. */
	if (!li_reader_check_end (reader))
	{
		lisys_error_set (EINVAL, "end of stream expected");
		goto error;
	}

	return 1;

error:
	return 0;
}

/**
 * \brief Gets the total number of animations.
 *
 * \param self Resources.
 * \return Number of animations.
 */
int
lieng_resources_get_animation_count (liengResources* self)
{
	return self->animations.count;
}

/**
 * \brief Gets the total number of models.
 *
 * \param self Resources.
 * \return Number of models.
 */
int
lieng_resources_get_model_count (liengResources* self)
{
	return self->models.count;
}

/*****************************************************************************/

static int
private_compare_animations (const void* a,
                            const void* b)
{
	const liengAnimation* aa = a;
	const liengAnimation* bb = b;

	return strcmp (aa->name, bb->name);
}

static int
private_compare_models (const void* a,
                        const void* b)
{
	const liengModel* const* aa = a;
	const liengModel* const* bb = b;

	return strcmp ((*aa)->name, (*bb)->name);
}

static int
private_compare_samples (const void* a,
                         const void* b)
{
	const liengSample* aa = a;
	const liengSample* bb = b;

	return strcmp (aa->name, bb->name);
}

static inline int
private_filter_models (const char* dir,
                       const char* name)
{
	const char* ptr;

	ptr = strstr (name, ".lmdl");
	if (ptr == NULL)
		return 0;
	if (strcmp (ptr, ".lmdl"))
		return 0;
	return 1;
}

static inline int
private_filter_samples (const char* dir,
                        const char* name)
{
	const char* ptr;

	ptr = strstr (name, ".ogg");
	if (ptr == NULL)
		return 0;
	if (strcmp (ptr, ".ogg"))
		return 0;
	return 1;
}

static int
private_insert_model (liengResources*   self,
                      const char*       name,
                      const limdlModel* model)
{
	int i;
	int j;
	int count;
	liengModel* tmp;
	liengAnimation* eanim;
	limdlAnimation* manim;

	/* Create new model. */
	tmp = lieng_model_new (self->engine, 0, self->engine->config.dir, name);
	if (tmp == NULL)
		return 0;
	tmp->bounds = model->bounds;
	if (!lialg_array_append (&self->models, &tmp))
	{
		lisys_error_set (ENOMEM, NULL);
		lieng_model_free (tmp);
		return 0;
	}

	/* Cache animations. */
	for (i = 0 ; i < model->animation.count ; i++)
	{
		manim = model->animation.animations + i;
		count = self->animations.count;

		/* Check for duplicates. */
		for (j = 0 ; j < count ; j++)
		{
			eanim = self->animations.array + j;
			if (!strcmp (manim->name, eanim->name))
				break;
		}
		if (j != count)
			continue;

		/* Create new animation. */
		if (!lialg_array_resize (&self->animations, count + 1))
			return 0;
		eanim = self->animations.array + count;
		eanim->id = 0;
		eanim->data = NULL;
		eanim->name = strdup (manim->name);
		if (eanim->name == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			self->animations.count--;
			return 0;
		}
	}

	return 1;
}

static int
private_insert_sample (liengResources* self,
                       const char*     name)
{
	int j;
	liengSample* sample;

	/* Check for duplicates. */
	for (j = 0 ; j < self->samples.count ; j++)
	{
		sample = self->samples.array + j;
		if (!strcmp (sample->name, name))
			return 1;
	}

	/* Create new sample. */
	if (!lialg_array_resize (&self->samples, self->samples.count + 1))
		return 0;
	sample = self->samples.array + self->samples.count - 1;
	sample->id = 0;
	sample->data = NULL;
	sample->name = strdup (name);
	if (sample->name == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		self->samples.count--;
		return 0;
	}

	return 1;
}

/** @} */
/** @} */
