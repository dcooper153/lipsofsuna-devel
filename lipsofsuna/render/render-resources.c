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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndResources Resources
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "render-resources.h"

/**
 * \brief Creates a new resource manager.
 *
 * \param self Renderer.
 * \return New resource manager or NULL.
 */
lirndResources*
lirnd_resources_new (lirndRender* render)
{
	lirndResources* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndResources));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->images = lialg_strdic_new ();
	if (self->images == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->models = lialg_strdic_new ();
	if (self->models == NULL)
	{
		lialg_strdic_free (self->images);
		lisys_free (self);
		return NULL;
	}
	self->shaders = lialg_strdic_new ();
	if (self->shaders == NULL)
	{
		lialg_strdic_free (self->models);
		lialg_strdic_free (self->images);
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the resources.
 *
 * \param self Resources.
 */
void
lirnd_resources_free (lirndResources* self)
{
	lialgStrdicIter iter;
	lirndImage* image;
	lirndModel* model;
	lirndShader* shader;

	/* Free shaders. */
	if (self->shaders != NULL)
	{
		LI_FOREACH_STRDIC (iter, self->shaders)
		{
			shader = iter.value;
			lirnd_shader_free (shader);
		}
		lialg_strdic_free (self->shaders);
	}

	/* Free models. */
	if (self->models != NULL)
	{
		assert (self->models->size == 0);
		LI_FOREACH_STRDIC (iter, self->models)
		{
			model = iter.value;
			lirnd_model_free (model);
		}
		lialg_strdic_free (self->models);
	}

	/* Free images. */
	if (self->images != NULL)
	{
		LI_FOREACH_STRDIC (iter, self->images)
		{
			image = iter.value;
			if (image->texture != NULL)
				liimg_texture_free (image->texture);
			lisys_free (image->name);
			lisys_free (image->path);
			lisys_free (image);
		}
		lialg_strdic_free (self->images);
	}

	lisys_free (self);
}

/**
 * \brief Finds an image by name.
 *
 * \param self Resources.
 * \param name Name.
 * \return Image or NULL.
 */
lirndImage*
lirnd_resources_find_image (lirndResources* self,
                            const char*     name)
{
	return lialg_strdic_find (self->images, name);
}

/**
 * \brief Finds a model by name.
 *
 * \param self Resources.
 * \param name Name.
 * \return Model or NULL.
 */
lirndModel*
lirnd_resources_find_model (lirndResources* self,
                            const char*     name)
{
	return lialg_strdic_find (self->models, name);
}

/**
 * \brief Finds a shader by name.
 *
 * \param self Resources.
 * \param name Name.
 * \return Shader or NULL.
 */
lirndShader*
lirnd_resources_find_shader (lirndResources* self,
                             const char*     name)
{
	char* path;
	lirndShader* shader;

	/* Try existing. */
	shader = lialg_strdic_find (self->shaders, name);
	if (shader != NULL)
		return shader;

	/* Try loading. */
	path = lisys_path_format (self->render->config.dir,
		LISYS_PATH_SEPARATOR, "shaders",
		LISYS_PATH_SEPARATOR, name, NULL);
	if (path == NULL)
		return NULL;
	shader = lirnd_shader_new_from_file (self->render, path);
	lisys_free (path);

	/* Try fallback. */
	if (shader == NULL)
		shader = lirnd_shader_new (self->render);
	if (shader == NULL)
		return NULL;

	/* Insert to dictionary. */
	shader->name = listr_dup (name);
	if (shader->name == NULL)
	{
		lirnd_shader_free (shader);
		return NULL;
	}
	if (!lialg_strdic_insert (self->shaders, name, shader))
	{
		lirnd_shader_free (shader);
		return NULL;
	}

	return shader;
}

/**
 * \brief Inserts an image to the resource list.
 *
 * \param self Resources.
 * \param name Image name.
 * \return Image or NULL.
 */
lirndImage*
lirnd_resources_insert_image (lirndResources* self,
                              const char*     name)
{
	lirndImage* image;

	image = lisys_calloc (1, sizeof (lirndImage));
	if (image == NULL)
		return NULL;
	image->name = listr_dup (name);
	image->path = lisys_path_format (self->render->config.dir,
		LISYS_PATH_SEPARATOR, "graphics",
		LISYS_PATH_SEPARATOR, name, ".dds", NULL);
	if (image->name == NULL || image->path == NULL)
	{
		lisys_free (image->name);
		lisys_free (image);
		return NULL;
	}
	if (!lialg_strdic_insert (self->images, name, image))
	{
		lisys_free (image->name);
		lisys_free (image->path);
		lisys_free (image);
		return NULL;
	}

	return image;
}

/**
 * \brief Inserts a model to the resource list.
 *
 * \param self Resources.
 * \param name Model name.
 * \param model Model data.
 * \return Model or NULL.
 */
lirndModel*
lirnd_resources_insert_model (lirndResources* self,
                              const char*     name,
                              limdlModel*     model)
{
	lirndModel* model_;

	model_ = lirnd_model_new (self->render, model);
	if (model_ == NULL)
		return NULL;
	if (!lialg_strdic_insert (self->models, name, model_))
	{
		lirnd_model_free (model_);
		return NULL;
	}

	return model_;
}

/**
 * \brief Removes a model from the resource list.
 *
 * \param self Resources.
 * \param name Model name.
 */
void
lirnd_resources_remove_model (lirndResources* self,
                              const char*     name)
{
	lirndModel* model;

	model = lialg_strdic_find (self->models, name);
	if (model != NULL)
	{
		lialg_strdic_remove (self->models, name);
		lirnd_model_free (model);
	}
}

int
lirnd_resources_get_image_count (lirndResources* self)
{
	return self->images->size;
}

int
lirnd_resources_get_shader_count (lirndResources* self)
{
	return self->shaders->size;
}

/** @} */
/** @} */
