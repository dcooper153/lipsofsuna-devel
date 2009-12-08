/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup lirndModel Model
 * @{
 */

#include <system/lips-system.h>
#include "render-model.h"

static void
private_clear_materials (lirndModel* self);

static void
private_clear_model (lirndModel* self);

static int
private_init_materials (lirndModel* self);

static int
private_init_model (lirndModel* self);

/*****************************************************************************/

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model buffer retains the ownership and must
 * ensure that the buffer is not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \return New model or NULL.
 */
lirndModel*
lirnd_model_new (lirndRender* render,
                 limdlModel*  model)
{
	lirndModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndModel));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->model = model;
	self->aabb = model->bounds;

	/* Initialize static mesh. */
	if (!private_init_materials (self) ||
	    !private_init_model (self))
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

lirndModel*
lirnd_model_new_instance (lirndModel* model)
{
	return lirnd_model_new (model->render, model->model);
}

/**
 * \brief Frees a model.
 *
 * \param self Model.
 */
void
lirnd_model_free (lirndModel* self)
{
	private_clear_materials (self);
	private_clear_model (self);
	lisys_free (self);
}

void
lirnd_model_replace_image (lirndModel* self,
                           lirndImage* image)
{
	int i;
	int j;
	lirndMaterial* material;
	lirndTexture* texture;

	for (i = 0 ; i < self->materials.count ; i++)
	{
		material = self->materials.array[i];
		for (j = 0 ; j < material->textures.count ; j++)
		{
			texture = material->textures.array + j;
			if (texture->image == image)
				lirnd_texture_set_image (texture, image);
		}
	}
}

void
lirnd_model_get_bounds (lirndModel* self,
                        limatAabb*  aabb)
{
	*aabb = self->aabb;
}

/**
 * \brief Checks if the model is static.
 *
 * \param self Model.
 * \return Nonzero if the model is static.
 */
int
lirnd_model_get_static (lirndModel* self)
{
	return !self->model->animations.count;
}

/*****************************************************************************/

static void
private_clear_materials (lirndModel* self)
{
	int i;

	for (i = 0 ; i < self->materials.count ; i++)
	{
		if (self->materials.array[i] != NULL)
			lirnd_material_free (self->materials.array[i]);
	}
	lisys_free (self->materials.array);
	self->materials.array = NULL;
	self->materials.count = 0;
}

static void
private_clear_model (lirndModel* self)
{
	int i;

	for (i = 0 ; i < self->buffers.count ; i++)
		lirnd_buffer_free (self->buffers.array + i);
	if (self->vertices != NULL)
	{
		lirnd_buffer_free (self->vertices);
		lisys_free (self->vertices);
		self->vertices = NULL;
	}
	lisys_free (self->buffers.array);
	self->buffers.array = NULL;
	self->buffers.count = 0;
}

static int
private_init_materials (lirndModel* self)
{
	uint32_t i;
	limdlMaterial* src;
	lirndMaterial* dst;

	/* Allocate materials. */
	self->materials.count = self->model->materials.count;
	if (self->materials.count)
	{
		self->materials.array = lisys_calloc (self->materials.count, sizeof (lirndMaterial*));
		if (self->materials.array == NULL)
			return 0;
	}

	/* Resolve materials. */
	for (i = 0 ; i < self->materials.count ; i++)
	{
		src = self->model->materials.array + i;
		dst = lirnd_material_new_from_model (self->render, src);
		if (dst == NULL)
			return 0;
		self->materials.array[i] = dst;
	}

	return 1;
}

static int
private_init_model (lirndModel* self)
{
	int i;
	limdlFaces* group;
	lirndFormat format =
	{
		12 * sizeof (float), 3,
		{ GL_FLOAT, GL_FLOAT, GL_FLOAT },
		{ 0 * sizeof (float), 2 * sizeof (float), 4 * sizeof (float) },
		GL_FLOAT, 6 * sizeof (float),
		GL_FLOAT, 9 * sizeof (float)
	};

	/* Allocate vertex buffer. */
	self->vertices = lisys_calloc (1, sizeof (lirndBuffer));
	if (self->vertices == NULL)
		return 0;

	/* Allocate vertex buffer data. */
	if (!lirnd_buffer_init_vertex (self->vertices, &format,
	     self->model->vertices.array, self->model->vertices.count))
		return 0;

	/* Allocate index buffer list. */
	self->buffers.array = lisys_calloc (self->model->facegroups.count, sizeof (lirndBuffer));
	if (self->buffers.array == NULL)
		return 0;
	self->buffers.count = self->model->facegroups.count;

	/* Allocate index buffer data. */
	for (i = 0 ; i < self->buffers.count ; i++)
	{
		group = self->model->facegroups.array + i;
		assert (group->material >= 0);
		assert (group->material < self->materials.count);
		if (!lirnd_buffer_init_index (self->buffers.array + i,
		     group->indices.array, group->indices.count))
			return 0;
	}

	return 1;
}

/** @} */
/** @} */
