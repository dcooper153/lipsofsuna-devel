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
 * \addtogroup LIRenModel21 Model21
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-model.h"
#include "render-private.h"
#include "../render-private.h"
#include "../render-scene.h"

static inline void private_quat_xform (
	const LIMatQuaternion q,
	const LIMatVector     v,
	LIMatVector*          o);

static inline void private_vert_xform (
	GLfloat*     buf,
	LIMdlVertex* in,
	LIMdlVertex* out);

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
LIRenModel21* liren_model21_new (
	LIRenRender21* render,
	LIMdlModel*    model,
	int            id)
{
	LIRenModel21* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenModel21));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Create model data. */
	if (!liren_model21_set_model (self, model))
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
void liren_model21_free (
	LIRenModel21* self)
{
	if (self->buffer != NULL)
		liren_buffer21_free (self->buffer);
	lisys_free (self->materials.array);
	lisys_free (self->groups.array);
	lisys_free (self->vertices.array);
	lisys_free (self);
}

int liren_model21_deform (
	LIRenModel21*    self,
	const char*      shader,
	const LIMdlPose* pose)
{
	int i;
	int j;
	int count;
	int size;
	GLfloat* data;
	LIMdlPoseGroup* group;
	LIMdlVertex* vertices;

	/* Collect pose data. */
	/* The first transformation in the list is the fallback identity
	   transformation used by vertices that don't have all four weights. */
	count = 12 * (pose->groups.count + 1);
	data = lisys_calloc (count, sizeof (GLfloat));
	if (data == NULL)
		return 0;
	j = 0;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 1.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 1.0f;
	for (i = 0 ; i < pose->groups.count ; i++)
	{
		group = pose->groups.array + i;
		data[j++] = group->head_rest.x;
		data[j++] = group->head_rest.y;
		data[j++] = group->head_rest.z;
		data[j++] = 0.0;
		data[j++] = group->head_pose.x;
		data[j++] = group->head_pose.y;
		data[j++] = group->head_pose.z;
		data[j++] = group->scale_pose;
		data[j++] = group->rotation.x;
		data[j++] = group->rotation.y;
		data[j++] = group->rotation.z;
		data[j++] = group->rotation.w;
	}

	/* Deform the mesh. */
	size = self->vertices.count * sizeof (LIMdlVertex);
	vertices = lisys_malloc (size);
	if (vertices == NULL)
	{
		lisys_free (data);
		return 0;
	}
	memcpy (vertices, self->vertices.array, size);
	for (i = 0 ; i < self->vertices.count ; i++)
		private_vert_xform (data, self->vertices.array + i, vertices + i);
	lisys_free (data);

	/* Upload the deformed mesh. */
	glBindBuffer (GL_ARRAY_BUFFER, self->buffer->vertex_buffer);
	glBufferData (GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
	glBufferData (GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
	lisys_free (vertices);

	return 1;
}

/**
 * \brief Reloads the model.
 *
 * This function is called when the video mode changes in Windows. It
 * reloads the model data that was lost when the context was erased.
 *
 * \param self Model.
 */
void liren_model21_reload (
	LIRenModel21* self)
{
	LIMdlPose* pose;

	/* TODO */

	/* Reset the pose. */
	pose = limdl_pose_new ();
	if (pose != NULL)
	{
		liren_model21_deform (self, "skeletal", pose);
		limdl_pose_free (pose);
	}
}

void liren_model21_replace_image (
	LIRenModel21* self,
	LIRenImage21* image)
{
}

/**
 * \brief Caches transparent faces for fast depth sorting.
 * \param self Model.
 */
void liren_model21_update_transparency (
	LIRenModel21* self)
{
}

void liren_model21_get_bounds (
	LIRenModel21* self,
	LIMatAabb*    aabb)
{
	*aabb = self->bounds;
}

int liren_model21_set_model (
	LIRenModel21* self,
	LIMdlModel*   model)
{
	int c;
	int i;
	int index_count;
	uint32_t* indices;
	LIAlgPtrdicIter iter0;
	LIAlgU32dicIter iter1;
	LIMdlFaces* group;
	LIMdlMaterial* src;
	LIMdlVertex* vertices;
	LIRenMaterial21* dst;
	LIRenMaterial21* materials;
	LIRenModelGroup21* groups;
	LIRenObject21* object;
	LIRenScene21* scene;
	LIRenFormat vertex_format =
	{
		16 * sizeof (float) + 8 * sizeof (char),
		GL_FLOAT, 0 * sizeof (float),
		GL_FLOAT, 2 * sizeof (float),
		GL_FLOAT, 5 * sizeof (float)
	};

	/* Create face groups. */
	if (model->facegroups.count)
	{
		groups = lisys_calloc (model->facegroups.count, sizeof (LIRenModelGroup21));
		if (groups == NULL)
			return 0;
		for (index_count = i = 0 ; i < model->facegroups.count ; i++)
		{
			group = model->facegroups.array + i;
			groups[i].start = index_count;
			groups[i].count = group->indices.count;
			index_count += group->indices.count;
		}
	}
	else
	{
		index_count = 0;
		groups = NULL;
	}

	/* Create materials. */
	if (model->materials.count)
	{
		materials = lisys_calloc (model->materials.count, sizeof (LIRenMaterial21));
		if (materials == NULL)
		{
			lisys_free (groups);
			return 0;
		}
		for (i = 0 ; i < model->materials.count ; i++)
		{
			src = model->materials.array + i;
			dst = materials + i;
			dst->shininess = LIMAT_CLAMP (src->shininess, 1.0f, 127.0f);
			dst->diffuse[0] = src->diffuse[0];
			dst->diffuse[1] = src->diffuse[1];
			dst->diffuse[2] = src->diffuse[2];
			dst->diffuse[3] = src->diffuse[3];
			dst->specular[0] = src->specular[0];
			dst->specular[1] = src->specular[1];
			dst->specular[2] = src->specular[2];
			dst->specular[3] = src->specular[3];
			if (src->textures.count && src->textures.array[0].string != NULL)
				dst->image = liren_render21_load_image (self->render, src->textures.array[0].string);
		}
	}
	else
		materials = NULL;

	/* Combine index lists. */
	if (index_count)
	{
		indices = lisys_calloc (index_count, sizeof (uint32_t));
		if (indices == NULL)
		{
			lisys_free (materials);
			lisys_free (groups);
			return 0;
		}
		for (c = i = 0 ; i < model->facegroups.count ; i++)
		{
			group = model->facegroups.array + i;
			memcpy (indices + c, group->indices.array, group->indices.count * sizeof (uint32_t));
			c += group->indices.count;
		}
	}
	else
		indices = NULL;

	/* Create a copy of the vertices. */
	/* This is needed if we need to deform the model. */
	if (model->vertices.count)
	{
		vertices = lisys_calloc (model->vertices.count, sizeof (LIMdlVertex));
		if (vertices == NULL)
		{
			lisys_free (indices);
			lisys_free (materials);
			lisys_free (groups);
			return 0;
		}
		memcpy (vertices, model->vertices.array, model->vertices.count * sizeof (LIMdlVertex));
	}
	else
		vertices = NULL;

	/* Initialize the vertex buffer. */
	if (self->buffer != NULL)
		liren_buffer21_free (self->buffer);
	if (indices != NULL)
	{
		self->buffer = liren_buffer21_new (indices, index_count, &vertex_format,
			model->vertices.array, model->vertices.count, LIREN_BUFFER_TYPE_STATIC);
		lisys_free (indices);
	}

	/* Replace materials. */
	lisys_free (self->materials.array);
	self->materials.array = materials;
	self->materials.count = model->materials.count;
	lisys_free (self->groups.array);
	self->groups.array = groups;
	self->groups.count = model->facegroups.count;
	lisys_free (self->vertices.array);
	self->vertices.array = vertices;
	self->vertices.count = model->vertices.count;

	/* We need to refresh any objects that use the model. Lights reference
	   the nodes of the model directly and changing the content of the model
	   invalidates the old node data. */
	LIALG_PTRDIC_FOREACH (iter0, self->render->scenes)
	{
		scene = iter0.value;
		LIALG_U32DIC_FOREACH (iter1, scene->scene->objects)
		{
			object = ((LIRenObject*) iter1.value)->v21;
			if (object->model == self)
				liren_object21_set_model (object, self);
		}
	}

	return 1;
}

/*****************************************************************************/
/* These are simply translated from GLSL to C. */
/* The original code is in data/lipsofsuna/scripts/client/shaders/skeletal.lua */

static inline void private_quat_xform (
	const LIMatQuaternion q,
	const LIMatVector     v,
	LIMatVector*          o)
{
	LIMatQuaternion a =
	{
		 (q.w * v.x) + (q.y * v.z) - (q.z * v.y),
		 (q.w * v.y) - (q.x * v.z) + (q.z * v.x),
		 (q.w * v.z) + (q.x * v.y) - (q.y * v.x),
		-(q.x * v.x) - (q.y * v.y) - (q.z * v.z)
	};
	o->x = (a.w * -q.x) + (a.x *  q.w) + (a.y * -q.z) - (a.z * -q.y);
	o->y = (a.w * -q.y) - (a.x * -q.z) + (a.y *  q.w) + (a.z * -q.x);
	o->z = (a.w * -q.z) + (a.x * -q.y) - (a.y * -q.x) + (a.z *  q.w);
}

static inline void private_vert_xform (
	GLfloat*     buf,
	LIMdlVertex* in,
	LIMdlVertex* out)
{
	int i;
	int offset;
	LIMatQuaternion poserot;
	LIMatVector tmp;
	LIMatVector ret;
	LIMatVector nml = { 0.0f, 0.0f, 0.0f };
	LIMatVector vtx = { 0.0f, 0.0f, 0.0f };

	for (i = 0 ; i < 8 && (i < 1 || in->bones[i]) ; i++)
	{
		offset = 12 * in->bones[i];
		poserot = limat_quaternion_init (buf[offset + 8], buf[offset + 9], buf[offset + 10], buf[offset + 11]);
		tmp.x = (in->coord.x - buf[offset + 0]) * buf[offset + 7];
		tmp.y = (in->coord.y - buf[offset + 1]) * buf[offset + 7];
		tmp.z = (in->coord.z - buf[offset + 2]) * buf[offset + 7];
		private_quat_xform (poserot, tmp, &ret);
		vtx.x += in->weights[i] * (ret.x + buf[offset + 4]);
		vtx.y += in->weights[i] * (ret.y + buf[offset + 5]);
		vtx.z += in->weights[i] * (ret.z + buf[offset + 6]);
		private_quat_xform (poserot, in->normal, &ret);
		nml.x += in->weights[i] * ret.x;
		nml.y += in->weights[i] * ret.y;
		nml.z += in->weights[i] * ret.z;
	}
	out->coord = vtx;
	out->normal = nml;
}

/** @} */
/** @} */
