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

static void
private_clear_materials (LIRenModel* self);

static void
private_clear_model (LIRenModel* self);

static int
private_init_materials (LIRenModel* self);

static int
private_init_model (LIRenModel* self);

/*****************************************************************************/

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model buffer retains the ownership and must
 * ensure that the buffer is not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \param name Unique model name or NULL.
 * \return New model or NULL.
 */
LIRenModel*
liren_model_new (LIRenRender* render,
                 LIMdlModel*  model,
                 const char*  name)
{
	LIAlgStrdicNode* node;
	LIRenModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenModel));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->model = model;

	/* Set name. */
	self->aabb = model->bounds;
	if (name != NULL)
	{
		self->name = strdup (name);
		if (self->name == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}

	/* Initialize static mesh. */
	if (!private_init_materials (self) ||
	    !private_init_model (self))
	{
		lisys_free (self->name);
		lisys_free (self);
		return NULL;
	}

	/* Add to dictionary. */
	if (name != NULL)
	{
		node = lialg_strdic_find_node (render->models, name);
		if (node == NULL)
		{
			if (!lialg_strdic_insert (render->models, name, self))
			{
				private_clear_materials (self);
				private_clear_model (self);
				lisys_free (self->name);
				lisys_free (self);
				return NULL;
			}
		}
		else
		{
			((LIRenModel*) node->value)->added = 0;
			node->value = self;
		}
	}
	else
	{
		if (!lialg_ptrdic_insert (render->models_inst, self, self))
		{
			private_clear_materials (self);
			private_clear_model (self);
			lisys_free (self);
			return NULL;
		}
	}
	self->added = 1;

	return self;
}

LIRenModel*
liren_model_new_instance (LIRenModel* model)
{
	return liren_model_new (model->render, model->model, NULL);
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
	if (self->added)
	{
		if (self->name != NULL)
			lialg_strdic_remove (self->render->models, self->name);
		else
			lialg_ptrdic_remove (self->render->models_inst, self);
	}

	/* Free self. */
	private_clear_materials (self);
	private_clear_model (self);
	lisys_free (self->name);
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
	*aabb = self->aabb;
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

/*****************************************************************************/

static void
private_clear_materials (LIRenModel* self)
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

static void
private_clear_model (LIRenModel* self)
{
	int i;

	for (i = 0 ; i < self->buffers.count ; i++)
		liren_buffer_free (self->buffers.array + i);
	if (self->vertices != NULL)
	{
		liren_buffer_free (self->vertices);
		lisys_free (self->vertices);
		self->vertices = NULL;
	}
	lisys_free (self->buffers.array);
	self->buffers.array = NULL;
	self->buffers.count = 0;
}

static int
private_init_materials (LIRenModel* self)
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

static int
private_init_model (LIRenModel* self)
{
	int i;
	LIMdlFaces* group;
	LIRenFormat format =
	{
		12 * sizeof (float), 3,
		{ GL_FLOAT, GL_FLOAT, GL_FLOAT },
		{ 0 * sizeof (float), 2 * sizeof (float), 4 * sizeof (float) },
		GL_FLOAT, 6 * sizeof (float),
		GL_FLOAT, 9 * sizeof (float)
	};

	/* Allocate vertex buffer. */
	self->vertices = lisys_calloc (1, sizeof (LIRenBuffer));
	if (self->vertices == NULL)
		return 0;

	/* Allocate vertex buffer data. */
	if (!liren_buffer_init_vertex (self->vertices, &format,
	     self->model->vertices.array, self->model->vertices.count))
		return 0;

	/* Allocate index buffer list. */
	self->buffers.array = lisys_calloc (self->model->facegroups.count, sizeof (LIRenBuffer));
	if (self->buffers.array == NULL)
		return 0;
	self->buffers.count = self->model->facegroups.count;

	/* Allocate index buffer data. */
	for (i = 0 ; i < self->buffers.count ; i++)
	{
		group = self->model->facegroups.array + i;
		assert (group->material >= 0);
		assert (group->material < self->materials.count);
		if (!liren_buffer_init_index (self->buffers.array + i,
		     group->indices.array, group->indices.count))
			return 0;
	}

	return 1;
}

/** @} */
/** @} */
