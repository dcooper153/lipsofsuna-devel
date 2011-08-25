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
 * \addtogroup LIRenObject Object
 * @{
 */

#include "lipsofsuna/network.h"
#include "lipsofsuna/system.h"
#include "render-object.h"
#include "render-private.h"
#include "render32/render-private.h"

static void private_free (
	LIRenObject* self);

/*****************************************************************************/

/**
 * \brief Creates a new render object and adds it to the scene.
 * \param render Renderer.
 * \param id Unique identifier.
 * \return Object ID or zero on failure.
 */
int liren_render_object_new (
	LIRenRender* render,
	int          id)
{
	LIRenObject* self;

	self = lisys_calloc (1, sizeof (LIRenObject));
	if (self == NULL)
		return 0;
	self->render = render;

	/* Choose a unique ID. */
	while (!id)
	{
		id = lialg_random_range (&render->random, LINET_RANGE_RENDER_START, LINET_RANGE_RENDER_END);
		if (lialg_u32dic_find (render->objects, id))
			id = 0;
	}
	self->id = id;

	/* Initialize backend. */
	if (render->v32 != NULL)
	{
		self->v32 = liren_object32_new (render->v32, id);
		if (self->v32 == NULL)
		{
			private_free (self);
			return 0;
		}
	}
	else
	{
		self->v21 = liren_object21_new (render->v21, id);
		if (self->v21 == NULL)
		{
			private_free (self);
			return 0;
		}
	}

	/* Add to dictionary. */
	if (!lialg_u32dic_insert (render->objects, id, self))
	{
		private_free (self);
		return 0;
	}

	return self->id;
}

/**
 * \brief Frees the render object.
 * \param self Renderer.
 * \param id Object ID.
 */
void liren_render_object_free (
	LIRenRender* self,
	int          id)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return;

	private_free (object);
}

/**
 * \brief Deforms the object.
 * \param self Renderer.
 * \param id Object ID.
 */
void liren_render_object_deform (
	LIRenRender* self,
	int          id)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return;

	if (self->v32 != NULL)
		liren_object32_deform (object->v32);
	else
		liren_object21_deform (object->v21);
}

/**
 * \brief Sets the particle animation state of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param start Animation offset in seconds.
 * \param loop Nonzero to enable looping.
 */
void liren_render_object_particle_animation (
	LIRenRender* self,
	int          id,
	float        start,
	int          loop)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return;

	if (self->v32 != NULL)
		liren_object32_particle_animation (object->v32, start, loop);
	else
		liren_object21_particle_animation (object->v21, start, loop);
}

/**
 * \brief Sets the effect layer of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param shader Shader name or NULL to disable the effect.
 * \param params Effect parameters or NULL.
 * \return Nonzero on success.
 */
int liren_render_object_set_effect (
	LIRenRender* self,
	int          id,
	const char*  shader,
	const float* params)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return 0;

	if (self->v32 != NULL)
	{
		if (!liren_object32_set_effect (object->v32, shader, params))
			return 0;
	}

	return 1;
}

/**
 * \brief Sets the model of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param model Model.
 * \return Nonzero on success.
 */
int liren_render_object_set_model (
	LIRenRender* self,
	int          id,
	int          model)
{
	LIRenObject* object;
	LIRenModel* model_;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return 0;
	model_ = lialg_u32dic_find (self->models, model);

	if (self->v32 != NULL)
	{
		if (!liren_object32_set_model (object->v32, (model_ != NULL)? model_->v32 : NULL))
			return 0;
		object->model = model_;
	}
	else
	{
		if (!liren_object21_set_model (object->v21, (model_ != NULL)? model_->v21 : NULL))
			return 0;
		object->model = model_;
	}

	return 1;
}

/**
 * \brief Sets the pose of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param pose Pose.
 * \return Nonzero on success.
 */
int liren_render_object_set_pose (
	LIRenRender* self,
	int          id,
	LIMdlPose*   pose)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return 0;

	if (object->v32 != NULL)
		return liren_object32_set_pose (object->v32, pose);
	else
		return liren_object21_set_pose (object->v21, pose);
}

/**
 * \brief Sets the realized flag of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param value Flag value.
 * \return Nonzero if succeeded.
 */
int liren_render_object_set_realized (
	LIRenRender* self,
	int          id,
	int          value)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return 0;

	if (object->v32 != NULL)
		return liren_object32_set_realized (object->v32, value);
	else
		return liren_object21_set_realized (object->v21, value);
}

/**
 * \brief Sets the transformation of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param value Transformation.
 */
void liren_render_object_set_transform (
	LIRenRender*          self,
	int                   id,
	const LIMatTransform* value)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return;

	if (object->v32 != NULL)
		liren_object32_set_transform (object->v32, value);
	else
		liren_object21_set_transform (object->v21, value);
}

/*****************************************************************************/

static void private_free (
	LIRenObject* self)
{
	lialg_u32dic_remove (self->render->objects, self->id);
	if (self->v32 != NULL)
		liren_object32_free (self->v32);
	if (self->v21 != NULL)
		liren_object21_free (self->v21);
	lisys_free (self);
}

/** @} */
/** @} */
