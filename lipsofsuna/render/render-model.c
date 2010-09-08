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
 * \addtogroup liren Render
 * @{
 * \addtogroup LIRenModel Model
 * @{
 */

#include <lipsofsuna/system.h>
#include "render-model.h"

static void private_clear_materials (
	LIRenModel* self);

static void private_clear_model (
	LIRenModel* self);

static int private_init_materials (
	LIRenModel* self);

static int private_init_model (
	LIRenModel* self,
	int         type);

/*****************************************************************************/

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model buffer retains the ownership and must
 * ensure that the buffer is not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \param id Unique model ID.
 * \return New model or NULL.
 */
LIRenModel* liren_model_new (
	LIRenRender* render,
	LIMdlModel*  model,
	int          id)
{
	LIRenModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenModel));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->model = model;
	self->id = id;
	self->type = LIREN_BUFFER_TYPE_STATIC;

	/* Create model data. */
	if (!liren_model_set_model (self, model))
	{
		lisys_free (self);
		return NULL;
	}

	/* Add to dictionary. */
	if (!lialg_u32dic_insert (render->models, id, self))
	{
		private_clear_materials (self);
		private_clear_model (self);
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees a model.
 *
 * \param self Model.
 */
void
liren_model_free (LIRenModel* self)
{
	/* Remove from dictionary. */
	lialg_u32dic_remove (self->render->models, self->id);

	/* Free self. */
	private_clear_materials (self);
	private_clear_model (self);
	lisys_free (self);
}

void
liren_model_replace_image (LIRenModel* self,
                           LIRenImage* image)
{
	int i;
	int j;
	LIRenMaterial* material;
	LIRenTexture* texture;

	for (i = 0 ; i < self->materials.count ; i++)
	{
		material = self->materials.array[i];
		for (j = 0 ; j < material->textures.count ; j++)
		{
			texture = material->textures.array + j;
			if (texture->image == image)
				liren_texture_set_image (texture, image);
		}
	}
}

void
liren_model_get_bounds (LIRenModel* self,
                        LIMatAabb*  aabb)
{
	if (self->model != NULL)
		*aabb = self->model->bounds;
	else
		limat_aabb_init (aabb);
}

int liren_model_set_model (
	LIRenModel* self,
	LIMdlModel* model)
{
	LIAlgPtrdicIter iter0;
	LIAlgU32dicIter iter1;
	LIRenModel backup;
	LIRenObject* object;
	LIRenScene* scene;

	/* Create new model data and erase the old data. */
	backup = *self;
	self->model = model;
	if (!private_init_materials (self) ||
	    !private_init_model (self, self->type))
	{
		private_clear_materials (self);
		private_clear_model (self);
		*self = backup;
		return 0;
	}
	private_clear_materials (&backup);
	private_clear_model (&backup);

	/* We need to refresh any objects using the model since lights reference
	   the nodes of the model directly and those might have changed. */
	LIALG_PTRDIC_FOREACH (iter0, self->render->scenes)
	{
		scene = iter0.value;
		LIALG_U32DIC_FOREACH (iter1, scene->objects)
		{
			object = iter1.value;
			if (object->model == self)
				liren_object_set_model (object, self);
		}
	}

	return 1;
}

/**
 * \brief Checks if the model is static.
 *
 * \param self Model.
 * \return Nonzero if the model is static.
 */
int
liren_model_get_static (LIRenModel* self)
{
	return !self->model->animations.count;
}

int liren_model_get_type (
	const LIRenModel* self)
{
	return self->type;
}

int liren_model_set_type (
	LIRenModel* self,
	int         value)
{
	self->type = value;
	return liren_model_set_model (self, self->model);
}

/*****************************************************************************/

static void private_clear_materials (
	LIRenModel* self)
{
	int i;

	for (i = 0 ; i < self->materials.count ; i++)
	{
		if (self->materials.array[i] != NULL)
			liren_material_free (self->materials.array[i]);
	}
	lisys_free (self->materials.array);
	self->materials.array = NULL;
	self->materials.count = 0;
}

static void private_clear_model (
	LIRenModel* self)
{
	if (self->buffer != NULL)
	{
		liren_buffer_free (self->buffer);
		lisys_free (self->buffer);
		self->buffer = NULL;
	}
	lisys_free (self->groups.array);
	self->groups.array = NULL;
	self->groups.count = 0;
}

static int private_init_materials (
	LIRenModel* self)
{
	uint32_t i;
	LIMdlMaterial* src;
	LIRenMaterial* dst;

	/* Allocate materials. */
	self->materials.count = self->model->materials.count;
	if (self->materials.count)
	{
		self->materials.array = lisys_calloc (self->materials.count, sizeof (LIRenMaterial*));
		if (self->materials.array == NULL)
			return 0;
	}

	/* Resolve materials. */
	for (i = 0 ; i < self->materials.count ; i++)
	{
		src = self->model->materials.array + i;
		dst = liren_material_new_from_model (self->render, src);
		if (dst == NULL)
			return 0;
		self->materials.array[i] = dst;
	}

	return 1;
}

static int private_init_model (
	LIRenModel* self,
	int         type)
{
	int c;
	int i;
	uint32_t* indices;
	LIMdlFaces* group;
	LIRenFormat format =
	{
		8 * sizeof (float),
		GL_FLOAT, 0 * sizeof (float),
		GL_FLOAT, 2 * sizeof (float),
		GL_FLOAT, 5 * sizeof (float)
	};

	/* Allocate render buffer. */
	self->buffer = lisys_calloc (1, sizeof (LIRenBuffer));
	if (self->buffer == NULL)
		return 0;

	/* Allocate face groups. */
	self->groups.count = self->model->facegroups.count;
	if (self->groups.count)
	{
		self->groups.array = lisys_calloc (self->groups.count, sizeof (LIRenModelGroup));
		if (self->groups.array == NULL)
			return 0;
	}

	/* Calculate face group offsets. */
	for (c = i = 0 ; i < self->groups.count ; i++)
	{
		group = self->model->facegroups.array + i;
		self->groups.array[i].start = c;
		self->groups.array[i].count = group->indices.count;
		c += group->indices.count;
	}

	/* Combine the index lists. */
	if (c)
	{
		indices = lisys_calloc (c, sizeof (uint32_t));
		if (indices == NULL)
			return 0;
		for (c = i = 0 ; i < self->groups.count ; i++)
		{
			group = self->model->facegroups.array + i;
			memcpy (indices + c, group->indices.array, group->indices.count * sizeof (uint32_t));
			c += group->indices.count;
		}
	}
	else
		indices = NULL;

	/* Initialize the render buffer. */
	if (!liren_buffer_init (self->buffer, indices, c, &format,
	     self->model->vertices.array, self->model->vertices.count, type))
	{
		lisys_free (indices);
		return 0;
	}
	lisys_free (indices);

	return 1;
}

/** @} */
/** @} */
