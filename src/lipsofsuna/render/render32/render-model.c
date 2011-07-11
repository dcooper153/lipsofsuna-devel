/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenModel32 Model32
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-buffer-texture.h"
#include "render-context.h"
#include "render-model.h"
#include "render-private.h"
#include "../render-private.h"
#include "../render-scene.h"

static void private_clear_materials (
	LIRenModel32* self);

static void private_clear_model (
	LIRenModel32* self);

static int private_init_materials (
	LIRenModel32* self,
	LIMdlModel*   model);

static int private_init_model (
	LIRenModel32* self,
	LIMdlModel*   model);

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
LIRenModel32* liren_model32_new (
	LIRenRender32* render,
	LIMdlModel*    model,
	int            id)
{
	LIRenModel32* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenModel32));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Create model data. */
	if (!liren_model32_set_model (self, model))
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees a model.
 * \param self Model.
 */
void liren_model32_free (
	LIRenModel32* self)
{
	/* Free self. */
	private_clear_materials (self);
	private_clear_model (self);
	lisys_free (self);
}

int liren_model32_deform (
	LIRenModel32*    self,
	const char*      shader,
	const LIMdlPose* pose)
{
	/* The vertex shader takes care of animations. */
	return 1;
}

/**
 * \brief Reloads the model.
 *
 * This function is called when the video mode changes in Windows. It
 * reloads the model data that was lost when the context was erased.
 *
 * \param self Model.
 * \param pass Reload pass.
 */
void liren_model32_reload (
	LIRenModel32* self,
	int           pass)
{
	/* Reload the vertex buffer. */
	liren_mesh32_reload (&self->mesh, pass);
}

void liren_model32_replace_image (
	LIRenModel32* self,
	LIRenImage32* image)
{
}

/**
 * \brief Caches transparent faces for fast depth sorting.
 *
 * Recalculates the center points of transparent triangles after an animation
 * has deformed the mesh. For maximum correctness of transparency, the function
 * should be called every time the mesh is deformed, but since the update
 * involves an expensive download from video memory, the user might choose to
 * trade transparency quality for better performance.
 *
 * \param self Model.
 */
void liren_model32_update_transparency (
	LIRenModel32* self)
{
	int i;
	int j;
	void* vtxdata;
	LIRenFormat format;
	LIRenModelGroup32* group;
	LIRenMaterial32* material;

	/* Update each material group. */
	for (i = 0 ; i < self->groups.count ; i++)
	{
		/* Check if transparency center is needed. */
		group = self->groups.array + i;
		group->center = limat_vector_init (0.0f, 0.0f, 0.0f);
		material = self->materials.array[i];
		if (material->shader == NULL)
			continue;
		if (!material->shader->sort)
			continue;

		/* Download the deformed vertices from video memory. */
		liren_mesh32_get_format (&self->mesh, &format);
		vtxdata = liren_mesh32_lock_vertices (&self->mesh, group->start, group->count);
		if (vtxdata == NULL)
			continue;

		/* Calculate the center of the group. */
		if (group->count > 0)
		{
			for (j = 0 ; j < group->count ; j++)
			{
				group->center = limat_vector_add (group->center,
					*((LIMatVector*)(vtxdata + format.vtx_offset + format.size * j)));
			}
			group->center = limat_vector_multiply (group->center, 1.0f / group->count);
		}

		/* Unmap the deformed vertices. */
		liren_mesh32_unlock_vertices (&self->mesh);
	}
}

void liren_model32_get_bounds (
	LIRenModel32* self,
	LIMatAabb*    aabb)
{
	*aabb = self->bounds;
}

int liren_model32_set_model (
	LIRenModel32* self,
	LIMdlModel*   model)
{
	LIAlgPtrdicIter iter0;
	LIAlgU32dicIter iter1;
	LIRenModel32 backup;
	LIRenObject32* object;
	LIRenScene32* scene;

	/* Create new model data and erase the old data. */
	backup = *self;
	self->bounds = model->bounds;
	if (!private_init_materials (self, model) ||
	    !private_init_model (self, model))
	{
		private_clear_materials (self);
		private_clear_model (self);
		*self = backup;
		return 0;
	}
	private_clear_materials (&backup);
	private_clear_model (&backup);

	/* We need to refresh any objects that use the model. Lights reference
	   the nodes of the model directly and changing the content of the model
	   invalidates the old node data. */
	LIALG_PTRDIC_FOREACH (iter0, self->render->scenes)
	{
		scene = iter0.value;
		LIALG_U32DIC_FOREACH (iter1, scene->scene->objects)
		{
			object = ((LIRenObject*) iter1.value)->v32;
			if (object->model == self)
				liren_object32_set_model (object, self);
		}
	}

	return 1;
}

/*****************************************************************************/

static void private_clear_materials (
	LIRenModel32* self)
{
	int i;

	for (i = 0 ; i < self->materials.count ; i++)
	{
		if (self->materials.array[i] != NULL)
			liren_material32_free (self->materials.array[i]);
	}
	lisys_free (self->materials.array);
	self->materials.array = NULL;
	self->materials.count = 0;
}

static void private_clear_model (
	LIRenModel32* self)
{
	liren_particles32_clear (&self->particles);
	liren_mesh32_clear (&self->mesh);
	lisys_free (self->groups.array);
	self->groups.array = NULL;
	self->groups.count = 0;
}

static int private_init_materials (
	LIRenModel32* self,
	LIMdlModel*   model)
{
	uint32_t i;
	LIMdlMaterial* src;
	LIRenMaterial32* dst;

	/* Allocate materials. */
	self->materials.count = model->materials.count;
	if (self->materials.count)
	{
		self->materials.array = lisys_calloc (self->materials.count, sizeof (LIRenMaterial32*));
		if (self->materials.array == NULL)
			return 0;
	}

	/* Resolve materials. */
	for (i = 0 ; i < self->materials.count ; i++)
	{
		src = model->materials.array + i;
		dst = liren_material32_new_from_model (self->render, src);
		if (dst == NULL)
			return 0;
		self->materials.array[i] = dst;
	}

	return 1;
}

static int private_init_model (
	LIRenModel32* self,
	LIMdlModel*   model)
{
	int c;
	int i;
	uint32_t* indices;
	LIMdlFaces* group;

	/* Allocate face groups. */
	self->groups.count = model->facegroups.count;
	if (self->groups.count)
	{
		self->groups.array = lisys_calloc (self->groups.count, sizeof (LIRenModelGroup32));
		if (self->groups.array == NULL)
			return 0;
	}

	/* Calculate face group offsets. */
	for (c = i = 0 ; i < self->groups.count ; i++)
	{
		group = model->facegroups.array + i;
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
			group = model->facegroups.array + i;
			memcpy (indices + c, group->indices.array, group->indices.count * sizeof (uint32_t));
			c += group->indices.count;
		}
	}
	else
		indices = NULL;

	/* Initialize particles. */
	liren_particles32_init (&self->particles, self->render, model);
	if (self->particles.frames.count)
		self->bounds = limat_aabb_union (self->bounds, self->particles.bounds);

	/* Initialize the render buffer. */
	if (!liren_mesh32_init (&self->mesh, indices, c,
	     model->vertices.array, model->vertices.count))
	{
		lisys_free (indices);
		return 0;
	}
	lisys_free (indices);

	/* Initialize face sorting for transparent material groups. */
	liren_model32_update_transparency (self);

	return 1;
}

/** @} */
/** @} */
